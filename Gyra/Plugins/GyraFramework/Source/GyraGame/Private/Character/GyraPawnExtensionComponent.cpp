// Copyright 2024 Huagang Liu. All Rights Reserved.

#include "GyraPawnExtensionComponent.h"

#include "AbilitySystem/GyraAbilitySystemComponent.h"
#include "Components/GameFrameworkComponentDelegates.h"
#include "Components/GameFrameworkComponentManager.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"
#include "GyraGameplayTags.h"
#include "LogGyraGame.h"
#include "GyraPawnData.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GyraPawnExtensionComponent)

class FLifetimeProperty;
class UActorComponent;

const FName UGyraPawnExtensionComponent::NAME_ActorFeatureName("PawnExtension");


UGyraPawnExtensionComponent::UGyraPawnExtensionComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);

	PawnData = nullptr;
	AbilitySystemComponent = nullptr;
}

void UGyraPawnExtensionComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UGyraPawnExtensionComponent, PawnData);
}

FName UGyraPawnExtensionComponent::GetFeatureName() const
{
	return NAME_ActorFeatureName;
}


bool UGyraPawnExtensionComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState,
                                                     FGameplayTag DesiredState) const
{
	check(Manager);

	APawn* Pawn = GetPawn<APawn>();
	// 1.Actor和组件构造完成即可
	// 只要我们处于有效的兵状态，就视为已生成。
	if (!CurrentState.IsValid() && DesiredState == GyraGameplayTags::InitState_Spawned)
	{
		// As long as we are on a valid pawn, we count as spawned
		// 只要我们处于有效的兵状态，就视为已生成。
		if (Pawn)
		{
			return true;
		}
	}
	// 2.准备数据
	if (CurrentState == GyraGameplayTags::InitState_Spawned && DesiredState ==
		GyraGameplayTags::InitState_DataPrepared)
	{
		// 由其他组件进行收集各自的应当收集的数据		
		return true;
	}
	
	// 3.验证所需数据可用才行
	if (CurrentState == GyraGameplayTags::InitState_DataPrepared && DesiredState ==
		GyraGameplayTags::InitState_DataAvailable)
	{
		// Pawn data is required.
		if (!PawnData)
		{
			return false;
		}

		const bool bHasAuthority = Pawn->HasAuthority();
		const bool bIsLocallyControlled = Pawn->IsLocallyControlled();

		if (bHasAuthority || bIsLocallyControlled)
		{
			// Check for being possessed by a controller.
			// 检查是否被某个控制器所控制。
			if (!GetController<AController>())
			{
				return false;
			}
		}

		return true;
	}
	// 4. 等待其他所有组件都进入3阶段,中枢组件才允许第一个进入4阶段
	if (CurrentState == GyraGameplayTags::InitState_DataAvailable && DesiredState ==
		GyraGameplayTags::InitState_DataInitialized)
	{
		// Transition to initialize if all features have their data available
		// 若所有功能均已具备所需数据，则进行初始化过渡操作
		return Manager->HaveAllFeaturesReachedInitState(Pawn, GyraGameplayTags::InitState_DataAvailable);
	}
	// 5.角色初始化玩法结束
	if (CurrentState == GyraGameplayTags::InitState_DataInitialized && DesiredState ==
		GyraGameplayTags::InitState_GameplayReady)
	{
		return true;
	}

	return false;
}

void UGyraPawnExtensionComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager,
                                                        FGameplayTag CurrentState, FGameplayTag DesiredState)
{
	if (DesiredState == GyraGameplayTags::InitState_DataInitialized)
	{
		// This is currently all handled by other components listening to this state change
		// 目前这一切都是由其他监听此状态变化的组件来处理的。
	}
}

void UGyraPawnExtensionComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	// If another feature is now in DataAvailable, see if we should transition to DataInitialized
	// 如果当前另一个功能已处于“数据可用”状态，则检查是否应转为“数据已初始化”状态
	if (Params.FeatureName != NAME_ActorFeatureName)
	{
		if (Params.FeatureState == GyraGameplayTags::InitState_DataAvailable)
		{
			CheckDefaultInitialization();
		}
	}
}

void UGyraPawnExtensionComponent::CheckDefaultInitialization()
{
	// Before checking our progress, try progressing any other features we might depend on
	// 在检查当前进展之前，先尝试推进我们可能依赖的其他功能
	CheckDefaultInitializationForImplementers();

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


void UGyraPawnExtensionComponent::OnRegister()
{
	Super::OnRegister();

	const APawn* Pawn = GetPawn<APawn>();
	ensureAlwaysMsgf((Pawn != nullptr), TEXT("GyraPawnExtensionComponent on [%s] can only be added to Pawn actors."),
	                 *GetNameSafe(GetOwner()));

	TArray<UActorComponent*> PawnExtensionComponents;
	Pawn->GetComponents(UGyraPawnExtensionComponent::StaticClass(), PawnExtensionComponents);
	ensureAlwaysMsgf((PawnExtensionComponents.Num() == 1),
	                 TEXT("Only one GyraPawnExtensionComponent should exist on [%s]."), *GetNameSafe(GetOwner()));

	// Register with the init state system early, this will only work if this is a game world
	// 请尽早在初始化状态系统中进行注册，这仅适用于游戏世界环境。
	RegisterInitStateFeature();
}

void UGyraPawnExtensionComponent::BeginPlay()
{
	Super::BeginPlay();

	// Listen for changes to all features
	// 监听所有功能的变化
	BindOnActorInitStateChanged(NAME_None, FGameplayTag(), false);

	// Notifies state manager that we have spawned, then try rest of default initialization
	// 向状态管理器通知我们已启动，然后尝试完成默认初始化的剩余部分
	ensure(TryToChangeInitState(GyraGameplayTags::InitState_Spawned));
	CheckDefaultInitialization();
}

void UGyraPawnExtensionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UninitializeAbilitySystem();
	UnregisterInitStateFeature();

	Super::EndPlay(EndPlayReason);
}

