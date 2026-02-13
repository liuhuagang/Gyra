// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once


#pragma once

#include "AbilitySystemComponent.h"
#include "GyraAttributeSet.h"
#include "NativeGameplayTags.h"


#include "GyraMagicSet.generated.h"

class UObject;
struct FFrame;





/**
 * UGyraMagicSet
 *
 * 定义了人物的魔法属性
 * 
 * 最大魔法值,当前魔法值,恢复以及消耗
 * 
 *
 */
UCLASS(BlueprintType)
class UGyraMagicSet : public UGyraAttributeSet
{
	GENERATED_BODY()

public:

	UGyraMagicSet();

	ATTRIBUTE_ACCESSORS(UGyraMagicSet, Mana);
	ATTRIBUTE_ACCESSORS(UGyraMagicSet, MaxMana);
	ATTRIBUTE_ACCESSORS(UGyraMagicSet, Cover);
	ATTRIBUTE_ACCESSORS(UGyraMagicSet, Cost);


	// Delegate when health changes due to Cover/Cost, some information may be missing on the client
	mutable FGyraAttributeEvent OnManaChanged;

	// Delegate when max Mana changes
	mutable FGyraAttributeEvent OnMaxManaChanged;

	// Delegate to broadcast when the Mana attribute reaches zero
	mutable FGyraAttributeEvent OnOutOfMana;

protected:


	UFUNCTION()
	void OnRep_Mana(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MaxMana(const FGameplayAttributeData& OldValue);

	virtual bool PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

	void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const;

	
private:

	// The current Mana attribute.  The Mana will be capped by the max Mana attribute.  Mana is hidden from modifiers so only executions can modify it.
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Mana, Category = "Gyra|Mana", Meta = (HideFromModifiers,AllowPrivateAccess = true))
	FGameplayAttributeData Mana;

	// The current max Mana attribute.  Max Mana is an attribute since gameplay effects can modify it.
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxMana, Category = "Gyra|Mana", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxMana;

	// Used to track when the mana reaches 0.
	bool bOutOfMana;

	// Store the mana before any changes 
	float MaxManaBeforeAttributeChange;
	float ManaBeforeAttributeChange;


	// -------------------------------------------------------------------
	//	Meta Attribute (please keep attributes that aren't 'stateful' below 
	// -------------------------------------------------------------------

	// Incoming cover. This is mapped directly to +Mana
	UPROPERTY(BlueprintReadOnly,Category = "Gyra|Mana", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Cover;


	// Incoming cost. This is mapped directly to -Mana
	UPROPERTY(BlueprintReadOnly,Category = "Gyra|Stats", Meta = (HideFromModifiers,AllowPrivateAccess = true))
	FGameplayAttributeData Cost;



};

