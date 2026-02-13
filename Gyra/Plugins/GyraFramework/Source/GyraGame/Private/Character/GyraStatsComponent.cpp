// Copyright 2024 Huagang Liu. All Rights Reserved.

#include "GyraStatsComponent.h"

#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameFramework/PlayerState.h"
#include "Components/GameFrameworkComponentManager.h"
#include "Engine/World.h"

#include "GyraPawnInterface.h"
#include "GyraAbilitySystemComponent.h"
#include "GyraPawnExtensionComponent.h"
#include "GyraPawnInitInfo.h"
#include "DataRegistrySubsystem.h"


#include "GyraGameSettings.h"
#include "GyraPawnData.h"
#include "GyraPawnInitInfo.h"
#include "GyraAssetManager.h"
#include "GyraGameData.h"

#include "GyraVerbMessage.h"
#include "GyraVerbMessageHelpers.h"

#include "GyraGameplayTags.h"
#include "GyraGameplayTags_Attribute.h"

#include "LogGyraGame.h"

#if WITH_EDITOR
#include "Misc/UObjectToken.h"
#endif	// WITH_EDITOR

#include UE_INLINE_GENERATED_CPP_BY_NAME(GyraStatsComponent)
static FName NAME_CharacterAttributeSetRegistryID("CharacterAttributeSet");
const FName UGyraStatsComponent::NAME_ActorFeatureName("Stats");


UGyraStatsComponent::UGyraStatsComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;

	// 这个组件不需要网络复制
	SetIsReplicatedByDefault(false);

	AbilitySystemComponent = nullptr;

	MagicSet = nullptr;
	StatsSet = nullptr;
}

UGyraStatsComponent* UGyraStatsComponent::FindStatsComponent(const AActor* Actor)
{
	return (Actor ? Actor->FindComponentByClass<UGyraStatsComponent>() : nullptr);
}

void UGyraStatsComponent::InitializeWithAbilitySystem(UGyraAbilitySystemComponent* InASC)
{
	AActor* Owner = GetOwner();
	check(Owner);

	if (AbilitySystemComponent)
	{
		UE_LOG(LogGyraGame, Error,
		       TEXT(
			       "GyraStatsComponent: Stats component for owner [%s] has already been initialized with an ability system."
		       ), *GetNameSafe(Owner));
		return;
	}

	AbilitySystemComponent = InASC;
	if (!AbilitySystemComponent)
	{
		UE_LOG(LogGyraGame, Error,
		       TEXT("GyraStatsComponent: Cannot initialize Stats component for owner [%s] with NULL ability system."),
		       *GetNameSafe(Owner));
		return;
	}

	MagicSet = AbilitySystemComponent->GetSet<UGyraMagicSet>();
	StatsSet = AbilitySystemComponent->GetSet<UGyraStatsSet>();
	HealthSet = AbilitySystemComponent->GetSet<UGyraHealthSet>();

	if (!HealthSet)
	{
		UE_LOG(LogGyraGame, Error,
		       TEXT(
			       "GyraStatsComponent: Cannot initialize Stats component for owner [%s] with NULL HealthSet on the ability system."
		       ), *GetNameSafe(Owner));
		return;
	}

	if (!MagicSet)
	{
		UE_LOG(LogGyraGame, Error,
		       TEXT(
			       "GyraStatsComponent: Cannot initialize Stats component for owner [%s] with NULL MagicSet on the ability system."
		       ), *GetNameSafe(Owner));
		return;
	}

	if (!StatsSet)
	{
		UE_LOG(LogGyraGame, Error,
		       TEXT(
			       "GyraStatsComponent: Cannot initialize Stats component for owner [%s] with NULL StatsSet on the ability system."
		       ), *GetNameSafe(Owner));
		return;
	}

	// Register to listen for attribute changes.
	// 注册关于属性改变的监听
	StatsSet->OnChanged_StatsAttribute.AddUObject(this, &ThisClass::HandleChanged_StatsAttribute);

	// TEMP: Reset attributes to default values.  Eventually this will be driven by a spread sheet.
	//在这里通过表格初始化角色数据
	PawnInitInfo.InitAttributeFromInfo(AbilitySystemComponent);
}

