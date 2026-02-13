// Copyright 2024 Huagang Liu. All Rights Reserved.

#include "IndicatorLibrary.h"

#include "GyraIndicatorManagerComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(IndicatorLibrary)

class AController;

UIndicatorLibrary::UIndicatorLibrary()
{
}

UGyraIndicatorManagerComponent* UIndicatorLibrary::GetIndicatorManagerComponent(AController* Controller)
{
	return UGyraIndicatorManagerComponent::GetComponent(Controller);
}

