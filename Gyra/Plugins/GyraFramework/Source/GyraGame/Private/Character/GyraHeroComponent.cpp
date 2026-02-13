// Copyright 2024 Huagang Liu. All Rights Reserved.

#include "GyraHeroComponent.h"
#include "Components/GameFrameworkComponentDelegates.h"
#include "Logging/MessageLog.h"
#include "LogGyraGame.h"
#include "EnhancedInputSubsystems.h"
#include "Player/GyraPlayerController.h"
#include "Player/GyraPlayerState.h"
#include "Player/GyraLocalPlayer.h"
#include "Character/GyraPawnExtensionComponent.h"
#include "Character/GyraPawnData.h"
#include "Character/GyraCharacter.h"
#include "AbilitySystem/GyraAbilitySystemComponent.h"
#include "Input/GyraInputConfig.h"
#include "Input/GyraInputComponent.h"
#include "Camera/GyraCameraComponent.h"
#include "GyraGameplayTags.h"
#include "Components/GameFrameworkComponentManager.h"
#include "Camera/GyraCameraMode.h"
#include "UserSettings/EnhancedInputUserSettings.h"
#include "InputMappingContext.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GyraHeroComponent)

#if WITH_EDITOR
#include "Misc/UObjectToken.h"
#endif	// WITH_EDITOR

namespace GyraHero
{
	static const float LookYawRate = 300.0f;
	static const float LookPitchRate = 165.0f;
};

const FName UGyraHeroComponent::NAME_BindInputsNow("BindInputsNow");
const FName UGyraHeroComponent::NAME_ActorFeatureName("Hero");

UGyraHeroComponent::UGyraHeroComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AbilityCameraMode = nullptr;
	bReadyToBindInputs = false;
}

UGyraHeroComponent* UGyraHeroComponent::FindHeroComponent(const AActor* Actor)
{
	return (Actor ? Actor->FindComponentByClass<UGyraHeroComponent>() : nullptr);
}

void UGyraHeroComponent::SetAbilityCameraMode(TSubclassOf<UGyraCameraMode> CameraMode,
                                              const FGameplayAbilitySpecHandle& OwningSpecHandle)
{
	if (CameraMode)
	{
		AbilityCameraMode = CameraMode;
		AbilityCameraModeOwningSpecHandle = OwningSpecHandle;
	}
}

void UGyraHeroComponent::ClearAbilityCameraMode(const FGameplayAbilitySpecHandle& OwningSpecHandle)
{
	if (AbilityCameraModeOwningSpecHandle == OwningSpecHandle)
	{
		AbilityCameraMode = nullptr;
		AbilityCameraModeOwningSpecHandle = FGameplayAbilitySpecHandle();
	}
}

void UGyraHeroComponent::AddAdditionalInputConfig(const UGyraInputConfig* InputConfig)
{
	TArray<uint32> BindHandles;

	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return;
	}

	const APlayerController* PC = GetController<APlayerController>();
	check(PC);

	const ULocalPlayer* LP = PC->GetLocalPlayer();
	check(LP);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);

	if (const UGyraPawnExtensionComponent* PawnExtComp = UGyraPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		UGyraInputComponent* GyraIC = Pawn->FindComponentByClass<UGyraInputComponent>();
		if (ensureMsgf(
			GyraIC,
			TEXT(
				"Unexpected Input Component class! The Gameplay Abilities will not be bound to their inputs. Change the input component to UGyraInputComponent or a subclass of it."
			)))
		{
			GyraIC->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityInputTagPressed,
			                           &ThisClass::Input_AbilityInputTagReleased, /*out*/ BindHandles);
		}
	}
}

void UGyraHeroComponent::RemoveAdditionalInputConfig(const UGyraInputConfig* InputConfig)
{
	//@TODO: Implement me!
}

bool UGyraHeroComponent::IsReadyToBindInputs() const
{
	return bReadyToBindInputs;
}

