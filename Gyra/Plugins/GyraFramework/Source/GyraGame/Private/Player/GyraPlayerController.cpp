// Copyright 2024 Huagang Liu. All Rights Reserved.

#include "GyraPlayerController.h"
#include "CommonInputTypeEnum.h"
#include "Components/PrimitiveComponent.h"
#include "LogGyraGame.h"
#include "GyraCheatManager.h"
#include "GyraPlayerState.h"
#include "Camera/GyraPlayerCameraManager.h"
#include "UI/GyraHUD.h"
#include "AbilitySystem/GyraAbilitySystemComponent.h"
#include "EngineUtils.h"
#include "GyraGameplayTags.h"
#include "GameFramework/Pawn.h"
#include "Net/UnrealNetwork.h"
#include "Engine/GameInstance.h"
#include "AbilitySystemGlobals.h"
#include "CommonInputSubsystem.h"
#include "GyraLocalPlayer.h"
#include "GyraGameState.h"
#include "GyraSettingsLocal.h"
#include "GyraSettingsShared.h"
#include "ReplaySubsystem.h"
#include "Development/GyraDeveloperSettings.h"
#include "GameMapsSettings.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GyraPlayerController)

namespace Gyra
{
	namespace Input
	{
		static int32 ShouldAlwaysPlayForceFeedback = 0;
		static FAutoConsoleVariableRef CVarShouldAlwaysPlayForceFeedback(TEXT("GyraPC.ShouldAlwaysPlayForceFeedback"),
			ShouldAlwaysPlayForceFeedback,
			TEXT("Should force feedback effects be played, even if the last input device was not a gamepad?"));
	}
}

AGyraPlayerController::AGyraPlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PlayerCameraManagerClass = AGyraPlayerCameraManager::StaticClass();

#if USING_CHEAT_MANAGER
	CheatClass = UGyraCheatManager::StaticClass();
#endif // #if USING_CHEAT_MANAGER
}

void AGyraPlayerController::PreInitializeComponents()
{
	Super::PreInitializeComponents();
}

void AGyraPlayerController::BeginPlay()
{
	Super::BeginPlay();
	SetActorHiddenInGame(false);
}

void AGyraPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AGyraPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Disable replicating the PC target view as it doesn't work well for replays or client-side spectating.
	// The engine TargetViewRotation is only set in APlayerController::TickActor if the server knows ahead of time that 
	// a specific pawn is being spectated and it only replicates down for COND_OwnerOnly.
	// In client-saved replays, COND_OwnerOnly is never true and the target pawn is not always known at the time of recording.
	// To support client-saved replays, the replication of this was moved to ReplicatedViewRotation and updated in PlayerTick.
	DISABLE_REPLICATED_PROPERTY(APlayerController, TargetViewRotation);
}

void AGyraPlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();
}

void AGyraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	// If we are auto running then add some player input
	if (GetIsAutoRunning())
	{
		if (APawn* CurrentPawn = GetPawn())
		{
			const FRotator MovementRotation(0.0f, GetControlRotation().Yaw, 0.0f);
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::ForwardVector);
			CurrentPawn->AddMovementInput(MovementDirection, 1.0f);
		}
	}

	AGyraPlayerState* GyraPlayerState = GetGyraPlayerState();

	if (PlayerCameraManager && GyraPlayerState)
	{
		APawn* TargetPawn = PlayerCameraManager->GetViewTargetPawn();

		if (TargetPawn)
		{
			// Update view rotation on the server so it replicates
			if (HasAuthority() || TargetPawn->IsLocallyControlled())
			{
				GyraPlayerState->SetReplicatedViewRotation(TargetPawn->GetViewRotation());
			}

			// Update the target view rotation if the pawn isn't locally controlled
			if (!TargetPawn->IsLocallyControlled())
			{
				GyraPlayerState = TargetPawn->GetPlayerState<AGyraPlayerState>();
				if (GyraPlayerState)
				{
					// Get it from the spectated pawn's player state, which may not be the same as the PC's playerstate
					TargetViewRotation = GyraPlayerState->GetReplicatedViewRotation();
				}
			}
		}
	}
}

