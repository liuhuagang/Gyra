// Copyright 2024 Huagang Liu. All Rights Reserved.

#include "GyraAbilityCost_PlayerTagStack.h"

#include "GameFramework/Controller.h"
#include "GyraGameplayAbility.h"
#include "Player/GyraPlayerState.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GyraAbilityCost_PlayerTagStack)

UGyraAbilityCost_PlayerTagStack::UGyraAbilityCost_PlayerTagStack()
{
	Quantity.SetValue(1.0f);
}

bool UGyraAbilityCost_PlayerTagStack::CheckCost(const UGyraGameplayAbility* Ability,
                                                const FGameplayAbilitySpecHandle Handle,
                                                const FGameplayAbilityActorInfo* ActorInfo,
                                                FGameplayTagContainer* OptionalRelevantTags) const
{
	if (AController* PC = Ability->GetControllerFromActorInfo())
	{
		if (AGyraPlayerState* PS = Cast<AGyraPlayerState>(PC->PlayerState))
		{
			const int32 AbilityLevel = Ability->GetAbilityLevel(Handle, ActorInfo);

			const float NumStacksReal = Quantity.GetValueAtLevel(AbilityLevel);
			const int32 NumStacks = FMath::TruncToInt(NumStacksReal);

			return PS->GetStatTagStackCount(Tag) >= NumStacks;
		}
	}
	return false;
}

void UGyraAbilityCost_PlayerTagStack::ApplyCost(const UGyraGameplayAbility* Ability,
                                                const FGameplayAbilitySpecHandle Handle,
                                                const FGameplayAbilityActorInfo* ActorInfo,
                                                const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (ActorInfo->IsNetAuthority())
	{
		if (AController* PC = Ability->GetControllerFromActorInfo())
		{
			if (AGyraPlayerState* PS = Cast<AGyraPlayerState>(PC->PlayerState))
			{
				const int32 AbilityLevel = Ability->GetAbilityLevel(Handle, ActorInfo);

				const float NumStacksReal = Quantity.GetValueAtLevel(AbilityLevel);
				const int32 NumStacks = FMath::TruncToInt(NumStacksReal);

				PS->RemoveStatTagStack(Tag, NumStacks);
			}
		}
	}
}
