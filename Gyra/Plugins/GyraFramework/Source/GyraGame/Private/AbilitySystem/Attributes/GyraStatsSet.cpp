// Copyright 2024 Huagang Liu. All Rights Reserved.

#include "GyraStatsSet.h"

#include "Net/UnrealNetwork.h"
#include "Engine/World.h"
#include "GameplayEffectExtension.h"

#include "GyraGameplayTags.h"
#include "GyraGameplayTags_Attribute.h"

#include "GyraAttributeSet.h"
#include "GyraAbilitySystemComponent.h"

#include "GameFramework/GameplayMessageSubsystem.h"
#include "GyraVerbMessage.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GyraStatsSet)

class FLifetimeProperty;


UGyraStatsSet::UGyraStatsSet()
	: Attribute_S_F_Level(1.0f)
	, Attribute_S_F_EXP(0.0f)
	, Attribute_S_F_RewardEXP(1.0f)
	, Attribute_S_F_MaxEXP(100.0f)
	, Attribute_S_F_Glod(0.0f)
	, Attribute_S_F_RewardGlod(0.0f)
	, Attribute_S_V_MaxHealth(100.0f)
	, Attribute_S_R_MaxHealth(0.0f)
	, Attribute_S_V_MaxMana(100.f)
	, Attribute_S_R_MaxMana(0.f)
	, Attribute_S_V_FightRecoverHealth(0.f)
	, Attribute_S_V_OutOfFightRecoverHealth(0.f)
	, Attribute_S_V_FightRecoverMana(0.f)
	, Attribute_S_V_OutOfFightRecoverMana(0.f)
	, Attribute_A_V_Base(0.f)
	, Attribute_A_R_Base(0.f)
	, Attribute_A_V_CriticalStrikePercentage(0.f)
	, Attribute_A_R_WeakStrike(0.f)
	, Attribute_A_R_HitAgain(0.f)
	, Attribute_A_V_Gemophagia(0.f)
	, Attribute_A_R_Gemophagia(0.f)
	, Attribute_D_V_Base(0.f)
	, Attribute_D_R_Base(0.f)
	, Attribute_D_V_Dodge(0.f)
	, Attribute_D_V_Block(0.f)
{




}




void UGyraStatsSet::BroadcastAttributeChanage(AActor* EffectInstigator,
	AActor* EffectCauser,
	const FGameplayEffectSpec* EffectSpec,
	FGameplayTag GameplayTag_Attribute,
	float EffectMagnitude,
	float OldValue,
	float NewValue) const
{
	OnChanged_StatsAttribute.Broadcast(EffectInstigator,
		EffectCauser,
		EffectSpec,
		GameplayTag_Attribute,
		EffectMagnitude,
		OldValue,
		NewValue);

}

bool UGyraStatsSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{

	return Super::PreGameplayEffectExecute(Data);

}


void UGyraStatsSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	const FGameplayEffectContextHandle& EffectContext = Data.EffectSpec.GetEffectContext();
	AActor* Instigator = EffectContext.GetOriginalInstigator();
	AActor* Causer = EffectContext.GetEffectCauser();

	float CurrentAttributeValue =0.0f;
	FGameplayTag GameplayTag_Attribute;
	
	GameplayTag_Attribute =AttributeToTag(Data.EvaluatedData.Attribute);
	CurrentAttributeValue = AttributeToValue(Data.EvaluatedData.Attribute);

	BroadcastAttributeChanage(Instigator,
		Causer,
		&Data.EffectSpec,
		GameplayTag_Attribute,
		Data.EvaluatedData.Magnitude,
		CurrentAttributeValue,
		CurrentAttributeValue);

}

void UGyraStatsSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);
}

void UGyraStatsSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
}

void UGyraStatsSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	BroadcastAttributeChanage(nullptr,nullptr,nullptr, AttributeToTag(Attribute), OldValue - NewValue, OldValue, NewValue);
}
//这里使用宏进行简化
#define XG_DOREPLIFETIME_CONDITION_NOTIFY(AttributeTagName) \
	DOREPLIFETIME_CONDITION_NOTIFY(UGyraStatsSet, AttributeTagName, COND_None, REPNOTIFY_Always);

void UGyraStatsSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);


	DOREPLIFETIME_CONDITION_NOTIFY(UGyraStatsSet, Attribute_S_F_Level, COND_None, REPNOTIFY_Always);
	XG_DOREPLIFETIME_CONDITION_NOTIFY(Attribute_S_F_EXP);
	XG_DOREPLIFETIME_CONDITION_NOTIFY(Attribute_S_F_RewardEXP);
	XG_DOREPLIFETIME_CONDITION_NOTIFY(Attribute_S_F_MaxEXP);
	XG_DOREPLIFETIME_CONDITION_NOTIFY(Attribute_S_F_Glod);
	XG_DOREPLIFETIME_CONDITION_NOTIFY(Attribute_S_F_RewardGlod);

	XG_DOREPLIFETIME_CONDITION_NOTIFY(Attribute_S_V_MaxHealth);
	XG_DOREPLIFETIME_CONDITION_NOTIFY(Attribute_S_R_MaxHealth);
	XG_DOREPLIFETIME_CONDITION_NOTIFY(Attribute_S_V_MaxMana);
	XG_DOREPLIFETIME_CONDITION_NOTIFY(Attribute_S_R_MaxMana);
	XG_DOREPLIFETIME_CONDITION_NOTIFY(Attribute_S_V_FightRecoverHealth);
	XG_DOREPLIFETIME_CONDITION_NOTIFY(Attribute_S_V_OutOfFightRecoverHealth);
	XG_DOREPLIFETIME_CONDITION_NOTIFY(Attribute_S_V_FightRecoverMana);
	XG_DOREPLIFETIME_CONDITION_NOTIFY(Attribute_S_V_OutOfFightRecoverMana);

	XG_DOREPLIFETIME_CONDITION_NOTIFY(Attribute_A_V_Base);
	XG_DOREPLIFETIME_CONDITION_NOTIFY(Attribute_A_R_Base);
	XG_DOREPLIFETIME_CONDITION_NOTIFY(Attribute_A_V_CriticalStrikePercentage);
	XG_DOREPLIFETIME_CONDITION_NOTIFY(Attribute_A_R_CriticalStrike);
	XG_DOREPLIFETIME_CONDITION_NOTIFY(Attribute_A_R_WeakStrike);
	XG_DOREPLIFETIME_CONDITION_NOTIFY(Attribute_A_R_HitAgain);
	XG_DOREPLIFETIME_CONDITION_NOTIFY(Attribute_A_V_Gemophagia);
	XG_DOREPLIFETIME_CONDITION_NOTIFY(Attribute_A_R_Gemophagia);

	XG_DOREPLIFETIME_CONDITION_NOTIFY(Attribute_D_V_Base);
	XG_DOREPLIFETIME_CONDITION_NOTIFY(Attribute_D_R_Base);
	XG_DOREPLIFETIME_CONDITION_NOTIFY(Attribute_D_V_Dodge);
	XG_DOREPLIFETIME_CONDITION_NOTIFY(Attribute_D_V_Block);


}

//这里使用宏进行简化
#define XG_OnRep(AttributeTagName)\
void UGyraStatsSet::OnRep_##AttributeTagName(const FGameplayAttributeData& OldValue)\
{\
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGyraStatsSet, AttributeTagName, OldValue);\
}


XG_OnRep(Attribute_S_F_Level)
XG_OnRep(Attribute_S_F_EXP)
XG_OnRep(Attribute_S_F_RewardEXP)
XG_OnRep(Attribute_S_F_MaxEXP)
XG_OnRep(Attribute_S_F_Glod)
XG_OnRep(Attribute_S_F_RewardGlod)

XG_OnRep(Attribute_S_V_MaxHealth)
XG_OnRep(Attribute_S_R_MaxHealth)
XG_OnRep(Attribute_S_V_MaxMana)
XG_OnRep(Attribute_S_R_MaxMana)
XG_OnRep(Attribute_S_V_FightRecoverHealth)
XG_OnRep(Attribute_S_V_OutOfFightRecoverHealth)
XG_OnRep(Attribute_S_V_FightRecoverMana)
XG_OnRep(Attribute_S_V_OutOfFightRecoverMana)

