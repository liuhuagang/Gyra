// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "GameplayTagContainer.h"
#include "GyraAbilityCost.h"
#include "ScalableFloat.h"

#include "GyraAbilityCost_PlayerTagStack.generated.h"

struct FGameplayAbilityActivationInfo;
struct FGameplayAbilitySpecHandle;

class UGyraGameplayAbility;
class UObject;
struct FGameplayAbilityActorInfo;

/**
 * Represents a cost that requires expending a quantity of a tag stack on the player state
 */
UCLASS(meta=(DisplayName="Player Tag Stack"))
class UGyraAbilityCost_PlayerTagStack : public UGyraAbilityCost
{
	GENERATED_BODY()

public:
	UGyraAbilityCost_PlayerTagStack();

	//~UGyraAbilityCost interface
	virtual bool CheckCost(const UGyraGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle,
	                       const FGameplayAbilityActorInfo* ActorInfo,
	                       FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual void ApplyCost(const UGyraGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle,
	                       const FGameplayAbilityActorInfo* ActorInfo,
	                       const FGameplayAbilityActivationInfo ActivationInfo) override;
	//~End of UGyraAbilityCost interface

protected:
	/** How much of the tag to spend (keyed on ability level) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Costs)
	FScalableFloat Quantity;

	/** Which tag to spend some of */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Costs)
	FGameplayTag Tag;
};