AGyraPlayerState* AGyraPlayerController::GetGyraPlayerState() const
{
	return CastChecked<AGyraPlayerState>(PlayerState, ECastCheckedType::NullAllowed);
}

UGyraAbilitySystemComponent* AGyraPlayerController::GetGyraAbilitySystemComponent() const
{
	const AGyraPlayerState* GyraPS = GetGyraPlayerState();
	return (GyraPS ? GyraPS->GetGyraAbilitySystemComponent() : nullptr);
}

AGyraHUD* AGyraPlayerController::GetGyraHUD() const
{
	return CastChecked<AGyraHUD>(GetHUD(), ECastCheckedType::NullAllowed);
}

void AGyraPlayerController::OnPlayerStateChangedTeam(UObject* TeamAgent, int32 OldTeam, int32 NewTeam)
{
	ConditionalBroadcastTeamChanged(this, IntegerToGenericTeamId(OldTeam), IntegerToGenericTeamId(NewTeam));
}

void AGyraPlayerController::OnPlayerStateChanged()
{
	// Empty, place for derived classes to implement without having to hook all the other events
}

void AGyraPlayerController::BroadcastOnPlayerStateChanged()
{
	OnPlayerStateChanged();

	// Unbind from the old player state, if any
	FGenericTeamId OldTeamID = FGenericTeamId::NoTeam;
	if (LastSeenPlayerState != nullptr)
	{
		if (IGyraTeamAgentInterface* PlayerStateTeamInterface = Cast<IGyraTeamAgentInterface>(LastSeenPlayerState))
		{
			OldTeamID = PlayerStateTeamInterface->GetGenericTeamId();
			PlayerStateTeamInterface->GetTeamChangedDelegateChecked().RemoveAll(this);
		}
	}

	// Bind to the new player state, if any
	FGenericTeamId NewTeamID = FGenericTeamId::NoTeam;
	if (PlayerState != nullptr)
	{
		if (IGyraTeamAgentInterface* PlayerStateTeamInterface = Cast<IGyraTeamAgentInterface>(PlayerState))
		{
			NewTeamID = PlayerStateTeamInterface->GetGenericTeamId();
			PlayerStateTeamInterface->GetTeamChangedDelegateChecked().AddDynamic(this, &ThisClass::OnPlayerStateChangedTeam);
		}
	}

	// Broadcast the team change (if it really has)
	ConditionalBroadcastTeamChanged(this, OldTeamID, NewTeamID);

	LastSeenPlayerState = PlayerState;
}

void AGyraPlayerController::InitPlayerState()
{
	Super::InitPlayerState();
	BroadcastOnPlayerStateChanged();
}

void AGyraPlayerController::CleanupPlayerState()
{
	Super::CleanupPlayerState();
	BroadcastOnPlayerStateChanged();
}

void AGyraPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	BroadcastOnPlayerStateChanged();
}

void AGyraPlayerController::SetPlayer(UPlayer* InPlayer)
{
	Super::SetPlayer(InPlayer);

	if (const UGyraLocalPlayer* GyraLocalPlayer = Cast<UGyraLocalPlayer>(InPlayer))
	{
		UGyraSettingsShared* UserSettings = GyraLocalPlayer->GetSharedSettings();
		UserSettings->OnSettingChanged.AddUObject(this, &ThisClass::OnSettingsChanged);

		OnSettingsChanged(UserSettings);
	}
}

void AGyraPlayerController::OnSettingsChanged(UGyraSettingsShared* InSettings)
{
	bForceFeedbackEnabled = InSettings->GetForceFeedbackEnabled();
}

void AGyraPlayerController::AddCheats(bool bForce)
{
#if USING_CHEAT_MANAGER
	Super::AddCheats(true);
#else //#if USING_CHEAT_MANAGER
	Super::AddCheats(bForce);
#endif // #else //#if USING_CHEAT_MANAGER
}

