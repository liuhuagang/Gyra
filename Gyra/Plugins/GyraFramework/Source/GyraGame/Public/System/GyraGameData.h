// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "GyraGameData.generated.h"

class UGameplayEffect;
class UObject;
struct FGameplayTag;

/**
 * UGyraGameData
 *
 * Non-mutable data asset that contains global game data.
 * 包含全局游戏数据的不可变数据资产。
 * 
 */
UCLASS(BlueprintType, Const, Meta = (DisplayName = "Gyra Game Data", ShortTooltip = "Data asset containing global game data."))
class UGyraGameData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UGyraGameData();

	// Returns the loaded game data.
	// 返回已加载的游戏数据。
	static const UGyraGameData& Get();

public:

	// Gameplay effect used to apply damage.  Uses SetByCaller for the damage magnitude.
	// 用于施加伤害的游戏效果。使用SetByCaller来表示损坏程度。
	UPROPERTY(EditDefaultsOnly, Category = "Default Gameplay Effects", meta = (DisplayName = "Damage Gameplay Effect (SetByCaller)"))
	TSoftClassPtr<UGameplayEffect> DamageGameplayEffect_SetByCaller;

	// Gameplay effect used to apply healing.  Uses SetByCaller for the healing magnitude.
	// 用于治疗的游戏效果。使用SetByCaller作为治疗大小。
	UPROPERTY(EditDefaultsOnly, Category = "Default Gameplay Effects", meta = (DisplayName = "Heal Gameplay Effect (SetByCaller)"))
	TSoftClassPtr<UGameplayEffect> HealGameplayEffect_SetByCaller;

	// Gameplay effect used to add and remove dynamic tags.
	// 游戏效果用于添加和删除动态标签。
	UPROPERTY(EditDefaultsOnly, Category = "Default Gameplay Effects")
	TSoftClassPtr<UGameplayEffect> DynamicTagGameplayEffect;
		
	// 用于施加装备的增益游戏效果。使用SetByCaller来表示增强程度。
	UPROPERTY(EditDefaultsOnly, Category = "Default Gameplay Effects", meta = (DisplayName = "Equipment Gameplay Effect (SetByCaller)"))
	TMap<FGameplayTag,TSoftClassPtr<UGameplayEffect>> EquipmentGameplayEffect_SetByCaller;

};