void UGyraStatsComponent::UninitializeFromAbilitySystem()
{
	if (StatsSet)
	{
		StatsSet->OnChanged_StatsAttribute.RemoveAll(this);
	}

	HealthSet = nullptr;
	MagicSet = nullptr;
	StatsSet = nullptr;

	AbilitySystemComponent = nullptr;
}

float UGyraStatsComponent::GetStats(FGameplayTag InGameplayTag_Attribute) const
{
	if (!StatsSet)
	{
		return 0.0f;
	}

	return StatsSet->TagToValue(InGameplayTag_Attribute);
}

FName UGyraStatsComponent::GetFeatureName() const
{
	return NAME_ActorFeatureName;
}



bool UGyraStatsComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState,
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
	
	// 2.准备数据阶段 需要有PawnData
	if (CurrentState == GyraGameplayTags::InitState_Spawned && DesiredState ==
		GyraGameplayTags::InitState_DataPrepared)
	{

		UGyraPawnExtensionComponent* PawnExtComp = UGyraPawnExtensionComponent::FindPawnExtensionComponent(GetOwner());
		
		if (!PawnExtComp)
		{
			return false;
		}
		
		const UGyraPawnData* PawnData = PawnExtComp->GetPawnData<UGyraPawnData>();

		if (!PawnData)
		{
			return false;
		}

		// 我们已经进行过数据准备了
		if (bApplyForInit)
		{
			return false;
		}			
		
		return true;
	}
	// 必须等待异步查表完成之后,才能往下一个阶段过渡!
	if (CurrentState == GyraGameplayTags::InitState_DataPrepared && DesiredState ==
		GyraGameplayTags::InitState_DataAvailable)
	{
		return bPawnAttributeInit;
	}
	
	if (CurrentState == GyraGameplayTags::InitState_DataAvailable && DesiredState ==
		GyraGameplayTags::InitState_DataInitialized)
	{
		return true;
	}
	
	if (CurrentState == GyraGameplayTags::InitState_DataInitialized && DesiredState ==
		GyraGameplayTags::InitState_GameplayReady)
	{
		return true;
	}

	return false;
}

void UGyraStatsComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState,
                                                FGameplayTag DesiredState)
{
	//我们在1->2这个阶段去读取数据
	if (CurrentState == GyraGameplayTags::InitState_Spawned && DesiredState == GyraGameplayTags::InitState_DataPrepared)
	{
		if (bApplyForInit)
		{
			
			return;
			
		}
		else
		{
			//这里是为了限制多次去查询,这样会出bug和断点
			bApplyForInit = true;
			
			UGyraPawnExtensionComponent* PawnExtComp = UGyraPawnExtensionComponent::FindPawnExtensionComponent(
				GetOwner());
			
			const UGyraPawnData* PawnData = PawnExtComp->GetPawnData<UGyraPawnData>();

			//通过DataRegister进行初始化管理
			FDataRegistryId ItemID(NAME_CharacterAttributeSetRegistryID, PawnData->PawnInitName);

			// 在这里所有的物理 魔法 状态等信息都只是读取完成了 
			// 具体反馈到属性集上面需要有HeroComp或者Monster初始化ASC才来反馈
			if (!UDataRegistrySubsystem::Get()->AcquireItem(
				ItemID, FDataRegistryItemAcquiredCallback::CreateUObject(this, &ThisClass::OnRegistryLoadCompleted)))
			{
				UE_LOG(LogGyraGame, Error,
				       TEXT("Failed to find CharacterAttributeSet registry for tag %s, Attribute will be not right "),
				       *PawnData->PawnInitName.ToString());
			}
			
		}
	}
	
	
}

void UGyraStatsComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	if (Params.FeatureName == UGyraPawnExtensionComponent::NAME_ActorFeatureName)
	{
		if (Params.FeatureState == GyraGameplayTags::InitState_DataAvailable)
		{
			CheckDefaultInitialization();
		}
	}
}

