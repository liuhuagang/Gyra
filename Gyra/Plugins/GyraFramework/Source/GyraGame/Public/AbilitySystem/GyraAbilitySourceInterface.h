// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "UObject/Interface.h"

#include "GyraAbilitySourceInterface.generated.h"

class UObject;
class UPhysicalMaterial;
struct FGameplayTagContainer;

/** Base interface for anything acting as a ability calculation source */
/** 任何作为能力计算源的基本接口 */
UINTERFACE()
class UGyraAbilitySourceInterface : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class IGyraAbilitySourceInterface
{
	GENERATED_IINTERFACE_BODY()

	/**
	 * Compute the multiplier for effect falloff with distance
	 * 计算效果随距离衰减的乘数
	 *
	 * @param Distance			Distance from source to target for ability calculations (distance bullet traveled for a gun, etc...)
	 *							从源到目标的距离（用于计算能力）（例如子弹穿过的距离等等）。
	 * @param SourceTags		Aggregated Tags from the source
	 *							来自源的聚合标记
	 * @param TargetTags		Aggregated Tags currently on the target
	 *							当前目标上的聚合标签
	 * @return Multiplier to apply to the base attribute value due to distance
	 * 由于距离，将乘数应用于基本属性值
	 */
	virtual float GetDistanceAttenuation(float Distance, 
	const FGameplayTagContainer* SourceTags = nullptr,
	const FGameplayTagContainer* TargetTags = nullptr) const = 0;

	virtual float GetPhysicalMaterialAttenuation(const UPhysicalMaterial* PhysicalMaterial, 
	const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr) const = 0;
};
