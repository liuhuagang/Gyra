// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "Templates/SubclassOf.h"


#include "GyraInventoryItemDefinition.h"
#include "GyraInventoryItemInfo.h"

#include "InventoryFragment_EquippableItem.generated.h"

class UGyraEquipmentDefinition;
class UObject;

UCLASS()
class UInventoryFragment_EquippableItem : public UGyraInventoryItemFragment
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Inventory)
	EGyraEquipmentItemType EquipmentType = EGyraEquipmentItemType::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Inventory)
	TSubclassOf<UGyraEquipmentDefinition> EquipmentDefinition;


};
