// Copyright 2024 Huagang Liu. All Rights Reserved.

#include "GyraEquipmentDefinition.h"
#include "GyraEquipmentInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GyraEquipmentDefinition)

UGyraEquipmentDefinition::UGyraEquipmentDefinition(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstanceType = UGyraEquipmentInstance::StaticClass();
}

