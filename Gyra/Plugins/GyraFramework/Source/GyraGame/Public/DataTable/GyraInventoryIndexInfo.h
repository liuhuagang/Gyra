// Copyright 2025 Huagang Liu. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GyraStatsSet.h"
#include "Engine/DataTable.h"

#include "GyraInventoryIndexInfo.generated.h"

class UGyraInventoryItemDefinition;

USTRUCT(BlueprintType)
struct GYRAGAME_API FGyraInventoryItemIndexInfo :public FTableRowBase
{
	GENERATED_BODY()

public:

	//物品主键
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gyra|Inventory")
	FGuid InventoryItemID = FGuid();

	//物品定义类
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gyra|Inventory")
	TSubclassOf<UGyraInventoryItemDefinition> InventoryItemDefinition = nullptr;

	bool operator==(const FGyraInventoryItemIndexInfo& Other) const
	{

		return InventoryItemID == Other.InventoryItemID ? true : false;
	}

};

