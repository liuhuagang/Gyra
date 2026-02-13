// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "GameFramework/SaveGame.h"

#include "GyraArchiveType.h"
#include "GyraMutableInfo.h"

#include "GyraArchiveSlotInformation.generated.h"



UCLASS()
class GYRAGAME_API UGyraArchiveSlotInformation :public USaveGame
{
	GENERATED_BODY()

public:

	//玩家装扮信息
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FGyraMutableInfo MutableInfo;

	//快捷装备栏
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FGyraArchiveListInfomation QuickEquipmentList;

	//被动装备栏
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FGyraArchiveListInfomation PassiveEquipmentList;

	//背包物品栏
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FGyraArchiveListInfomation BackpackList;


};
