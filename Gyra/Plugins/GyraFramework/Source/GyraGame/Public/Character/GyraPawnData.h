// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "Engine/DataAsset.h"


#include "GyraPawnData.generated.h"

class APawn;
class UGyraAbilitySet;
class UGyraAbilityTagRelationshipMapping;
class UGyraCameraMode;
class UGyraInputConfig;
class UObject;


/**
 * UGyraPawnData
 *
 * Non-mutable data asset that contains properties used to define a pawn.
 * 
 * 包含用于定义棋子的属性的不可变数据资产。
 * 
 */
UCLASS(BlueprintType, Const, Meta = (DisplayName = "Gyra Pawn Data", ShortTooltip = "Data asset used to define a Pawn."))
class GYRAGAME_API UGyraPawnData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UGyraPawnData(const FObjectInitializer& ObjectInitializer);

public:

	//用来查表的主键,主要用于怪物查表
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gyra|Pawn")
	FName PawnInitName = NAME_None;

	// Class to instantiate for this pawn (should usually derive from AGyraPawn or AGyraCharacter).
	// 类来实例化这个小卒（通常应该从AGyraPawn或AGyracharter派生）。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gyra|Pawn")
	TSubclassOf<APawn> PawnClass;

	// Ability sets to grant to this pawn's ability system.
	// 赋予该兵的能力系统的能力集。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gyra|Abilities")
	TArray<TObjectPtr<UGyraAbilitySet>> AbilitySets;

	// What mapping of ability tags to use for actions taking by this pawn
	// 这个棋子所采取的行动要使用什么样的能力标签映射
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gyra|Abilities")
	TObjectPtr<UGyraAbilityTagRelationshipMapping> TagRelationshipMapping;

	// Input configuration used by player controlled pawns to create input mappings and bind input actions.
	// 玩家控制棋子用来创建输入映射和绑定输入动作的输入配置。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gyra|Input")
	TObjectPtr<UGyraInputConfig> InputConfig;

	// Default camera mode used by player controlled pawns.
	// 玩家控制棋子时使用的默认摄像模式。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gyra|Camera")
	TSubclassOf<UGyraCameraMode> DefaultCameraMode;

};
