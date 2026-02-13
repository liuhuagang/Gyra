// Copyright 2025 Huagang Liu. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GyraStatsSet.h"
#include "Engine/DataTable.h"

#include "GyraPawnIndexInfo.generated.h"

class UGyraPawnData;

USTRUCT(BlueprintType)
struct FGyraPawnIndexInfo :public FTableRowBase
{
	GENERATED_BODY()

public:

	//角色主键
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gyra|Pawn")
	FGuid PawnID = FGuid();

	//角色数据定义类
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gyra|Pawn")
	TObjectPtr<const UGyraPawnData> PawnData = nullptr;

	bool operator==(const FGyraPawnIndexInfo& Other) const
	{
	
		return PawnID == Other.PawnID ? true : false;
	}

};
