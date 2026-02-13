// Copyright 2024 Huagang Liu. All Rights Reserved.

#include "Character/GyraHealthComponent.h"

#include "GyraAttributeSet.h"
#include "LogGyraGame.h"
#include "System/GyraAssetManager.h"
#include "System/GyraGameData.h"
#include "GyraGameplayTags.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"
#include "AbilitySystem/GyraAbilitySystemComponent.h"
#include "GyraHealthSet.h"
#include "GyraVerbMessage.h"
#include "GyraVerbMessageHelpers.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameFramework/PlayerState.h"
#include "Engine/World.h"

#include "GyraStatsSet.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GyraHealthComponent)

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Gyra_Elimination_Message, "Gyra.Elimination.Message");


UGyraHealthComponent::UGyraHealthComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);

	AbilitySystemComponent = nullptr;
	HealthSet = nullptr;
	DeathState = EGyraDeathState::NotDead;
}

void UGyraHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UGyraHealthComponent, DeathState);
}

UGyraHealthComponent* UGyraHealthComponent::FindHealthComponent(const AActor* Actor)
{
	return (Actor ? Actor->FindComponentByClass<UGyraHealthComponent>() : nullptr);
}


void UGyraHealthComponent::InitializeWithAbilitySystem(UGyraAbilitySystemComponent* InASC)
{
	AActor* Owner = GetOwner();
	check(Owner);

	if (AbilitySystemComponent)
	{
		UE_LOG(LogGyraGame, Error,
		       TEXT(
			       "GyraHealthComponent: Health component for owner [%s] has already been initialized with an ability system."
		       ), *GetNameSafe(Owner));
		return;
	}

	AbilitySystemComponent = InASC;
	if (!AbilitySystemComponent)
	{
		UE_LOG(LogGyraGame, Error,
		       TEXT("GyraHealthComponent: Cannot initialize health component for owner [%s] with NULL ability system."),
		       *GetNameSafe(Owner));
		return;
	}

	HealthSet = AbilitySystemComponent->GetSet<UGyraHealthSet>();
	if (!HealthSet)
	{
		UE_LOG(LogGyraGame, Error,
		       TEXT(
			       "GyraHealthComponent: Cannot initialize health component for owner [%s] with NULL health set on the ability system."
		       ), *GetNameSafe(Owner));
		return;
	}

	// Register to listen for attribute changes.
	HealthSet->OnHealthChanged.AddUObject(this, &ThisClass::HandleHealthChanged);
	HealthSet->OnMaxHealthChanged.AddUObject(this, &ThisClass::HandleMaxHealthChanged);
	HealthSet->OnOutOfHealth.AddUObject(this, &ThisClass::HandleOutOfHealth);

	// TEMP: Reset attributes to default values.  Eventually this will be driven by a spread sheet.
	// 这个地方的生命值初始化 需要等待人物信息组件初始化完成后才可,要不然最大生命值会不对!
	AbilitySystemComponent->SetNumericAttributeBase(UGyraHealthSet::GetHealthAttribute(), HealthSet->GetMaxHealth());


	ClearGameplayTags();

	OnHealthChanged.Broadcast(this, HealthSet->GetHealth(), HealthSet->GetHealth(), nullptr);
	OnMaxHealthChanged.Broadcast(this, HealthSet->GetHealth(), HealthSet->GetHealth(), nullptr);
}

void UGyraHealthComponent::UninitializeFromAbilitySystem()
{
	ClearGameplayTags();

	if (HealthSet)
	{
		HealthSet->OnHealthChanged.RemoveAll(this);
		HealthSet->OnMaxHealthChanged.RemoveAll(this);
		HealthSet->OnOutOfHealth.RemoveAll(this);
	}

	HealthSet = nullptr;
	AbilitySystemComponent = nullptr;
}

float UGyraHealthComponent::GetHealth() const
{
	return (HealthSet ? HealthSet->GetHealth() : 0.0f);
}

float UGyraHealthComponent::GetMaxHealth() const
{
	return (HealthSet ? HealthSet->GetMaxHealth() : 0.0f);
}

float UGyraHealthComponent::GetHealthNormalized() const
{
	if (HealthSet)
	{
		const float Health = HealthSet->GetHealth();
		const float MaxHealth = HealthSet->GetMaxHealth();

		return ((MaxHealth > 0.0f) ? (Health / MaxHealth) : 0.0f);
	}

	return 0.0f;
}

EGyraDeathState UGyraHealthComponent::GetDeathState() const
{
	return DeathState;
}

