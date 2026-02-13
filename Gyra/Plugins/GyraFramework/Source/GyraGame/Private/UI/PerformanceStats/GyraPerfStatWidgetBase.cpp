// Copyright 2024 Huagang Liu. All Rights Reserved.

#include "GyraPerfStatWidgetBase.h"

#include "Engine/GameInstance.h"
#include "GyraPerformanceStatSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GyraPerfStatWidgetBase)

//////////////////////////////////////////////////////////////////////
// UGyraPerfStatWidgetBase

UGyraPerfStatWidgetBase::UGyraPerfStatWidgetBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

double UGyraPerfStatWidgetBase::FetchStatValue()
{
	if (CachedStatSubsystem == nullptr)
	{
		if (UWorld* World = GetWorld())
		{
			if (UGameInstance* GameInstance = World->GetGameInstance())
			{
				CachedStatSubsystem = GameInstance->GetSubsystem<UGyraPerformanceStatSubsystem>();
			}
		}
	}

	if (CachedStatSubsystem)
	{
		return CachedStatSubsystem->GetCachedStat(StatToDisplay);
	}
	else
	{
		return 0.0;
	}
}

