// Copyright 2024 Huagang Liu. All Rights Reserved.

#include "GyraGameData.h"
#include "GyraAssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GyraGameData)

UGyraGameData::UGyraGameData()
{
}

const UGyraGameData& UGyraGameData::Get()
{
	return UGyraAssetManager::Get().GetGameData();
}