FName UGyraHeroComponent::GetFeatureName() const
{
	return NAME_ActorFeatureName;
}

bool UGyraHeroComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState,
                                            FGameplayTag DesiredState) const
{
	check(Manager);

	APawn* Pawn = GetPawn<APawn>();
	// 1.只要Actor和组件构造完成即可
	if (!CurrentState.IsValid() && DesiredState == GyraGameplayTags::InitState_Spawned)
	{
		// As long as we have a real pawn, let us transition
		if (Pawn)
		{
			return true;
		}
		
		return false;
	}
	// 2.准备数据
	if (CurrentState == GyraGameplayTags::InitState_Spawned && DesiredState ==
		GyraGameplayTags::InitState_DataPrepared)
	{
		// 由其他组件进行收集各自的应当收集的数据		
		return true;
	}

	// 3.验证数据可用
	if (CurrentState == GyraGameplayTags::InitState_DataPrepared && DesiredState ==
		GyraGameplayTags::InitState_DataAvailable)
	{
		// The player state is required.
		if (!GetPlayerState<AGyraPlayerState>())
		{
			return false;
		}

		// If we're authority or autonomous, we need to wait for a controller with registered ownership of the player state.
		// 如果我们是权威控制者或自主控制者，那么我们就需要等待一个拥有玩家状态注册所有权的控制器。
		if (Pawn->GetLocalRole() != ROLE_SimulatedProxy)
		{
			AController* Controller = GetController<AController>();

			const bool bHasControllerPairedWithPS = (Controller != nullptr) &&
				(Controller->PlayerState != nullptr) &&
				(Controller->PlayerState->GetOwner() == Controller);

			if (!bHasControllerPairedWithPS)
			{
				return false;
			}
		}

		const bool bIsLocallyControlled = Pawn->IsLocallyControlled();
		const bool bIsBot = Pawn->IsBotControlled();
		// 如果是本地控制的,并且也不是机器人.此时需要输入组件准备就绪.
		if (bIsLocallyControlled && !bIsBot)
		{
			AGyraPlayerController* GyraPC = GetController<AGyraPlayerController>();

			// The input component and local player is required when locally controlled.
			// 当采用本地控制时，必须提供输入组件和本地玩家。
			if (!Pawn->InputComponent || !GyraPC || !GyraPC->GetLocalPlayer())
			{
				return false;
			}
		}

		return true;
	}

	// 4.初始化数据
	if (CurrentState == GyraGameplayTags::InitState_DataAvailable && DesiredState ==
		GyraGameplayTags::InitState_DataInitialized)
	{
		// Wait for player state and extension component
		// 等待玩家状态和扩展组件的加载完成
		AGyraPlayerState* GyraPS = GetPlayerState<AGyraPlayerState>();

		// 必须等待中枢组件第一个进入数据初始化完成才行!
		return GyraPS && Manager->HasFeatureReachedInitState(Pawn, UGyraPawnExtensionComponent::NAME_ActorFeatureName,
		                                                     GyraGameplayTags::InitState_DataInitialized);
	}
	// 5.游戏玩法初始化完成
	if (CurrentState == GyraGameplayTags::InitState_DataInitialized && DesiredState ==
		GyraGameplayTags::InitState_GameplayReady)
	{
		// TODO add ability initialization checks?
		return true;
	}

	return false;
}

void UGyraHeroComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState,
                                               FGameplayTag DesiredState)
{
	// 使用可用的数据进行初始化
	if (CurrentState == GyraGameplayTags::InitState_DataAvailable && DesiredState ==
		GyraGameplayTags::InitState_DataInitialized)
	{
		APawn* Pawn = GetPawn<APawn>();
		AGyraPlayerState* GyraPS = GetPlayerState<AGyraPlayerState>();
		if (!ensure(Pawn && GyraPS))
		{
			return;
		}

		const UGyraPawnData* PawnData = nullptr;

		if (UGyraPawnExtensionComponent* PawnExtComp = UGyraPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
		{
			PawnData = PawnExtComp->GetPawnData<UGyraPawnData>();

			// The player state holds the persistent data for this player (state that persists across deaths and multiple pawns).
			// The ability system component and attribute sets live on the player state.
			// 玩家状态包含了此玩家的持久性数据（这些数据在死亡以及多个角色出现的情况下仍能保持不变）。
			// 能力系统组件和属性集都存在于玩家状态中。

			PawnExtComp->InitializeAbilitySystem(GyraPS->GetGyraAbilitySystemComponent(), GyraPS);
		}

		if (AGyraPlayerController* GyraPC = GetController<AGyraPlayerController>())
		{
			if (Pawn->InputComponent != nullptr)
			{
				InitializePlayerInput(Pawn->InputComponent);
			}
		}

		// Hook up the delegate for all pawns, in case we spectate later
		// 为所有兵种设置委托机制，以防之后我们进行旁观操作
		if (PawnData)
		{
			if (UGyraCameraComponent* CameraComponent = UGyraCameraComponent::FindCameraComponent(Pawn))
			{
				CameraComponent->DetermineCameraModeDelegate.BindUObject(this, &ThisClass::DetermineCameraMode);
			}
		}
	}
}

void UGyraHeroComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	if (Params.FeatureName == UGyraPawnExtensionComponent::NAME_ActorFeatureName)
	{
		if (Params.FeatureState == GyraGameplayTags::InitState_DataInitialized)
		{
			// If the extension component says all all other components are initialized, try to progress to next state
			// 如果扩展组件表明所有其他组件均已初始化，则尝试进入下一个状态
			CheckDefaultInitialization();
		}
	}
}

void UGyraHeroComponent::CheckDefaultInitialization()
{
	static const TArray<FGameplayTag> StateChain = {
		GyraGameplayTags::InitState_Spawned,
		GyraGameplayTags::InitState_DataPrepared,
		GyraGameplayTags::InitState_DataAvailable,
		GyraGameplayTags::InitState_DataInitialized,
		GyraGameplayTags::InitState_GameplayReady
	};

	// This will try to progress from spawned (which is only set in BeginPlay) through the data initialization stages until it gets to gameplay ready
	// 这将从“已生成”状态（该状态仅在“开始游戏”时设置）开始，依次经过数据初始化阶段，直至达到游戏准备就绪状态。
	ContinueInitStateChain(StateChain);
}

void UGyraHeroComponent::OnRegister()
{
	Super::OnRegister();

	if (!GetPawn<APawn>())
	{
		UE_LOG(LogGyraGame, Error,
		       TEXT(
			       "[UGyraHeroComponent::OnRegister] This component has been added to a blueprint whose base class is not a Pawn. To use this component, it MUST be placed on a Pawn Blueprint."
		       ));

#if WITH_EDITOR
		if (GIsEditor)
		{
			static const FText Message = NSLOCTEXT("GyraHeroComponent", "NotOnPawnError",
			                                       "has been added to a blueprint whose base class is not a Pawn. To use this component, it MUST be placed on a Pawn Blueprint. This will cause a crash if you PIE!");
			static const FName HeroMessageLogName = TEXT("GyraHeroComponent");

			FMessageLog(HeroMessageLogName).Error()
			                               ->AddToken(FUObjectToken::Create(this, FText::FromString(GetNameSafe(this))))
			                               ->AddToken(FTextToken::Create(Message));

			FMessageLog(HeroMessageLogName).Open();
		}
#endif
	}
	else
	{
		// Register with the init state system early, this will only work if this is a game world
		RegisterInitStateFeature();
	}
}


void UGyraHeroComponent::BeginPlay()
{
	Super::BeginPlay();

	// Listen for when the pawn extension component changes init state
	// 监听棋子扩展组件的初始化状态发生变化的情况
	BindOnActorInitStateChanged(UGyraPawnExtensionComponent::NAME_ActorFeatureName, FGameplayTag(), false);

	// Notifies that we are done spawning, then try the rest of initialization
	// 通知我们已完成生成操作，然后尝试执行其余的初始化工作
	ensure(TryToChangeInitState(GyraGameplayTags::InitState_Spawned));
	CheckDefaultInitialization();
}

void UGyraHeroComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnregisterInitStateFeature();

	Super::EndPlay(EndPlayReason);
}

void UGyraHeroComponent::InitializePlayerInput(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return;
	}

	const APlayerController* PC = GetController<APlayerController>();
	check(PC);

	const UGyraLocalPlayer* LP = Cast<UGyraLocalPlayer>(PC->GetLocalPlayer());
	check(LP);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);

	Subsystem->ClearAllMappings();

	if (const UGyraPawnExtensionComponent* PawnExtComp = UGyraPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		if (const UGyraPawnData* PawnData = PawnExtComp->GetPawnData<UGyraPawnData>())
		{
			if (const UGyraInputConfig* InputConfig = PawnData->InputConfig)
			{
				for (const FInputMappingContextAndPriority& Mapping : DefaultInputMappings)
				{
					if (UInputMappingContext* IMC = Mapping.InputMapping.Get())
					{
						if (Mapping.bRegisterWithSettings)
						{
							if (UEnhancedInputUserSettings* Settings = Subsystem->GetUserSettings())
							{
								Settings->RegisterInputMappingContext(IMC);
							}

							FModifyContextOptions Options = {};
							Options.bIgnoreAllPressedKeysUntilRelease = false;
							// Actually add the config to the local player							
							Subsystem->AddMappingContext(IMC, Mapping.Priority, Options);
						}
					}
				}

				// The Gyra Input Component has some additional functions to map Gameplay Tags to an Input Action.
				// If you want this functionality but still want to change your input component class, make it a subclass
				// of the UGyraInputComponent or modify this component accordingly.
				UGyraInputComponent* GyraIC = Cast<UGyraInputComponent>(PlayerInputComponent);
				if (ensureMsgf(
					GyraIC,
					TEXT(
						"Unexpected Input Component class! The Gameplay Abilities will not be bound to their inputs. Change the input component to UGyraInputComponent or a subclass of it."
					)))
				{
					// Add the key mappings that may have been set by the player
					GyraIC->AddInputMappings(InputConfig, Subsystem);

					// This is where we actually bind and input action to a gameplay tag, which means that Gameplay Ability Blueprints will
					// be triggered directly by these input actions Triggered events. 
					TArray<uint32> BindHandles;
					GyraIC->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityInputTagPressed,
					                           &ThisClass::Input_AbilityInputTagReleased, /*out*/ BindHandles);

					GyraIC->BindNativeAction(InputConfig, GyraGameplayTags::InputTag_Move, ETriggerEvent::Triggered,
					                         this, &ThisClass::Input_Move, /*bLogIfNotFound=*/ false);
					GyraIC->BindNativeAction(InputConfig, GyraGameplayTags::InputTag_Look_Mouse,
					                         ETriggerEvent::Triggered, this, &ThisClass::Input_LookMouse,
					                         /*bLogIfNotFound=*/ false);
					GyraIC->BindNativeAction(InputConfig, GyraGameplayTags::InputTag_Look_Stick,
					                         ETriggerEvent::Triggered, this, &ThisClass::Input_LookStick,
					                         /*bLogIfNotFound=*/ false);
					GyraIC->BindNativeAction(InputConfig, GyraGameplayTags::InputTag_Crouch, ETriggerEvent::Triggered,
					                         this, &ThisClass::Input_Crouch, /*bLogIfNotFound=*/ false);
					GyraIC->BindNativeAction(InputConfig, GyraGameplayTags::InputTag_AutoRun, ETriggerEvent::Triggered,
					                         this, &ThisClass::Input_AutoRun, /*bLogIfNotFound=*/ false);
				}
			}
		}
	}

	if (ensure(!bReadyToBindInputs))
	{
		bReadyToBindInputs = true;
	}

	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(
		const_cast<APlayerController*>(PC), NAME_BindInputsNow);
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(
		const_cast<APawn*>(Pawn), NAME_BindInputsNow);
}


