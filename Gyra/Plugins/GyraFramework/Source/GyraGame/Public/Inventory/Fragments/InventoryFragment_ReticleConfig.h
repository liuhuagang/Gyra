// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "GyraInventoryItemDefinition.h"

#include "InventoryFragment_ReticleConfig.generated.h"

class UGyraReticleWidgetBase;
class UObject;

UCLASS()
class UInventoryFragment_ReticleConfig : public UGyraInventoryItemFragment
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Reticle)
	TArray<TSubclassOf<UGyraReticleWidgetBase>> ReticleWidgets;
};
