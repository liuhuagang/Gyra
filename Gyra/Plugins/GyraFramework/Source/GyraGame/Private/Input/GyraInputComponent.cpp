// Copyright 2024 Huagang Liu. All Rights Reserved.

#include "GyraInputComponent.h"

#include "EnhancedInputSubsystems.h"
#include "Player/GyraLocalPlayer.h"
#include "GyraSettingsLocal.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GyraInputComponent)

class UGyraInputConfig;

UGyraInputComponent::UGyraInputComponent(const FObjectInitializer& ObjectInitializer)
{
}

void UGyraInputComponent::AddInputMappings(const UGyraInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const
{
	check(InputConfig);
	check(InputSubsystem);

	// Here you can handle any custom logic to add something from your input config if required
}

void UGyraInputComponent::RemoveInputMappings(const UGyraInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const
{
	check(InputConfig);
	check(InputSubsystem);

	// Here you can handle any custom logic to remove input mappings that you may have added above
}

void UGyraInputComponent::RemoveBinds(TArray<uint32>& BindHandles)
{
	for (uint32 Handle : BindHandles)
	{
		RemoveBindingByHandle(Handle);
	}
	BindHandles.Reset();
}
