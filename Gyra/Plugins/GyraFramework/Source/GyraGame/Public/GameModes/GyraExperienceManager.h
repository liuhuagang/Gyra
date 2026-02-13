// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "Subsystems/EngineSubsystem.h"
#include "GyraExperienceManager.generated.h"


/**
 * Manager for experiences - primarily for arbitration between multiple PIE sessions
 * 体验管理器——主要用于多个PIE会话之间的仲裁
 */
UCLASS(MinimalAPI)
class UGyraExperienceManager : public UEngineSubsystem
{
	GENERATED_BODY()

public:

#if WITH_EDITOR
	GYRAGAME_API void OnPlayInEditorBegun();

	static void NotifyOfPluginActivation(const FString PluginURL);
	static bool RequestToDeactivatePlugin(const FString PluginURL);
#else
	static void NotifyOfPluginActivation(const FString PluginURL) {}
	static bool RequestToDeactivatePlugin(const FString PluginURL) { return true; }
#endif

private:
	// The map of requests to active count for a given game feature plugin
	// (to allow first in, last out activation management during PIE)
	// 给定游戏功能插件的请求到活动计数的映射（在PIE期间允许先入后出的激活管理）
	TMap<FString, int32> GameFeaturePluginRequestCountMap;
};

