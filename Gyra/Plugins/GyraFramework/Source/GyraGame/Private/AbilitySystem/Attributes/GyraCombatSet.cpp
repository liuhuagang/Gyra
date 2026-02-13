// Copyright 2024 Huagang Liu. All Rights Reserved.

#include "GyraCombatSet.h"

#include "GyraAttributeSet.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GyraCombatSet)

class FLifetimeProperty;


UGyraCombatSet::UGyraCombatSet()
	: BaseDamage(0.0f)
	, BaseHeal(0.0f)
{
}

void UGyraCombatSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UGyraCombatSet, BaseDamage, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGyraCombatSet, BaseHeal, COND_OwnerOnly, REPNOTIFY_Always);
}

void UGyraCombatSet::OnRep_BaseDamage(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGyraCombatSet, BaseDamage, OldValue);
}

void UGyraCombatSet::OnRep_BaseHeal(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGyraCombatSet, BaseHeal, OldValue);
}


