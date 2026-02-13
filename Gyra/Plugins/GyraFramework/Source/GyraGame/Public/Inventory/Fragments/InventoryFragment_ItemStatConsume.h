// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "GameplayTagContainer.h"
#include "Inventory/GyraInventoryItemDefinition.h"

#include "InventoryFragment_ItemStatConsume.generated.h"

class UGyraInventoryItemInstance;
class UObject;
struct FGameplayTag;

UCLASS()
class UInventoryFragment_ItemStatConsume : public UGyraInventoryItemFragment
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category=Equipment)
	TSubclassOf<UGyraInventoryItemDefinition> TargetInventoryItem;

};