void UGyraStatsComponent::CheckDefaultInitialization()
{
	static const TArray<FGameplayTag> StateChain = {
		GyraGameplayTags::InitState_Spawned, 
		GyraGameplayTags::InitState_DataPrepared,
		GyraGameplayTags::InitState_DataAvailable, 
		GyraGameplayTags::InitState_DataInitialized,
		GyraGameplayTags::InitState_GameplayReady
	};

	ContinueInitStateChain(StateChain);
}



void UGyraStatsComponent::HandleChanged_StatsAttribute(AActor* DamageInstigator, AActor* DamageCauser,
                                                       const FGameplayEffectSpec* DamageEffectSpec,
                                                       FGameplayTag GameplayTag_Attribute, float DamageMagnitude,
                                                       float OldValue, float NewValue)
{
	// 这里需要判断一下是否修改了最大生命值,如果修改了需要通知HealthSet
	if (GameplayTag_Attribute == GyraGameplayTags::Attribute_S_V_MaxHealth)
	{
		AbilitySystemComponent->SetNumericAttributeBase(UGyraHealthSet::GetMaxHealthAttribute(), NewValue);
	}
	// 如果是修改了最大魔法值也应当通知MagicSet
	if (GameplayTag_Attribute == GyraGameplayTags::Attribute_S_V_MaxMana)
	{
		AbilitySystemComponent->SetNumericAttributeBase(UGyraMagicSet::GetMaxManaAttribute(), NewValue);
	}
	//@TODO:这里或许应该还需要加入关于经验值的判定

	OnChanged_StatsAttribute.Broadcast(this, GameplayTag_Attribute, OldValue, NewValue, DamageInstigator);
}

void UGyraStatsComponent::OnRegistryLoadCompleted(const FDataRegistryAcquireResult& AccoladeHandle)
{
	if (const FGyraPawnInitInfo* PawnInitInfoPtr = AccoladeHandle.GetItem<FGyraPawnInitInfo>())
	{
		PawnInitInfo = *PawnInitInfoPtr;
		bPawnAttributeInit = true;
	}
	else
	{
		ensure(false);
	}

	CheckDefaultInitialization();
}

void UGyraStatsComponent::OnRegister()
{
	Super::OnRegister();

	if (!GetPawn<APawn>())
	{
		UE_LOG(LogGyraGame, Error,
		       TEXT(
			       "[GyraStatsComponent::OnRegister] This component has been added to a blueprint whose base class is not a Pawn. To use this component, it MUST be placed on a Pawn Blueprint."
		       ));

#if WITH_EDITOR
		if (GIsEditor)
		{
			static const FText Message = NSLOCTEXT("GyraStatsComponent", "NotOnPawnError",
			                                       "has been added to a blueprint whose base class is not a Pawn. To use this component, it MUST be placed on a Pawn Blueprint. This will cause a crash if you PIE!");
			static const FName StatsMessageLogName = TEXT("GyraStatsComponent");

			FMessageLog(StatsMessageLogName).Error()
			                               ->AddToken(FUObjectToken::Create(this, FText::FromString(GetNameSafe(this))))
			                               ->AddToken(FTextToken::Create(Message));

			FMessageLog(StatsMessageLogName).Open();
		}
#endif
	}
	else
	{
		RegisterInitStateFeature();
	}
}
void UGyraStatsComponent::OnUnregister()
{
	UninitializeFromAbilitySystem();

	Super::OnUnregister();
}

void UGyraStatsComponent::BeginPlay()
{
	Super::BeginPlay();

	// Listen for when the pawn extension component changes init state
	// 监听当兵棋扩展组件的初始化状态发生变化时
	BindOnActorInitStateChanged(UGyraPawnExtensionComponent::NAME_ActorFeatureName, FGameplayTag(), false);

	// Notifies that we are done spawning, then try the rest of initialization
	// 通知我们已完成生成操作，然后尝试执行其余的初始化步骤
	ensure(TryToChangeInitState(GyraGameplayTags::InitState_Spawned));
	CheckDefaultInitialization();
}

void UGyraStatsComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnregisterInitStateFeature();

	Super::EndPlay(EndPlayReason);
}
