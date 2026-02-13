// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "GameFramework/WorldSettings.h"
#include "GyraWorldSettings.generated.h"

class UGyraExperienceDefinition;


/**
 * 
 * The default world settings object, used primarily to set the default gameplay experience to use when playing on this map
 * 默认世界设置对象，主要用于设置在此地图上玩游戏时使用的默认游戏体验
 * 
 */
UCLASS()
class GYRAGAME_API AGyraWorldSettings : public AWorldSettings
{
	GENERATED_BODY()

public:

	AGyraWorldSettings(const FObjectInitializer& ObjectInitializer);

#if WITH_EDITOR
	virtual void CheckForErrors() override;
#endif

public:
	// Returns the default experience to use when a server opens this map if it is not overridden by the user-facing experience
	// 返回服务器打开此地图时使用的默认体验，如果它没有被面向用户的体验覆盖
	FPrimaryAssetId GetDefaultGameplayExperience() const;

protected:
	// The default experience to use when a server opens this map if it is not overridden by the user-facing experience
	// 当服务器打开此地图时，如果它没有被面向用户的体验覆盖，则使用默认体验
	UPROPERTY(EditDefaultsOnly, Category = GameMode)
	TSoftClassPtr<UGyraExperienceDefinition> DefaultGameplayExperience;

public:

#if WITH_EDITORONLY_DATA
	// Is this level part of a front-end or other standalone experience?
	// When set, the net mode will be forced to Standalone when you hit Play in the editor
	// 这个关卡是前端的一部分还是其他独立的体验？
	// 当设置时，当你在编辑器中点击Play时，net模式将被强制为Standalone
	UPROPERTY(EditDefaultsOnly, Category = PIE)
	bool ForceStandaloneNetMode = false;
#endif
};