void AGyraPlayerController::ServerCheat_Implementation(const FString& Msg)
{
#if USING_CHEAT_MANAGER
	if (CheatManager)
	{
		UE_LOG(LogGyraGame, Warning, TEXT("ServerCheat: %s"), *Msg);
		ClientMessage(ConsoleCommand(Msg));
	}
#endif // #if USING_CHEAT_MANAGER
}

bool AGyraPlayerController::ServerCheat_Validate(const FString& Msg)
{
	return true;
}

void AGyraPlayerController::ServerCheatAll_Implementation(const FString& Msg)
{
#if USING_CHEAT_MANAGER
	if (CheatManager)
	{
		UE_LOG(LogGyraGame, Warning, TEXT("ServerCheatAll: %s"), *Msg);
		for (TActorIterator<AGyraPlayerController> It(GetWorld()); It; ++It)
		{
			AGyraPlayerController* GyraPC = (*It);
			if (GyraPC)
			{
				GyraPC->ClientMessage(GyraPC->ConsoleCommand(Msg));
			}
		}
	}
#endif // #if USING_CHEAT_MANAGER
}

bool AGyraPlayerController::ServerCheatAll_Validate(const FString& Msg)
{
	return true;
}

void AGyraPlayerController::PreProcessInput(const float DeltaTime, const bool bGamePaused)
{
	Super::PreProcessInput(DeltaTime, bGamePaused);
}

void AGyraPlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	if (UGyraAbilitySystemComponent* GyraASC = GetGyraAbilitySystemComponent())
	{
		GyraASC->ProcessAbilityInput(DeltaTime, bGamePaused);
	}

	Super::PostProcessInput(DeltaTime, bGamePaused);
}

void AGyraPlayerController::OnCameraPenetratingTarget()
{
	bHideViewTargetPawnNextFrame = true;
}

void AGyraPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

#if WITH_SERVER_CODE && WITH_EDITOR
	if (GIsEditor && (InPawn != nullptr) && (GetPawn() == InPawn))
	{
		for (const FGyraCheatToRun& CheatRow : GetDefault<UGyraDeveloperSettings>()->CheatsToRun)
		{
			if (CheatRow.Phase == ECheatExecutionTime::OnPlayerPawnPossession)
			{
				ConsoleCommand(CheatRow.Cheat, /*bWriteToLog=*/ true);
			}
		}
	}
#endif

	SetIsAutoRunning(false);
}

void AGyraPlayerController::SetIsAutoRunning(const bool bEnabled)
{
	const bool bIsAutoRunning = GetIsAutoRunning();
	if (bEnabled != bIsAutoRunning)
	{
		if (!bEnabled)
		{
			OnEndAutoRun();
		}
		else
		{
			OnStartAutoRun();
		}
	}
}

bool AGyraPlayerController::GetIsAutoRunning() const
{
	bool bIsAutoRunning = false;
	if (const UGyraAbilitySystemComponent* GyraASC = GetGyraAbilitySystemComponent())
	{
		bIsAutoRunning = GyraASC->GetTagCount(GyraGameplayTags::Status_AutoRunning) > 0;
	}
	return bIsAutoRunning;
}

void AGyraPlayerController::OnStartAutoRun()
{
	if (UGyraAbilitySystemComponent* GyraASC = GetGyraAbilitySystemComponent())
	{
		GyraASC->SetLooseGameplayTagCount(GyraGameplayTags::Status_AutoRunning, 1);
		K2_OnStartAutoRun();
	}
}

void AGyraPlayerController::OnEndAutoRun()
{
	if (UGyraAbilitySystemComponent* GyraASC = GetGyraAbilitySystemComponent())
	{
		GyraASC->SetLooseGameplayTagCount(GyraGameplayTags::Status_AutoRunning, 0);
		K2_OnEndAutoRun();
	}
}

