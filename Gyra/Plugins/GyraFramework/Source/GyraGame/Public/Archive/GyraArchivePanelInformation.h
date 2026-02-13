// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"

#include "GyraArchiveType.h"

#include "GyraArchivePanelInformation.generated.h"


USTRUCT(BlueprintType)
struct FGyraArchivePanelItemBriefInformation
{
	GENERATED_BODY()

public:

	//最后一次激活的时间
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FString DateTime;


};

UCLASS()
class GYRAGAME_API UGyraArchivePanelInformation :public USaveGame
{
	GENERATED_BODY()

public:

	UGyraArchivePanelInformation();

	//存档数量
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	int32 ArchiveSlots;

	//上次使用的存档索引
	int32 LastActiveSlotIndex =-1;

	//仓库物品栏
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FGyraArchiveListInfomation WarehouseList;

	//已使用的存档位置
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TMap<int32, FGyraArchivePanelItemBriefInformation> ActiveArchiveSlots;


};
