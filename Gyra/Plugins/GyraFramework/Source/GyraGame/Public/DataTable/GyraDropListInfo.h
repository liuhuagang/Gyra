// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GyraStatsSet.h"
#include "Engine/DataTable.h"

#include "GyraDropListInfo.generated.h"

USTRUCT(BlueprintType)
struct FGyraDropChanceInfo 
{
	GENERATED_BODY()

public:
	
	//掉落几率0-100
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gyra|Inventory")
	int32 DropChance = 0;

	//物品对应的唯一名称
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gyra|Inventory")
	FName InventoryItem = NAME_None;

	//物品掉落数量
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gyra|Inventory")
	int32 Num = 1;


};


USTRUCT(BlueprintType)
struct FGyraDropListInfo :public FTableRowBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gyra|Inventory")
	TArray<FGyraDropChanceInfo> RandomDropList;

};
