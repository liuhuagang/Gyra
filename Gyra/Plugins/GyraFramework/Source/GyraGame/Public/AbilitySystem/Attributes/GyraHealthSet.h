// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "AbilitySystemComponent.h"
#include "GyraAttributeSet.h"
#include "NativeGameplayTags.h"

#include "GyraHealthSet.generated.h"

class UObject;
struct FFrame;



struct FGameplayEffectModCallbackData;


/**
 * UGyraHealthSet
 *
 *	Class that defines attributes that are necessary for taking damage.
 *	Attribute examples include: health, shields, and resistances.
 * 
 * 类定义的属性是必要的伤害。
 * 属性的例子包括：生命值、护盾和抗性
 * 
 */
UCLASS(BlueprintType)
class UGyraHealthSet : public UGyraAttributeSet
{
	GENERATED_BODY()

public:

	UGyraHealthSet();

	ATTRIBUTE_ACCESSORS(UGyraHealthSet, Health);
	ATTRIBUTE_ACCESSORS(UGyraHealthSet, MaxHealth);
	ATTRIBUTE_ACCESSORS(UGyraHealthSet, Healing);
	ATTRIBUTE_ACCESSORS(UGyraHealthSet, Damage);

	// Delegate when health changes due to damage/healing, some information may be missing on the client
	mutable FGyraAttributeEvent OnHealthChanged;

	// Delegate when max health changes
	mutable FGyraAttributeEvent OnMaxHealthChanged;

	// Delegate to broadcast when the health attribute reaches zero
	mutable FGyraAttributeEvent OnOutOfHealth;

protected:

	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldValue);

	virtual bool PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

	void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const;

private:

	// The current health attribute.  The health will be capped by the max health attribute.  Health is hidden from modifiers so only executions can modify it.
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "Gyra|Health", Meta = (HideFromModifiers, AllowPrivateAccess = true))
	FGameplayAttributeData Health;

	// The current max health attribute.  Max health is an attribute since gameplay effects can modify it.
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Gyra|Health", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxHealth;

	// Used to track when the health reaches 0.
	bool bOutOfHealth;

	// Store the health before any changes 
	float MaxHealthBeforeAttributeChange;
	float HealthBeforeAttributeChange;

	// -------------------------------------------------------------------
	//	Meta Attribute (please keep attributes that aren't 'stateful' below 
	// -------------------------------------------------------------------

	// Incoming healing. This is mapped directly to +Health
	UPROPERTY(BlueprintReadOnly, Category = "Gyra|Health", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Healing;

	// Incoming damage. This is mapped directly to -Health
	UPROPERTY(BlueprintReadOnly, Category = "Gyra|Health", Meta = (HideFromModifiers, AllowPrivateAccess = true))
	FGameplayAttributeData Damage;
};
