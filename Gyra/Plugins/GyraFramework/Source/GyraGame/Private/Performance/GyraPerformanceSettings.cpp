// Copyright 2024 Huagang Liu. All Rights Reserved.

#include "GyraPerformanceSettings.h"

#include "Engine/PlatformSettingsManager.h"
#include "Misc/EnumRange.h"
#include "Performance/GyraPerformanceStatTypes.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GyraPerformanceSettings)

//////////////////////////////////////////////////////////////////////

UGyraPlatformSpecificRenderingSettings::UGyraPlatformSpecificRenderingSettings()
{
	MobileFrameRateLimits.Append({ 20, 30, 45, 60, 90, 120 });
}

const UGyraPlatformSpecificRenderingSettings* UGyraPlatformSpecificRenderingSettings::Get()
{
	UGyraPlatformSpecificRenderingSettings* Result = UPlatformSettingsManager::Get().GetSettingsForPlatform<ThisClass>();
	check(Result);
	return Result;
}

//////////////////////////////////////////////////////////////////////

UGyraPerformanceSettings::UGyraPerformanceSettings()
{
	PerPlatformSettings.Initialize(UGyraPlatformSpecificRenderingSettings::StaticClass());

	CategoryName = TEXT("Game");

	DesktopFrameRateLimits.Append({ 30, 60, 120, 144, 160, 165, 180, 200, 240, 360 });

	// Default to all stats are allowed
	FGyraPerformanceStatGroup& StatGroup = UserFacingPerformanceStats.AddDefaulted_GetRef();
	for (EGyraDisplayablePerformanceStat PerfStat : TEnumRange<EGyraDisplayablePerformanceStat>())
	{
		StatGroup.AllowedStats.Add(PerfStat);
	}
}

