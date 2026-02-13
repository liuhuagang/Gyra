// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "AbilitySystemComponent.h"
#include "GyraAttributeSet.h"

#include "GyraCombatSet.generated.h"

class UObject;
struct FFrame;


/**
 * UGyraCombatSet
 *
 *  Class that defines attributes that are necessary for applying damage or healing.
 *	Attribute examples include: damage, healing, attack power, and shield penetrations.
 * 
 * 定义用于施加伤害或治疗所需属性的类。
 * 属性的例子包括:伤害、治疗、攻击强度和穿透护盾。
 * 
 */
UCLASS(BlueprintType)
class UGyraCombatSet : public UGyraAttributeSet
{
	GENERATED_BODY()

public:

	UGyraCombatSet();

	ATTRIBUTE_ACCESSORS(UGyraCombatSet, BaseDamage);
	ATTRIBUTE_ACCESSORS(UGyraCombatSet, BaseHeal);

protected:

	UFUNCTION()
	void OnRep_BaseDamage(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_BaseHeal(const FGameplayAttributeData& OldValue);

private:

	// The base amount of damage to apply in the damage execution.
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BaseDamage, Category = "Gyra|Combat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData BaseDamage;

	// The base amount of healing to apply in the heal execution.
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BaseHeal, Category = "Gyra|Combat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData BaseHeal;
};

