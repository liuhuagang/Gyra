// Copyright 2024 Huagang Liu. All Rights Reserved.

#include "GyraAttributeSet.h"

#include "GyraAbilitySystemComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GyraAttributeSet)

class UWorld;


UGyraAttributeSet::UGyraAttributeSet()
{
}

UWorld* UGyraAttributeSet::GetWorld() const
{
	const UObject* Outer = GetOuter();
	check(Outer);

	return Outer->GetWorld();
}

UGyraAbilitySystemComponent* UGyraAttributeSet::GetGyraAbilitySystemComponent() const
{
	return Cast<UGyraAbilitySystemComponent>(GetOwningAbilitySystemComponent());
}

