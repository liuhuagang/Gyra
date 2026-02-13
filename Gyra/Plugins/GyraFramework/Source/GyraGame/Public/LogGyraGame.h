// Copyright 2025 Huagang Liu. All Rights Reserved.

#pragma once

#include "GyraGameMacro.h"

#include "Logging/LogMacros.h"



class UObject;

GYRAGAMEFRAMEWORK_API DECLARE_LOG_CATEGORY_EXTERN(LogGyraGame, Log, All);
GYRAGAMEFRAMEWORK_API DECLARE_LOG_CATEGORY_EXTERN(LogGyraExperience, Log, All);
GYRAGAMEFRAMEWORK_API DECLARE_LOG_CATEGORY_EXTERN(LogGyraAbilitySystem, Log, All);
GYRAGAMEFRAMEWORK_API DECLARE_LOG_CATEGORY_EXTERN(LogGyraTeams, Log, All);


GYRAGAMEFRAMEWORK_API FString GetClientServerContextString(UObject* ContextObject = nullptr);