bool UGyraHealthComponent::IsDeadOrDying() const
{
	return (DeathState > EGyraDeathState::NotDead);
}
void UGyraHealthComponent::StartDeath()
{
	if (DeathState != EGyraDeathState::NotDead)
	{
		return;
	}

	DeathState = EGyraDeathState::DeathStarted;

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->SetLooseGameplayTagCount(GyraGameplayTags::Status_Death_Dying, 1);
	}

	AActor* Owner = GetOwner();
	check(Owner);

	OnDeathStarted.Broadcast(Owner);

	Owner->ForceNetUpdate();
}

void UGyraHealthComponent::FinishDeath()
{
	if (DeathState != EGyraDeathState::DeathStarted)
	{
		return;
	}

	DeathState = EGyraDeathState::DeathFinished;

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->SetLooseGameplayTagCount(GyraGameplayTags::Status_Death_Dead, 1);
	}

	AActor* Owner = GetOwner();
	check(Owner);

	OnDeathFinished.Broadcast(Owner);

	Owner->ForceNetUpdate();
}

void UGyraHealthComponent::DamageSelfDestruct(bool bFellOutOfWorld)
{
	if ((DeathState == EGyraDeathState::NotDead) && AbilitySystemComponent)
	{
		const TSubclassOf<UGameplayEffect> DamageGE = UGyraAssetManager::GetSubclass(
			UGyraGameData::Get().DamageGameplayEffect_SetByCaller);
		if (!DamageGE)
		{
			UE_LOG(LogGyraGame, Error,
			       TEXT(
				       "GyraHealthComponent: DamageSelfDestruct failed for owner [%s]. Unable to find gameplay effect [%s]."
			       ), *GetNameSafe(GetOwner()), *UGyraGameData::Get().DamageGameplayEffect_SetByCaller.GetAssetName());
			return;
		}

		FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(
			DamageGE, 1.0f, AbilitySystemComponent->MakeEffectContext());
		FGameplayEffectSpec* Spec = SpecHandle.Data.Get();

		if (!Spec)
		{
			UE_LOG(LogGyraGame, Error,
			       TEXT(
				       "GyraHealthComponent: DamageSelfDestruct failed for owner [%s]. Unable to make outgoing spec for [%s]."
			       ), *GetNameSafe(GetOwner()), *GetNameSafe(DamageGE));
			return;
		}

		Spec->AddDynamicAssetTag(GyraGameplayTags::TAG_Gameplay_DamageSelfDestruct);

		if (bFellOutOfWorld)
		{
			Spec->AddDynamicAssetTag(GyraGameplayTags::TAG_Gameplay_FellOutOfWorld);
		}

		const float DamageAmount = GetMaxHealth();

		Spec->SetSetByCallerMagnitude(GyraGameplayTags::SetByCaller_Damage, DamageAmount);
		AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*Spec);
	}
}

void UGyraHealthComponent::OnUnregister()
{
	UninitializeFromAbilitySystem();

	Super::OnUnregister();
}


void UGyraHealthComponent::ClearGameplayTags()
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->SetLooseGameplayTagCount(GyraGameplayTags::Status_Death_Dying, 0);
		AbilitySystemComponent->SetLooseGameplayTagCount(GyraGameplayTags::Status_Death_Dead, 0);
	}
}


void UGyraHealthComponent::HandleHealthChanged(AActor* DamageInstigator, AActor* DamageCauser,
                                               const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude,
                                               float OldValue, float NewValue)
{
	OnHealthChanged.Broadcast(this, OldValue, NewValue, DamageInstigator);
}

void UGyraHealthComponent::HandleMaxHealthChanged(AActor* DamageInstigator, AActor* DamageCauser,
                                                  const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude,
                                                  float OldValue, float NewValue)
{
	OnMaxHealthChanged.Broadcast(this, OldValue, NewValue, DamageInstigator);
}

