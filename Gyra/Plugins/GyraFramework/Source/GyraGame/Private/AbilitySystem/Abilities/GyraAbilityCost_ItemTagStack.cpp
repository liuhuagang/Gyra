// Copyright 2024 Huagang Liu. All Rights Reserved.

#include "GyraAbilityCost_ItemTagStack.h"

#include "Equipment/GyraGameplayAbility_FromEquipment.h"
#include "Inventory/GyraInventoryItemInstance.h"
#include "NativeGameplayTags.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GyraAbilityCost_ItemTagStack)

UE_DEFINE_GAMEPLAY_TAG(TAG_ABILITY_FAIL_COST, "Ability.ActivateFail.Cost");

UGyraAbilityCost_ItemTagStack::UGyraAbilityCost_ItemTagStack()
{
	Quantity.SetValue(1.0f);
	FailureTag = TAG_ABILITY_FAIL_COST;
}

bool UGyraAbilityCost_ItemTagStack::CheckCost(const UGyraGameplayAbility* Ability,
                                              const FGameplayAbilitySpecHandle Handle,
                                              const FGameplayAbilityActorInfo* ActorInfo,
                                              FGameplayTagContainer* OptionalRelevantTags) const
{
	if (const UGyraGameplayAbility_FromEquipment* EquipmentAbility = Cast<const
		UGyraGameplayAbility_FromEquipment>(Ability))
	{
		if (UGyraInventoryItemInstance* ItemInstance = EquipmentAbility->GetAssociatedItem())
		{
			const int32 AbilityLevel = Ability->GetAbilityLevel(Handle, ActorInfo);

			const float NumStacksReal = Quantity.GetValueAtLevel(AbilityLevel);
			const int32 NumStacks = FMath::TruncToInt(NumStacksReal);
			const bool bCanApplyCost = ItemInstance->GetStatTagStackCount(Tag) >= NumStacks;

			// Inform other abilities why this cost cannot be applied
			if (!bCanApplyCost && OptionalRelevantTags && FailureTag.IsValid())
			{
				OptionalRelevantTags->AddTag(FailureTag);
			}
			return bCanApplyCost;
		}
	}
	return false;
}

void UGyraAbilityCost_ItemTagStack::ApplyCost(const UGyraGameplayAbility* Ability,
                                              const FGameplayAbilitySpecHandle Handle,
                                              const FGameplayAbilityActorInfo* ActorInfo,
                                              const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (ActorInfo->IsNetAuthority())
	{
		if (const UGyraGameplayAbility_FromEquipment* EquipmentAbility = Cast<const
			UGyraGameplayAbility_FromEquipment>(Ability))
		{
			if (UGyraInventoryItemInstance* ItemInstance = EquipmentAbility->GetAssociatedItem())
			{
				const int32 AbilityLevel = Ability->GetAbilityLevel(Handle, ActorInfo);

				const float NumStacksReal = Quantity.GetValueAtLevel(AbilityLevel);
				const int32 NumStacks = FMath::TruncToInt(NumStacksReal);

				ItemInstance->RemoveStatTagStack(Tag, NumStacks);
			}
		}
	}
}
