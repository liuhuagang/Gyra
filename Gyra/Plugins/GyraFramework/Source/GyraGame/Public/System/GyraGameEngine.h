// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "Engine/GameEngine.h"

#include "GyraGameEngine.generated.h"

class IEngineLoop;
class UObject;

/*
 * 插件所使用的游戏引擎基类
 */
UCLASS()
class UGyraGameEngine : public UGameEngine
{
	GENERATED_BODY()

public:

	UGyraGameEngine(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

	virtual void Init(IEngineLoop* InEngineLoop) override;
};

