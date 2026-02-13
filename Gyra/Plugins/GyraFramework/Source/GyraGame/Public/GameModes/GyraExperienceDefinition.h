// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "Engine/DataAsset.h"

#include "GyraExperienceDefinition.generated.h"

class UGameFeatureAction;
class UGyraPawnData;
class UGyraMonsterData;
class UGyraExperienceActionSet;




/**
 *
 *
 * Definition of an experience
 *
 * 体验的定义
 *
 */
UCLASS(BlueprintType, Const)
class UGyraExperienceDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UGyraExperienceDefinition();


	//~UObject interface
#if WITH_EDITOR
	//在变更列表验证期间验证对象的通用函数。
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const     override;
#endif
	//~End of UObject interface

	//~UPrimaryDataAsset interface
#if WITH_EDITORONLY_DATA
	//这将扫描类中资产属性上的AssetBundles元数据，并使用InitializeAssetBundlesFromMetadata初始化AssetBundleData
	virtual void UpdateAssetBundleData() override;
#endif
	//~End of UPrimaryDataAsset interface

public:
	// List of Game Feature Plugins this experience wants to have active
	// 游戏功能插件列表
	UPROPERTY(EditDefaultsOnly, Category = Gameplay)
	TArray<FString> GameFeaturesToEnable;

	/** The default pawn class to spawn for players */
	//@TODO: Make soft?
	//后面可以转换为软引用
	//玩家默认生成的棋子类的数据引用
	UPROPERTY(EditDefaultsOnly, Category = Gameplay)
	TObjectPtr<const UGyraPawnData> DefaultPawnData;

	// List of actions to perform as this experience is loaded/activated/deactivated/unloaded
	// 加载/激活/停用/卸载此体验时要执行的操作列表
	UPROPERTY(EditDefaultsOnly, Instanced, Category = "Actions")
	TArray<TObjectPtr<UGameFeatureAction>> Actions;


	// List of additional action sets to compose into this experience
	// 要组合到此体验中的其他操作集的列表
	UPROPERTY(EditDefaultsOnly, Category = Gameplay)
	TArray<TObjectPtr<UGyraExperienceActionSet>> ActionSets;


};