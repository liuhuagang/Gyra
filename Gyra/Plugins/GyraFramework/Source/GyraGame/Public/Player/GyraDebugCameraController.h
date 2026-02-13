// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "Engine/DebugCameraController.h"

#include "GyraDebugCameraController.generated.h"

class UObject;


/**
 * AGyraDebugCameraController
 *
 *	Used for controlling the debug camera when it is enabled via the cheat manager.
 */
UCLASS()
class AGyraDebugCameraController : public ADebugCameraController
{
	GENERATED_BODY()

public:

	AGyraDebugCameraController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

	virtual void AddCheats(bool bForce) override;
};
