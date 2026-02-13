// Copyright 2024 Huagang Liu. All Rights Reserved.

#include "GyraMagicSet.h"

#include "Net/UnrealNetwork.h"
#include "Engine/World.h"
#include "GameplayEffectExtension.h"

#include "GameFramework/GameplayMessageSubsystem.h"

#include "GyraAbilitySystemComponent.h"
#include "GyraAttributeSet.h"
#include "GyraGameplayTags.h"
#include "GyraVerbMessage.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GyraMagicSet)





class FLifetimeProperty;

UGyraMagicSet::UGyraMagicSet()
	:Mana(100.f),
	MaxMana(100.f)
{

	bOutOfMana = false;

	MaxManaBeforeAttributeChange = 0.0f;
	ManaBeforeAttributeChange = 0.0f;

}

void UGyraMagicSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UGyraMagicSet, Mana, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGyraMagicSet, MaxMana, COND_OwnerOnly, REPNOTIFY_Always);

}

void UGyraMagicSet::OnRep_Mana(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGyraMagicSet, Mana, OldValue);
}

void UGyraMagicSet::OnRep_MaxMana(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGyraMagicSet, MaxMana, OldValue);
}

bool UGyraMagicSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
	if (!Super::PreGameplayEffectExecute(Data))
	{
		return false;
	}

	// Handle modifying incoming normal cost
	if (Data.EvaluatedData.Attribute == GetCostAttribute())
	{
		if (Data.EvaluatedData.Magnitude > 0.0f)
		{
			const bool bIsDamageFromSelfDestruct = Data.EffectSpec.GetDynamicAssetTags().HasTagExact(GyraGameplayTags::TAG_Gameplay_DamageSelfDestruct);

			if (Data.Target.HasMatchingGameplayTag(GyraGameplayTags::TAG_Gameplay_DamageImmunity) && !bIsDamageFromSelfDestruct)
			{
				// Do not take away any mana.
				Data.EvaluatedData.Magnitude = 0.0f;
				return false;
			}

#if !UE_BUILD_SHIPPING
			// Check GodMode cheat, unlimited mana is checked below
			if (Data.Target.HasMatchingGameplayTag(GyraGameplayTags::Cheat_GodMode) && !bIsDamageFromSelfDestruct)
			{
				// Do not take away any mana.
				Data.EvaluatedData.Magnitude = 0.0f;
				return false;
			}
#endif // #if !UE_BUILD_SHIPPING
		}
	}

	// Save the current mana
	ManaBeforeAttributeChange = GetMana();
	MaxManaBeforeAttributeChange = GetMaxMana();

	return true;
}

void UGyraMagicSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	const bool bIsDamageFromSelfDestruct = Data.EffectSpec.GetDynamicAssetTags().HasTagExact(GyraGameplayTags::TAG_Gameplay_DamageSelfDestruct);
	float MinimumMana = 0.0f;

#if !UE_BUILD_SHIPPING
	// Godmode and unlimited Mana stop death unless it's a self destruct
	if (!bIsDamageFromSelfDestruct &&
		(Data.Target.HasMatchingGameplayTag(GyraGameplayTags::Cheat_GodMode) || Data.Target.HasMatchingGameplayTag(GyraGameplayTags::Cheat_UnlimitedHealth)))
	{
		MinimumMana = 1.0f;
	}
#endif // #if !UE_BUILD_SHIPPING

	const FGameplayEffectContextHandle& EffectContext = Data.EffectSpec.GetEffectContext();
	AActor* Instigator = EffectContext.GetOriginalInstigator();
	AActor* Causer = EffectContext.GetEffectCauser();

	if (Data.EvaluatedData.Attribute == GetCostAttribute())
	{
		// Send a standardized verb message that other systems can observe
		if (Data.EvaluatedData.Magnitude > 0.0f)
		{
			FGyraVerbMessage Message;
			Message.Verb = GyraGameplayTags::TAG_Gyra_Damage_Message;
			Message.Instigator = Data.EffectSpec.GetEffectContext().GetEffectCauser();
			Message.InstigatorTags = *Data.EffectSpec.CapturedSourceTags.GetAggregatedTags();
			Message.Target = GetOwningActor();
			Message.TargetTags = *Data.EffectSpec.CapturedTargetTags.GetAggregatedTags();
			//@TODO: Fill out context tags, and any non-ability-system source/instigator tags
			//@TODO: Determine if it's an opposing team kill, self-own, team kill, etc...
			Message.Magnitude = Data.EvaluatedData.Magnitude;

			UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(GetWorld());
			MessageSystem.BroadcastMessage(Message.Verb, Message);
		}

		// Convert into -Mana and then clamp
		SetMana(FMath::Clamp(GetMana() - GetMana(), MinimumMana, GetMaxMana()));
		SetMana(0.0f);
	}
	else if (Data.EvaluatedData.Attribute == GetCoverAttribute())
	{
		// Convert into +Health and then clamo
		SetMana(FMath::Clamp(GetMana() + GetCover(), MinimumMana, GetMaxMana()));
		SetCover(0.0f);
	}
	else if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		// Clamp and fall into out of mana handling below
		SetMana(FMath::Clamp(GetMana(), MinimumMana, GetMaxMana()));
	}
	else if (Data.EvaluatedData.Attribute == GetMaxManaAttribute())
	{
		// TODO clamp current mana?

		// Notify on any requested max mana changes
		OnMaxManaChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, MaxManaBeforeAttributeChange, GetMaxMana());
	}

	// If mana has actually changed activate callbacks
	if (GetMana() != ManaBeforeAttributeChange)
	{
		OnManaChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, ManaBeforeAttributeChange, GetMana());
	}

	if ((GetMana() <= 0.0f) && !bOutOfMana)
	{
		OnOutOfMana.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, ManaBeforeAttributeChange, GetMana());
	}

	// Check mana again in case an event above changed it.
	bOutOfMana = (GetMana() <= 0.0f);
}

void UGyraMagicSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);

	ClampAttribute(Attribute, NewValue);
}

void UGyraMagicSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	ClampAttribute(Attribute, NewValue);
}
void UGyraMagicSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	if (Attribute == GetMaxManaAttribute())
	{
		// Make sure current mana is not greater than the new max mana.
		if (GetMana() > NewValue)
		{
			UGyraAbilitySystemComponent* GyraASC = GetGyraAbilitySystemComponent();
			check(GyraASC);

			GyraASC->ApplyModToAttribute(GetManaAttribute(), EGameplayModOp::Override, NewValue);
		}
	}

	if (bOutOfMana && (GetMana() > 0.0f))
	{
		bOutOfMana = false;
	}
}

void UGyraMagicSet::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
{
	if (Attribute == GetManaAttribute())
	{
		// Do not allow Mana to go negative or above max Mana.
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxMana());
	}
	else if (Attribute == GetMaxManaAttribute())
	{
		// Do not allow max mana to drop below 1.
		NewValue = FMath::Max(NewValue, 1.0f);
	}
}

