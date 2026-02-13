// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "Camera/PlayerCameraManager.h"

#include "GyraPlayerCameraManager.generated.h"

class FDebugDisplayInfo;
class UCanvas;
class UObject;

#define GYRA_CAMERA_DEFAULT_FOV			(80.0f)
#define GYRA_CAMERA_DEFAULT_PITCH_MIN	(-89.0f)
#define GYRA_CAMERA_DEFAULT_PITCH_MAX	(89.0f)

class UGyraUICameraManagerComponent;

/**
 * AGyraPlayerCameraManager
 *
 * The base player camera manager class used by this project.
 * 这个项目使用的基本播放器相机管理器类。
 */
UCLASS(notplaceable, MinimalAPI)
class AGyraPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()

public:

	AGyraPlayerCameraManager(const FObjectInitializer& ObjectInitializer);

	UGyraUICameraManagerComponent* GetUICameraComponent() const;

protected:

	virtual void UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime) override;

	virtual void DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DebugDisplay, float& YL, float& YPos) override;

private:
	/** The UI Camera Component, controls the camera when UI is doing something important that gameplay doesn't get priority over. */
	/** UI Camera组件，当UI在做一些游戏玩法无法优先处理的重要事情时，控制摄像头。 */
	UPROPERTY(Transient)
	TObjectPtr<UGyraUICameraManagerComponent> UICamera;
};