void AGyraPlayerController::UpdateForceFeedback(IInputInterface* InputInterface, const int32 ControllerId)
{
	if (bForceFeedbackEnabled)
	{
		if (const UCommonInputSubsystem* CommonInputSubsystem = UCommonInputSubsystem::Get(GetLocalPlayer()))
		{
			const ECommonInputType CurrentInputType = CommonInputSubsystem->GetCurrentInputType();
			if (Gyra::Input::ShouldAlwaysPlayForceFeedback || CurrentInputType == ECommonInputType::Gamepad || CurrentInputType == ECommonInputType::Touch)
			{
				InputInterface->SetForceFeedbackChannelValues(ControllerId, ForceFeedbackValues);
				return;
			}
		}
	}

	InputInterface->SetForceFeedbackChannelValues(ControllerId, FForceFeedbackValues());
}

void AGyraPlayerController::UpdateHiddenComponents(const FVector& ViewLocation, TSet<FPrimitiveComponentId>& OutHiddenComponents)
{
	Super::UpdateHiddenComponents(ViewLocation, OutHiddenComponents);

	if (bHideViewTargetPawnNextFrame)
	{
		AActor* const ViewTargetPawn = PlayerCameraManager ? Cast<AActor>(PlayerCameraManager->GetViewTarget()) : nullptr;
		if (ViewTargetPawn)
		{
			// internal helper func to hide all the components
			auto AddToHiddenComponents = [&OutHiddenComponents](const TInlineComponentArray<UPrimitiveComponent*>& InComponents)
				{
					// add every component and all attached children
					for (UPrimitiveComponent* Comp : InComponents)
					{
						if (Comp->IsRegistered())
						{
							OutHiddenComponents.Add(Comp->GetPrimitiveSceneId());

							for (USceneComponent* AttachedChild : Comp->GetAttachChildren())
							{
								static FName NAME_NoParentAutoHide(TEXT("NoParentAutoHide"));
								UPrimitiveComponent* AttachChildPC = Cast<UPrimitiveComponent>(AttachedChild);
								if (AttachChildPC && AttachChildPC->IsRegistered() && !AttachChildPC->ComponentTags.Contains(NAME_NoParentAutoHide))
								{
									OutHiddenComponents.Add(AttachChildPC->GetPrimitiveSceneId());
								}
							}
						}
					}
				};

			//TODO Solve with an interface.  Gather hidden components or something.
			//TODO Hiding isn't awesome, sometimes you want the effect of a fade out over a proximity, needs to bubble up to designers.

			// hide pawn's components
			TInlineComponentArray<UPrimitiveComponent*> PawnComponents;
			ViewTargetPawn->GetComponents(PawnComponents);
			AddToHiddenComponents(PawnComponents);

			//// hide weapon too
			//if (ViewTargetPawn->CurrentWeapon)
			//{
			//	TInlineComponentArray<UPrimitiveComponent*> WeaponComponents;
			//	ViewTargetPawn->CurrentWeapon->GetComponents(WeaponComponents);
			//	AddToHiddenComponents(WeaponComponents);
			//}
		}

		// we consumed it, reset for next frame
		bHideViewTargetPawnNextFrame = false;
	}
}

void AGyraPlayerController::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	UE_LOG(LogGyraTeams, Error, TEXT("You can't set the team ID on a player controller (%s); it's driven by the associated player state"), *GetPathNameSafe(this));
}

FGenericTeamId AGyraPlayerController::GetGenericTeamId() const
{
	if (const IGyraTeamAgentInterface* PSWithTeamInterface = Cast<IGyraTeamAgentInterface>(PlayerState))
	{
		return PSWithTeamInterface->GetGenericTeamId();
	}
	return FGenericTeamId::NoTeam;
}

FOnGyraTeamIndexChangedDelegate* AGyraPlayerController::GetOnTeamIndexChangedDelegate()
{
	return &OnTeamChangedDelegate;
}

void AGyraPlayerController::OnUnPossess()
{
	// Make sure the pawn that is being unpossessed doesn't remain our ASC's avatar actor
	if (APawn* PawnBeingUnpossessed = GetPawn())
	{
		if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(PlayerState))
		{
			if (ASC->GetAvatarActor() == PawnBeingUnpossessed)
			{
				ASC->SetAvatarActor(nullptr);
			}
		}
	}

	Super::OnUnPossess();
}