void UGyraPawnExtensionComponent::SetPawnData(const UGyraPawnData* InPawnData)
{
	check(InPawnData);

	APawn* Pawn = GetPawnChecked<APawn>();

	if (Pawn->GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	if (PawnData)
	{
		UE_LOG(LogGyraGame, Error,
		       TEXT("Trying to set PawnData [%s] on pawn [%s] that already has valid PawnData [%s]."),
		       *GetNameSafe(InPawnData), *GetNameSafe(Pawn), *GetNameSafe(PawnData));
		return;
	}

	PawnData = InPawnData;

	Pawn->ForceNetUpdate();

	CheckDefaultInitialization();
}

void UGyraPawnExtensionComponent::OnRep_PawnData()
{
	CheckDefaultInitialization();
}

FName UGyraPawnExtensionComponent::GetGyraPawnName() const
{
	if (PawnData)
	{
		return PawnData->PawnInitName;
	}

	return NAME_None;
}

void UGyraPawnExtensionComponent::InitializeAbilitySystem(UGyraAbilitySystemComponent* InASC, AActor* InOwnerActor)
{
	check(InASC);
	check(InOwnerActor);

	if (AbilitySystemComponent == InASC)
	{
		// The ability system component hasn't changed.
		return;
	}

	if (AbilitySystemComponent)
	{
		// Clean up the old ability system component.
		UninitializeAbilitySystem();
	}

	APawn* Pawn = GetPawnChecked<APawn>();
	AActor* ExistingAvatar = InASC->GetAvatarActor();

	UE_LOG(LogGyraGame, Verbose, TEXT("Setting up ASC [%s] on pawn [%s] owner [%s], existing [%s] "),
	       *GetNameSafe(InASC), *GetNameSafe(Pawn), *GetNameSafe(InOwnerActor), *GetNameSafe(ExistingAvatar));

	if ((ExistingAvatar != nullptr) && (ExistingAvatar != Pawn))
	{
		UE_LOG(LogGyraGame, Log, TEXT("Existing avatar (authority=%d)"), ExistingAvatar->HasAuthority() ? 1 : 0);

		// There is already a pawn acting as the ASC's avatar, so we need to kick it out
		// This can happen on clients if they're lagged: their new pawn is spawned + possessed before the dead one is removed
		ensure(!ExistingAvatar->HasAuthority());

		if (UGyraPawnExtensionComponent* OtherExtensionComponent = FindPawnExtensionComponent(ExistingAvatar))
		{
			OtherExtensionComponent->UninitializeAbilitySystem();
		}
	}

	AbilitySystemComponent = InASC;
	AbilitySystemComponent->InitAbilityActorInfo(InOwnerActor, Pawn);

	if (ensure(PawnData))
	{
		InASC->SetTagRelationshipMapping(PawnData->TagRelationshipMapping);
	}

	OnAbilitySystemInitialized.Broadcast();
}

void UGyraPawnExtensionComponent::UninitializeAbilitySystem()
{
	if (!AbilitySystemComponent)
	{
		return;
	}

	// Uninitialize the ASC if we're still the avatar actor (otherwise another pawn already did it when they became the avatar actor)
	if (AbilitySystemComponent->GetAvatarActor() == GetOwner())
	{
		FGameplayTagContainer AbilityTypesToIgnore;
		AbilityTypesToIgnore.AddTag(GyraGameplayTags::Ability_Behavior_SurvivesDeath);

		AbilitySystemComponent->CancelAbilities(nullptr, &AbilityTypesToIgnore);
		AbilitySystemComponent->ClearAbilityInput();
		AbilitySystemComponent->RemoveAllGameplayCues();

		if (AbilitySystemComponent->GetOwnerActor() != nullptr)
		{
			AbilitySystemComponent->SetAvatarActor(nullptr);
		}
		else
		{
			// If the ASC doesn't have a valid owner, we need to clear *all* actor info, not just the avatar pairing
			AbilitySystemComponent->ClearActorInfo();
		}

		OnAbilitySystemUninitialized.Broadcast();
	}

	AbilitySystemComponent = nullptr;
}

void UGyraPawnExtensionComponent::HandleControllerChanged()
{
	if (AbilitySystemComponent && (AbilitySystemComponent->GetAvatarActor() == GetPawnChecked<APawn>()))
	{
		ensure(AbilitySystemComponent->AbilityActorInfo->OwnerActor == AbilitySystemComponent->GetOwnerActor());
		if (AbilitySystemComponent->GetOwnerActor() == nullptr)
		{
			UninitializeAbilitySystem();
		}
		else
		{
			AbilitySystemComponent->RefreshAbilityActorInfo();
		}
	}

	CheckDefaultInitialization();
}

void UGyraPawnExtensionComponent::HandlePlayerStateReplicated()
{
	CheckDefaultInitialization();
}

void UGyraPawnExtensionComponent::SetupPlayerInputComponent()
{
	CheckDefaultInitialization();
}



void UGyraPawnExtensionComponent::OnAbilitySystemInitialized_RegisterAndCall(
	FSimpleMulticastDelegate::FDelegate Delegate)
{
	if (!OnAbilitySystemInitialized.IsBoundToObject(Delegate.GetUObject()))
	{
		OnAbilitySystemInitialized.Add(Delegate);
	}

	if (AbilitySystemComponent)
	{
		Delegate.Execute();
	}
}

void UGyraPawnExtensionComponent::OnAbilitySystemUninitialized_Register(FSimpleMulticastDelegate::FDelegate Delegate)
{
	if (!OnAbilitySystemUninitialized.IsBoundToObject(Delegate.GetUObject()))
	{
		OnAbilitySystemUninitialized.Add(Delegate);
	}
}
