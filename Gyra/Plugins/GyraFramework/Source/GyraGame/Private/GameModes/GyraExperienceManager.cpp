// Copyright 2024 Huagang Liu. All Rights Reserved.

#include "GyraExperienceManager.h"

#include "Engine/Engine.h"
#include "Subsystems/SubsystemCollection.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(GyraExperienceManager)


#if WITH_EDITOR

void UGyraExperienceManager::OnPlayInEditorBegun()
{
	ensure(GameFeaturePluginRequestCountMap.IsEmpty());
	GameFeaturePluginRequestCountMap.Empty();
}

void UGyraExperienceManager::NotifyOfPluginActivation(const FString PluginURL)
{
	if (GIsEditor)
	{
		UGyraExperienceManager* ExperienceManagerSubsystem = GEngine->GetEngineSubsystem<UGyraExperienceManager>();
		check(ExperienceManagerSubsystem);

		// Track the number of requesters who activate this plugin. Multiple load/activation requests are always allowed because concurrent requests are handled.
		// 跟踪激活此插件的请求者的数量。总是允许多个加载/激活请求，因为要处理并发请求。
		int32& Count = ExperienceManagerSubsystem->GameFeaturePluginRequestCountMap.FindOrAdd(PluginURL);
		++Count;
	}
}

bool UGyraExperienceManager::RequestToDeactivatePlugin(const FString PluginURL)
{
	if (GIsEditor)
	{
		UGyraExperienceManager* ExperienceManagerSubsystem = GEngine->GetEngineSubsystem<UGyraExperienceManager>();
		check(ExperienceManagerSubsystem);

		// Only let the last requester to get this far deactivate the plugin
		// 只让最后一个请求到此为止的人停用插件
		int32& Count = ExperienceManagerSubsystem->GameFeaturePluginRequestCountMap.FindChecked(PluginURL);
		--Count;

		if (Count == 0)
		{
			ExperienceManagerSubsystem->GameFeaturePluginRequestCountMap.Remove(PluginURL);
			return true;
		}

		return false;
	}

	return true;
}

#endif
