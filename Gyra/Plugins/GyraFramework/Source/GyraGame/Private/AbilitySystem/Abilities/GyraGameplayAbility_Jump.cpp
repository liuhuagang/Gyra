// Copyright 2024 Huagang Liu. All Rights Reserved.

#include "GyraGameplayAbility_Jump.h"

#include "GyraGameplayAbility.h"
#include "Character/GyraCharacter.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GyraGameplayAbility_Jump)

struct FGameplayTagContainer;


UGyraGameplayAbility_Jump::UGyraGameplayAbility_Jump(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

bool UGyraGameplayAbility_Jump::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!ActorInfo || !ActorInfo->AvatarActor.IsValid())
	{
		return false;
	}

	const AGyraCharacter* GyraCharacter = Cast<AGyraCharacter>(ActorInfo->AvatarActor.Get());
	if (!GyraCharacter || !GyraCharacter->CanJump())
	{
		return false;
	}

	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	return true;
}

void UGyraGameplayAbility_Jump::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	// Stop jumping in case the ability blueprint doesn't call it.
	CharacterJumpStop();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGyraGameplayAbility_Jump::CharacterJumpStart()
{
	if (AGyraCharacter* GyraCharacter = GetGyraCharacterFromActorInfo())
	{
		if (GyraCharacter->IsLocallyControlled() && !GyraCharacter->bPressedJump)
		{
			GyraCharacter->UnCrouch();
			GyraCharacter->Jump();
		}
	}
}

void UGyraGameplayAbility_Jump::CharacterJumpStop()
{
	if (AGyraCharacter* GyraCharacter = GetGyraCharacterFromActorInfo())
	{
		if (GyraCharacter->IsLocallyControlled() && GyraCharacter->bPressedJump)
		{
			GyraCharacter->StopJumping();
		}
	}
}