void UGyraHeroComponent::Input_AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (const APawn* Pawn = GetPawn<APawn>())
	{
		if (const UGyraPawnExtensionComponent* PawnExtComp =
			UGyraPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
		{
			if (UGyraAbilitySystemComponent* GyraASC = PawnExtComp->GetGyraAbilitySystemComponent())
			{
				GyraASC->AbilityInputTagPressed(InputTag);
			}
		}
	}
}

void UGyraHeroComponent::Input_AbilityInputTagReleased(FGameplayTag InputTag)
{
	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return;
	}

	if (const UGyraPawnExtensionComponent* PawnExtComp = UGyraPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		if (UGyraAbilitySystemComponent* GyraASC = PawnExtComp->GetGyraAbilitySystemComponent())
		{
			GyraASC->AbilityInputTagReleased(InputTag);
		}
	}
}

void UGyraHeroComponent::Input_Move(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();
	AController* Controller = Pawn ? Pawn->GetController() : nullptr;

	// If the player has attempted to move again then cancel auto running
	if (AGyraPlayerController* GyraController = Cast<AGyraPlayerController>(Controller))
	{
		GyraController->SetIsAutoRunning(false);
	}

	if (Controller)
	{
		const FVector2D Value = InputActionValue.Get<FVector2D>();
		const FRotator MovementRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);

		if (Value.X != 0.0f)
		{
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::RightVector);
			Pawn->AddMovementInput(MovementDirection, Value.X);
		}

		if (Value.Y != 0.0f)
		{
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::ForwardVector);
			Pawn->AddMovementInput(MovementDirection, Value.Y);
		}
	}
}

void UGyraHeroComponent::Input_LookMouse(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();

	if (!Pawn)
	{
		return;
	}

	const FVector2D Value = InputActionValue.Get<FVector2D>();

	if (Value.X != 0.0f)
	{
		Pawn->AddControllerYawInput(Value.X);
	}

	if (Value.Y != 0.0f)
	{
		Pawn->AddControllerPitchInput(Value.Y);
	}
}

void UGyraHeroComponent::Input_LookStick(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();

	if (!Pawn)
	{
		return;
	}

	const FVector2D Value = InputActionValue.Get<FVector2D>();

	const UWorld* World = GetWorld();
	check(World);

	if (Value.X != 0.0f)
	{
		Pawn->AddControllerYawInput(Value.X * GyraHero::LookYawRate * World->GetDeltaSeconds());
	}

	if (Value.Y != 0.0f)
	{
		Pawn->AddControllerPitchInput(Value.Y * GyraHero::LookPitchRate * World->GetDeltaSeconds());
	}
}

void UGyraHeroComponent::Input_Crouch(const FInputActionValue& InputActionValue)
{
	if (AGyraCharacter* Character = GetPawn<AGyraCharacter>())
	{
		Character->ToggleCrouch();
	}
}

void UGyraHeroComponent::Input_AutoRun(const FInputActionValue& InputActionValue)
{
	if (APawn* Pawn = GetPawn<APawn>())
	{
		if (AGyraPlayerController* Controller = Cast<AGyraPlayerController>(Pawn->GetController()))
		{
			// Toggle auto running
			Controller->SetIsAutoRunning(!Controller->GetIsAutoRunning());
		}
	}
}

TSubclassOf<UGyraCameraMode> UGyraHeroComponent::DetermineCameraMode() const
{
	if (AbilityCameraMode)
	{
		return AbilityCameraMode;
	}

	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return nullptr;
	}

	if (UGyraPawnExtensionComponent* PawnExtComp = UGyraPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		if (const UGyraPawnData* PawnData = PawnExtComp->GetPawnData<UGyraPawnData>())
		{
			return PawnData->DefaultCameraMode;
		}
	}

	return nullptr;
}
