// Copyright 2024 Huagang Liu. All Rights Reserved.


#include "GyraAbilitySystemGlobals.h"

#include "GyraGameplayEffectContext.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GyraAbilitySystemGlobals)

struct FGameplayEffectContext;

UGyraAbilitySystemGlobals::UGyraAbilitySystemGlobals(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

FGameplayEffectContext* UGyraAbilitySystemGlobals::AllocGameplayEffectContext() const
{
	return new FGyraGameplayEffectContext();
}

