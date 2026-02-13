// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "GameFramework/GameSession.h"

#include "GyraGameSession.generated.h"

class UObject;


UCLASS(Config = Game)
class AGyraGameSession : public AGameSession
{
	GENERATED_BODY()

public:

	AGyraGameSession(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

	/** Override to disable the default behavior */
	/** 重写此方法以禁用默认行为 */
	virtual bool ProcessAutoLogin() override;

	virtual void HandleMatchHasStarted() override;
	virtual void HandleMatchHasEnded() override;
};
