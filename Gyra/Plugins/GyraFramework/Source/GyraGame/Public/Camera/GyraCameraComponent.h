// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "Camera/CameraComponent.h"
#include "GameFramework/Actor.h"

#include "GyraCameraComponent.generated.h"


class UCanvas;
class UGyraCameraMode;
class UGyraCameraModeStack;
class UObject;
struct FFrame;
struct FGameplayTag;
struct FMinimalViewInfo;
template <class TClass> class TSubclassOf;

DECLARE_DELEGATE_RetVal(TSubclassOf<UGyraCameraMode>, FGyraCameraModeDelegate);

/**
 * UGyraCameraComponent
 *
 * The base camera component class used by this project.
 * 这个项目使用的基本相机组件类。
 */
UCLASS()
class UGyraCameraComponent : public UCameraComponent
{
	GENERATED_BODY()

public:

	UGyraCameraComponent(const FObjectInitializer& ObjectInitializer);

	// Returns the camera component if one exists on the specified actor.
	// 如果在指定的演员上存在，则返回相机组件。
	UFUNCTION(BlueprintPure, Category = "Gyra|Camera")
	static UGyraCameraComponent* FindCameraComponent(const AActor* Actor) 
	{ return (Actor ? Actor->FindComponentByClass<UGyraCameraComponent>() : nullptr); }

	// Returns the target actor that the camera is looking at.
	// 返回摄像机正在观看的目标演员。
	virtual AActor* GetTargetActor() const { return GetOwner(); }

	// Delegate used to query for the best camera mode.
	// 委托用于查询最佳相机模式。
	FGyraCameraModeDelegate DetermineCameraModeDelegate;

	// Add an offset to the field of view.  The offset is only for one frame, it gets cleared once it is applied.
	// 向视野添加偏移量。偏移量只适用于一帧，一旦应用就会被清除。
	void AddFieldOfViewOffset(float FovOffset) { FieldOfViewOffset += FovOffset; }

	virtual void DrawDebug(UCanvas* Canvas) const;

	// Gets the tag associated with the top layer and the blend weight of it
	// 获取与顶层关联的标记及其混合权重
	void GetBlendInfo(float& OutWeightOfTopLayer, FGameplayTag& OutTagOfTopLayer) const;

protected:

	virtual void OnRegister() override;
	virtual void GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView) override;

	virtual void UpdateCameraModes();

protected:

	// Stack used to blend the camera modes.
	// Stack用于混合相机模式。
	UPROPERTY()
	TObjectPtr<UGyraCameraModeStack> CameraModeStack;

	// Offset applied to the field of view.  The offset is only for one frame, it gets cleared once it is applied.
	// 应用于视场的偏移量。偏移量只适用于一帧，一旦应用就会被清除。
	float FieldOfViewOffset;

};

