// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "GyraTeamInfoBase.h"

#include "GyraTeamPrivateInfo.generated.h"

class UObject;

UCLASS()
class AGyraTeamPrivateInfo : public AGyraTeamInfoBase
{
	GENERATED_BODY()

public:
	AGyraTeamPrivateInfo(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