XG_OnRep(Attribute_A_V_Base)
XG_OnRep(Attribute_A_R_Base)
XG_OnRep(Attribute_A_V_CriticalStrikePercentage)
XG_OnRep(Attribute_A_R_CriticalStrike)
XG_OnRep(Attribute_A_R_WeakStrike)
XG_OnRep(Attribute_A_R_HitAgain)
XG_OnRep(Attribute_A_V_Gemophagia)
XG_OnRep(Attribute_A_R_Gemophagia)

XG_OnRep(Attribute_D_V_Base)
XG_OnRep(Attribute_D_R_Base)
XG_OnRep(Attribute_D_V_Dodge)
XG_OnRep(Attribute_D_V_Block)



//这里使用宏进行简化
#define XG_TagToValue(AttributeTagName)\
if (InAttributeTag == GyraGameplayTags::AttributeTagName)\
{\
	Value = Get##AttributeTagName();\
}

float UGyraStatsSet::TagToValue(const FGameplayTag& InAttributeTag) const
{
	float Value = 0.0f;

	XG_TagToValue(Attribute_S_F_Level);
	XG_TagToValue(Attribute_S_F_EXP);
	XG_TagToValue(Attribute_S_F_RewardEXP);
	XG_TagToValue(Attribute_S_F_MaxEXP);
	XG_TagToValue(Attribute_S_F_Glod);
	XG_TagToValue(Attribute_S_F_RewardGlod);


	XG_TagToValue(Attribute_S_V_MaxHealth);
	XG_TagToValue(Attribute_S_R_MaxHealth);
	XG_TagToValue(Attribute_S_V_MaxMana);
	XG_TagToValue(Attribute_S_R_MaxMana);
	XG_TagToValue(Attribute_S_V_FightRecoverHealth);
	XG_TagToValue(Attribute_S_V_OutOfFightRecoverHealth);
	XG_TagToValue(Attribute_S_V_FightRecoverMana);
	XG_TagToValue(Attribute_S_V_OutOfFightRecoverMana);


	XG_TagToValue(Attribute_A_V_Base);
	XG_TagToValue(Attribute_A_R_Base);
	XG_TagToValue(Attribute_A_V_CriticalStrikePercentage);
	XG_TagToValue(Attribute_A_R_CriticalStrike);
	XG_TagToValue(Attribute_A_R_WeakStrike);
	XG_TagToValue(Attribute_A_R_HitAgain);
	XG_TagToValue(Attribute_A_V_Gemophagia);
	XG_TagToValue(Attribute_A_R_Gemophagia);

	XG_TagToValue(Attribute_D_V_Base);
	XG_TagToValue(Attribute_D_R_Base);
	XG_TagToValue(Attribute_D_V_Dodge);
	XG_TagToValue(Attribute_D_V_Block);


	return Value;
}

//这里使用宏进行简化
#define XG_TagToAttribute(AttributeTagName)\
if (InAttributeTag == GyraGameplayTags::AttributeTagName)\
{\
	GameplayAttribute = Get##AttributeTagName##Attribute();\
}

