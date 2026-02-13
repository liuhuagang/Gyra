// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "Engine/World.h"
#include "GameplayTagContainer.h"

#include "GyraCameraMode.generated.h"


class AActor;
class UCanvas;
class UGyraCameraComponent;


/**
 * EGyraCameraModeBlendFunction
 *
 * Blend function used for transitioning between camera modes.
 * 混合功能，用于相机模式之间的转换。
 */
UENUM(BlueprintType)
enum class EGyraCameraModeBlendFunction : uint8
{
	// Does a simple linear interpolation.
	// 做一个简单的线性插值。
	Linear,

	// Immediately accelerates, but smoothly decelerates into the target.  Ease amount controlled by the exponent.
	// 立即加速，但平稳减速进入目标。由指数控制的轻松量。
	EaseIn,

	// Smoothly accelerates, but does not decelerate into the target.  Ease amount controlled by the exponent.
	// 平稳加速，但不减速进入目标。由指数控制的轻松量。
	EaseOut,

	// Smoothly accelerates and decelerates.  Ease amount controlled by the exponent.
	// 平稳加速和减速。由指数控制的轻松量。
	EaseInOut,

	COUNT	UMETA(Hidden)
};


/**
 * FGyraCameraModeView
 *
 * View data produced by the camera mode that is used to blend camera modes.
 * 查看用于混合摄像机模式的摄像机模式产生的数据。
 */
struct FGyraCameraModeView
{
public:

	FGyraCameraModeView();

	void Blend(const FGyraCameraModeView& Other, float OtherWeight);

public:

	FVector Location;
	FRotator Rotation;
	FRotator ControlRotation;
	float FieldOfView;
};


/**
 * UGyraCameraMode
 *
 * Base class for all camera modes.
 * 所有相机模式的基类。
 */
UCLASS(Abstract, NotBlueprintable)
class GYRAGAME_API UGyraCameraMode : public UObject
{
	GENERATED_BODY()

public:

	UGyraCameraMode();

	UGyraCameraComponent* GetGyraCameraComponent() const;

	virtual UWorld* GetWorld() const override;

	AActor* GetTargetActor() const;

	const FGyraCameraModeView& GetCameraModeView() const { return View; }

	// Called when this camera mode is activated on the camera mode stack.
	// 当相机模式在相机模式堆栈上被激活时调用。
	virtual void OnActivation() {};

	// Called when this camera mode is deactivated on the camera mode stack.
	// 当此相机模式在相机模式堆栈上被禁用时调用。
	virtual void OnDeactivation() {};


	void UpdateCameraMode(float DeltaTime);

	float GetBlendTime() const { return BlendTime; }
	float GetBlendWeight() const { return BlendWeight; }
	void SetBlendWeight(float Weight);

	FGameplayTag GetCameraTypeTag() const
	{
		return CameraTypeTag;
	}

	virtual void DrawDebug(UCanvas* Canvas) const;

protected:

	virtual FVector GetPivotLocation() const;
	virtual FRotator GetPivotRotation() const;

	virtual void UpdateView(float DeltaTime);
	virtual void UpdateBlending(float DeltaTime);

protected:

	// A tag that can be queried by gameplay code that cares when a kind of camera mode is active
	// without having to ask about a specific mode (e.g., when aiming downsights to get more accuracy)
	/**
	 * 一个可以被游戏代码查询的标签，它关心一种相机模式何时处于活动状态，而不必询问特定模式（例如，当瞄准瞄准器以获得更高的精度时）
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Blending")
	FGameplayTag CameraTypeTag;


	// View output produced by the camera mode.
	// 查看由相机模式产生的输出。
	FGyraCameraModeView View;

	// The horizontal field of view (in degrees).
	// //水平视野（以度为单位）。
	UPROPERTY(EditDefaultsOnly, Category = "View", 
	Meta = (UIMin = "5.0", UIMax = "170", ClampMin = "5.0", ClampMax = "170.0"))
	float FieldOfView;

	// Minimum view pitch (in degrees).
	// 最小视距（以度为单位）。
	UPROPERTY(EditDefaultsOnly, Category = "View", 
	Meta = (UIMin = "-89.9", UIMax = "89.9", ClampMin = "-89.9", ClampMax = "89.9"))
	float ViewPitchMin;

	// Maximum view pitch (in degrees).
	// 最大视距（以度为单位）。
	UPROPERTY(EditDefaultsOnly, Category = "View", 
	Meta = (UIMin = "-89.9", UIMax = "89.9", ClampMin = "-89.9", ClampMax = "89.9"))
	float ViewPitchMax;

	// How long it takes to blend in this mode.
	// 在此模式下混合需要多长时间。
	UPROPERTY(EditDefaultsOnly, Category = "Blending")
	float BlendTime;

	// Function used for blending.
	// 用于混合的函数。
	UPROPERTY(EditDefaultsOnly, Category = "Blending")
	EGyraCameraModeBlendFunction BlendFunction;

	// Exponent used by blend functions to control the shape of the curve.
	// 混合函数用来控制曲线形状的指数。
	UPROPERTY(EditDefaultsOnly, Category = "Blending")
	float BlendExponent;

	// Linear blend alpha used to determine the blend weight.
	// 线性混合alpha用于确定混合权重。
	float BlendAlpha;

	// Blend weight calculated using the blend alpha and function.
	// //使用混合alpha和函数计算混合权重。
	float BlendWeight;

protected:
	/** If true, skips all interpolation and puts camera in ideal location.  Automatically set to false next frame. */
	/** 如果为true，跳过所有插值并将相机放在理想位置。下一帧自动设置为false。*/
	UPROPERTY(transient)
	uint32 bResetInterpolation : 1;
};


/**
 * UGyraCameraModeStack
 *
 * Stack used for blending camera modes.
 * 用于混合相机模式的堆栈。
 */
UCLASS()
class UGyraCameraModeStack : public UObject
{
	GENERATED_BODY()

public:

	UGyraCameraModeStack();

	void ActivateStack();
	void DeactivateStack();

	bool IsStackActivate() const { return bIsActive; }

	void PushCameraMode(TSubclassOf<UGyraCameraMode> CameraModeClass);

	bool EvaluateStack(float DeltaTime, FGyraCameraModeView& OutCameraModeView);

	void DrawDebug(UCanvas* Canvas) const;

	// Gets the tag associated with the top layer and the blend weight of it
	void GetBlendInfo(float& OutWeightOfTopLayer, FGameplayTag& OutTagOfTopLayer) const;

protected:

	UGyraCameraMode* GetCameraModeInstance(TSubclassOf<UGyraCameraMode> CameraModeClass);

	void UpdateStack(float DeltaTime);
	void BlendStack(FGyraCameraModeView& OutCameraModeView) const;

protected:

	bool bIsActive;

	UPROPERTY()
	TArray<TObjectPtr<UGyraCameraMode>> CameraModeInstances;

	UPROPERTY()
	TArray<TObjectPtr<UGyraCameraMode>> CameraModeStack;

};
