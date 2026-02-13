// Copyright 2024 Huagang Liu. All Rights Reserved.

#include "GyraAbilityCost_InventoryItem.h"
#include "GameplayAbilitySpec.h"
#include "GameplayAbilitySpecHandle.h"

#include "GyraGameplayAbility.h"
#include "GyraInventoryManagerComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GyraAbilityCost_InventoryItem)

UGyraAbilityCost_InventoryItem::UGyraAbilityCost_InventoryItem()
{
	Quantity.SetValue(1.0f);
}

bool UGyraAbilityCost_InventoryItem::CheckCost(const UGyraGameplayAbility* Ability,
                                               const FGameplayAbilitySpecHandle Handle,
                                               const FGameplayAbilityActorInfo* ActorInfo,
                                               FGameplayTagContainer* OptionalRelevantTags) const
{
	if (AController* PC = Ability->GetControllerFromActorInfo())
	{
		if (UGyraInventoryManagerComponent* InventoryComponent = PC->GetComponentByClass<
			UGyraInventoryManagerComponent>())
		{
			const int32 AbilityLevel = Ability->GetAbilityLevel(Handle, ActorInfo);

			const float NumItemsToConsumeReal = Quantity.GetValueAtLevel(AbilityLevel);
			const int32 NumItemsToConsume = FMath::TruncToInt(NumItemsToConsumeReal);

			return InventoryComponent->GetItemStackCountByDefinition(EGyraInventoryListType::Backpack, ItemDefinition)
				>= NumItemsToConsume;
		}
	}

	return false;
}

void UGyraAbilityCost_InventoryItem::ApplyCost(const UGyraGameplayAbility* Ability,
                                               const FGameplayAbilitySpecHandle Handle,
                                               const FGameplayAbilityActorInfo* ActorInfo,
                                               const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (ActorInfo->IsNetAuthority())
	{
		if (AController* PC = Ability->GetControllerFromActorInfo())
		{
			if (UGyraInventoryManagerComponent* InventoryComponent = PC->GetComponentByClass<
				UGyraInventoryManagerComponent>())
			{
				const int32 AbilityLevel = Ability->GetAbilityLevel(Handle, ActorInfo);

				const float NumItemsToConsumeReal = Quantity.GetValueAtLevel(AbilityLevel);
				const int32 NumItemsToConsume = FMath::TruncToInt(NumItemsToConsumeReal);

				InventoryComponent->ConsumeItemStacksByDefinition(EGyraInventoryListType::Backpack, ItemDefinition,
				                                                  NumItemsToConsume);
			}
		}
	}
}
