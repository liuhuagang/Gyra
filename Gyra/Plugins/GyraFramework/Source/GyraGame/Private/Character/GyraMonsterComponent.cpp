// Copyright 2024 Huagang Liu. All Rights Reserved.

#include "GyraMonsterComponent.h"

#include "Components/GameFrameworkComponentDelegates.h"
#include "Logging/MessageLog.h"
#include "LogGyraGame.h"
#include "Player/GyraPlayerState.h"
#include "Character/GyraPawnExtensionComponent.h"
#include "Character/GyraPawnData.h"
#include "AbilitySystem/GyraAbilitySystemComponent.h"
#include "GyraGameplayTags.h"
#include "Components/GameFrameworkComponentManager.h"

#include "GyraMonster.h"
#include "GyraAbilitySet.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GyraMonsterComponent)

#if WITH_EDITOR
#include "Misc/UObjectToken.h"
#endif	// WITH_EDITOR


const FName UGyraMonsterComponent::NAME_ActorFeatureName("Monster");



UGyraMonsterComponent::UGyraMonsterComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UGyraMonsterComponent* UGyraMonsterComponent::FindMonsterComponent(const AActor* Actor)
{
	return (Actor ? Actor->FindComponentByClass<UGyraMonsterComponent>() : nullptr);
}

FName UGyraMonsterComponent::GetFeatureName() const
{
	return NAME_ActorFeatureName;
}



bool UGyraMonsterComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState,
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
		// The player state is not required.
		// 玩家状态是不能有的。
		if (GetPlayerState<AGyraPlayerState>())
		{
			check(false);
		}

		//AI在服务器上时,控制器必须要有
		if (Pawn->GetLocalRole() != ROLE_SimulatedProxy)
		{
			AController* Controller = GetController<AController>();

			const bool bHasControllerPairedWithPS = (Controller != nullptr);

			if (!bHasControllerPairedWithPS)
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
		// 等待其他的拓展组件即可

		return Manager->HasFeatureReachedInitState(Pawn, UGyraPawnExtensionComponent::NAME_ActorFeatureName,
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

void UGyraMonsterComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState,
                                                  FGameplayTag DesiredState)
{
	if (CurrentState == GyraGameplayTags::InitState_DataAvailable && DesiredState ==
		GyraGameplayTags::InitState_DataInitialized)
	{
		APawn* Pawn = GetPawn<APawn>();

		if (!ensure(Pawn))
		{
			return;
		}

		const UGyraPawnData* PawnData = nullptr;

		if (UGyraPawnExtensionComponent* PawnExtComp = UGyraPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
		{
			PawnData = PawnExtComp->GetPawnData<UGyraPawnData>();

			AGyraMonster* Monster = Cast<AGyraMonster>(Pawn);

			PawnExtComp->InitializeAbilitySystem(Monster->GetGyraAbilitySystemComponent(), Pawn);
			if (Pawn->GetLocalRole() != ROLE_SimulatedProxy)
			{
				//我们在这个地方统一预注册能力
				for (const UGyraAbilitySet* AbilitySet : PawnData->AbilitySets)
				{
					if (AbilitySet)
					{
						AbilitySet->GiveToAbilitySystem(Monster->GetGyraAbilitySystemComponent(), {}, nullptr);
					}
				}
			}
		}
	}
}

void UGyraMonsterComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
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

void UGyraMonsterComponent::CheckDefaultInitialization()
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


void UGyraMonsterComponent::OnRegister()
{
	Super::OnRegister();

	if (!GetPawn<APawn>())
	{
		UE_LOG(LogGyraGame, Error,
			   TEXT(
				   "[UGyraMonsterComponent::OnRegister] This component has been added to a blueprint whose base class is not a Pawn. To use this component, it MUST be placed on a Pawn Blueprint."
			   ));

#if WITH_EDITOR
		if (GIsEditor)
		{
			static const FText Message = NSLOCTEXT("GyraMonsterComponent", "NotOnPawnError",
												   "has been added to a blueprint whose base class is not a Pawn. To use this component, it MUST be placed on a Pawn Blueprint. This will cause a crash if you PIE!");
			static const FName MonsterMessageLogName = TEXT("GyraMonsterComponent");

			FMessageLog(MonsterMessageLogName).Error()
											  ->AddToken(FUObjectToken::Create(
												  this, FText::FromString(GetNameSafe(this))))
											  ->AddToken(FTextToken::Create(Message));

			FMessageLog(MonsterMessageLogName).Open();
		}
#endif
	}
	else
	{
		// Register with the init state system early, this will only work if this is a game world
		// 请尽早在初始化状态系统中进行注册，这仅适用于游戏世界环境。
		RegisterInitStateFeature();
	}
}

void UGyraMonsterComponent::BeginPlay()
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

void UGyraMonsterComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnregisterInitStateFeature();

	Super::EndPlay(EndPlayReason);
}
