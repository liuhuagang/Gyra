// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "GameplayTagContainer.h"
#include "Inventory/GyraInventoryItemDefinition.h"

#include "InventoryFragment_SetStats.generated.h"

class UGyraInventoryItemInstance;
class UObject;
struct FGameplayTag;



/**
 * 初始化物品实例的碎片
 * 这个包含了物品的初始化Tag:
 * 例如最大堆叠数量
 */
UCLASS()
class UInventoryFragment_SetStats : public UGyraInventoryItemFragment
{
	GENERATED_BODY()

public:
	
	UInventoryFragment_SetStats();


protected:


	UPROPERTY(EditDefaultsOnly, Category = "Equipment|InitTag")
	TMap<FGameplayTag, int32> InitialItemStats;




public:

	virtual void OnInstanceCreated(UGyraInventoryItemInstance* Instance) const override;

	int32 GetItemStatByTag(FGameplayTag Tag) const;

};