void UGyraHealthComponent::HandleOutOfHealth(AActor* DamageInstigator, AActor* DamageCauser,
                                             const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude,
                                             float OldValue, float NewValue)
{
#if WITH_SERVER_CODE
	if (AbilitySystemComponent && DamageEffectSpec)
	{
		// Send the "GameplayEvent.Death" gameplay event through the owner's ability system.  This can be used to trigger a death gameplay ability.
		// 通过对象的能力系统发送“GameplayEvent.Death”这一游戏事件。此操作可用于触发死亡类游戏能力。
		{
			FGameplayEventData Payload;
			Payload.EventTag = GyraGameplayTags::GameplayEvent_Death;
			Payload.Instigator = DamageInstigator;
			Payload.Target = AbilitySystemComponent->GetAvatarActor();
			Payload.OptionalObject = DamageEffectSpec->Def;
			Payload.ContextHandle = DamageEffectSpec->GetEffectContext();
			Payload.InstigatorTags = *DamageEffectSpec->CapturedSourceTags.GetAggregatedTags();
			Payload.TargetTags = *DamageEffectSpec->CapturedTargetTags.GetAggregatedTags();
			Payload.EventMagnitude = DamageMagnitude;

			FScopedPredictionWindow NewScopedWindow(AbilitySystemComponent, true);
			AbilitySystemComponent->HandleGameplayEvent(Payload.EventTag, &Payload);
		}

		/**
		 * Send a standardized verb message that other systems can observe
		 * 发送其他系统可以观察到的标准化动词消息
		 * 
		 * 这里死亡信息会发送给
		 * BP_ShooterGameScoring_Base - 场景面板更新
		 * BP_EliminationFeedRelay - 死亡击杀通告
		 * UElimStreakProcessor 瞬间击杀
		 * UElimChainProcessor 累计击杀
		 * 
		 * 
		 */
		{
			FGyraVerbMessage Message;

			Message.Verb = TAG_Gyra_Elimination_Message;

			Message.Instigator = DamageInstigator;
			//这里是伤害的Tag方式,在GE上进行指定,比如手枪,近战,手榴弹,步枪等.
			Message.InstigatorTags = *DamageEffectSpec->CapturedSourceTags.GetAggregatedTags();
			//被击杀的对象要么是玩家或者机器人,要么是怪物
			//我们需要传递ASC的直接拥有者.
			Message.Target = UGyraVerbMessageHelpers::GetPlayerStateFromObject(AbilitySystemComponent->GetAvatarActor())
				                 ? UGyraVerbMessageHelpers::GetPlayerStateFromObject(
					                 AbilitySystemComponent->GetAvatarActor())
				                 : AbilitySystemComponent->GetAvatarActor();
			Message.TargetTags = *DamageEffectSpec->CapturedTargetTags.GetAggregatedTags();
			//@TODO: Fill out context tags, and any non-ability-system source/instigator tags
			//@TODO: Determine if it's an opposing team kill, self-own, team kill, etc...
			//@TODO：填写上下文标签，以及任何非能力系统源/煽动者标签
			//@TODO：确定是对方团队击杀，自己击杀，团队击杀等等…

			UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(GetWorld());
			MessageSystem.BroadcastMessage(Message.Verb, Message);
		}

		//@TODO: assist messages (could compute from damage dealt elsewhere)?
		
	}

#endif // #if WITH_SERVER_CODE
}

void UGyraHealthComponent::OnRep_DeathState(EGyraDeathState OldDeathState)
{
	const EGyraDeathState NewDeathState = DeathState;

	// Revert the death state for now since we rely on StartDeath and FinishDeath to change it.
	// 现在先恢复死亡状态，因为我们是通过调用 StartDeath 和 FinishDeath 来改变该状态的。
	DeathState = OldDeathState;

	if (OldDeathState > NewDeathState)
	{
		// The server is trying to set us back but we've already predicted past the server state.
		// 服务器试图让我们原地不动，但我们已经超出了服务器所处的状态范围。
		UE_LOG(LogGyraGame, Warning,
		       TEXT("GyraHealthComponent: Predicted past server death state [%d] -> [%d] for owner [%s]."),
		       (uint8)OldDeathState, (uint8)NewDeathState, *GetNameSafe(GetOwner()));
		return;
	}

	if (OldDeathState == EGyraDeathState::NotDead)
	{
		if (NewDeathState == EGyraDeathState::DeathStarted)
		{
			StartDeath();
		}
		else if (NewDeathState == EGyraDeathState::DeathFinished)
		{
			StartDeath();
			FinishDeath();
		}
		else
		{
			UE_LOG(LogGyraGame, Error,
			       TEXT("GyraHealthComponent: Invalid death transition [%d] -> [%d] for owner [%s]."),
			       (uint8)OldDeathState, (uint8)NewDeathState, *GetNameSafe(GetOwner()));
		}
	}
	else if (OldDeathState == EGyraDeathState::DeathStarted)
	{
		if (NewDeathState == EGyraDeathState::DeathFinished)
		{
			FinishDeath();
		}
		else
		{
			UE_LOG(LogGyraGame, Error,
			       TEXT("GyraHealthComponent: Invalid death transition [%d] -> [%d] for owner [%s]."),
			       (uint8)OldDeathState, (uint8)NewDeathState, *GetNameSafe(GetOwner()));
		}
	}

	ensureMsgf((DeathState == NewDeathState),
	           TEXT("GyraHealthComponent: Death transition failed [%d] -> [%d] for owner [%s]."), (uint8)OldDeathState,
	           (uint8)NewDeathState, *GetNameSafe(GetOwner()));
}

