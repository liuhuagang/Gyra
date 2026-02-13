// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "GyraAbilityCost.h"
#include "ScalableFloat.h"
#include "Templates/SubclassOf.h"

#include "GyraAbilityCost_InventoryItem.generated.h"

struct FGameplayAbilityActivationInfo;
struct FGameplayAbilitySpecHandle;

class UGyraGameplayAbility;
class UGyraInventoryItemDefinition;
class UObject;
struct FGameplayAbilityActorInfo;
struct FGameplayTagContainer;

/**
 * Represents a cost that requires expending a quantity of an inventory item
 */
UCLASS(meta=(DisplayName="Inventory Item"))
class UGyraAbilityCost_InventoryItem : public UGyraAbilityCost
{
	GENERATED_BODY()

public:
	UGyraAbilityCost_InventoryItem();

	//~UGyraAbilityCost interface
	virtual bool CheckCost(const UGyraGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle,
	                       const FGameplayAbilityActorInfo* ActorInfo,
	                       FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual void ApplyCost(const UGyraGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle,
	                       const FGameplayAbilityActorInfo* ActorInfo,
	                       const FGameplayAbilityActivationInfo ActivationInfo) override;
	//~End of UGyraAbilityCost interface

protected:
	/** How much of the item to spend (keyed on ability level) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=AbilityCost)
	FScalableFloat Quantity;

	/** Which item to consume */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=AbilityCost)
	TSubclassOf<UGyraInventoryItemDefinition> ItemDefinition;
};