FGameplayAttribute UGyraStatsSet::TagToAttribute(const FGameplayTag& InAttributeTag)
{
	FGameplayAttribute GameplayAttribute;

	XG_TagToAttribute(Attribute_S_F_Level);
	XG_TagToAttribute(Attribute_S_F_EXP);
	XG_TagToAttribute(Attribute_S_F_RewardEXP);
	XG_TagToAttribute(Attribute_S_F_MaxEXP);
	XG_TagToAttribute(Attribute_S_F_Glod);
	XG_TagToAttribute(Attribute_S_F_RewardGlod);

	XG_TagToAttribute(Attribute_S_V_MaxHealth);
	XG_TagToAttribute(Attribute_S_R_MaxHealth);
	XG_TagToAttribute(Attribute_S_V_MaxMana);
	XG_TagToAttribute(Attribute_S_R_MaxMana);
	XG_TagToAttribute(Attribute_S_V_FightRecoverHealth);
	XG_TagToAttribute(Attribute_S_V_OutOfFightRecoverHealth);
	XG_TagToAttribute(Attribute_S_V_FightRecoverMana);
	XG_TagToAttribute(Attribute_S_V_OutOfFightRecoverMana);


	XG_TagToAttribute(Attribute_A_V_Base);
	XG_TagToAttribute(Attribute_A_R_Base);
	XG_TagToAttribute(Attribute_A_V_CriticalStrikePercentage);
	XG_TagToAttribute(Attribute_A_R_CriticalStrike);
	XG_TagToAttribute(Attribute_A_R_WeakStrike);
	XG_TagToAttribute(Attribute_A_R_HitAgain);
	XG_TagToAttribute(Attribute_A_V_Gemophagia);
	XG_TagToAttribute(Attribute_A_R_Gemophagia);


	XG_TagToAttribute(Attribute_D_V_Base);
	XG_TagToAttribute(Attribute_D_R_Base);
	XG_TagToAttribute(Attribute_D_V_Dodge);
	XG_TagToAttribute(Attribute_D_V_Block);


	check(GameplayAttribute.IsValid());

	return GameplayAttribute;
}

//这里使用宏进行简化
#define XG_AtttibuteToTag(AttributeTagName)\
if(InAttribute == Get##AttributeTagName##Attribute())\
{\
	GameplayAttributeTag = GyraGameplayTags::AttributeTagName;\
}
FGameplayTag UGyraStatsSet::AttributeToTag(const FGameplayAttribute& InAttribute) const
{
	FGameplayTag GameplayAttributeTag;

	XG_AtttibuteToTag(Attribute_S_F_Level);
	XG_AtttibuteToTag(Attribute_S_F_EXP);
	XG_AtttibuteToTag(Attribute_S_F_RewardEXP);
	XG_AtttibuteToTag(Attribute_S_F_MaxEXP);
	XG_AtttibuteToTag(Attribute_S_F_Glod);
	XG_AtttibuteToTag(Attribute_S_F_RewardGlod);


	XG_AtttibuteToTag(Attribute_S_V_MaxHealth);
	XG_AtttibuteToTag(Attribute_S_R_MaxHealth);
	XG_AtttibuteToTag(Attribute_S_V_MaxMana);
	XG_AtttibuteToTag(Attribute_S_R_MaxMana);
	XG_AtttibuteToTag(Attribute_S_V_FightRecoverHealth);
	XG_AtttibuteToTag(Attribute_S_V_OutOfFightRecoverHealth);
	XG_AtttibuteToTag(Attribute_S_V_FightRecoverMana);
	XG_AtttibuteToTag(Attribute_S_V_OutOfFightRecoverMana);


	XG_AtttibuteToTag(Attribute_A_V_Base);
	XG_AtttibuteToTag(Attribute_A_R_Base);
	XG_AtttibuteToTag(Attribute_A_V_CriticalStrikePercentage);
	XG_AtttibuteToTag(Attribute_A_R_CriticalStrike);
	XG_AtttibuteToTag(Attribute_A_R_WeakStrike);
	XG_AtttibuteToTag(Attribute_A_R_HitAgain);
	XG_AtttibuteToTag(Attribute_A_V_Gemophagia);
	XG_AtttibuteToTag(Attribute_A_R_Gemophagia);


	XG_AtttibuteToTag(Attribute_D_V_Base);
	XG_AtttibuteToTag(Attribute_D_R_Base);
	XG_AtttibuteToTag(Attribute_D_V_Dodge);
	XG_AtttibuteToTag(Attribute_D_V_Block);

	return GameplayAttributeTag;
}


float UGyraStatsSet::AttributeToValue(const FGameplayAttribute& InAttribute) const
{

	float Value = 0.0f;
	FGameplayTag GameplayAttributeTag = AttributeToTag(InAttribute);

	Value = TagToValue(GameplayAttributeTag);

	return Value;

}