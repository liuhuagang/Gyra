// Copyright 2024 Huagang Liu. All Rights Reserved.

#include "GyraWeaponDebugSettings.h"
#include "Misc/App.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GyraWeaponDebugSettings)

UGyraWeaponDebugSettings::UGyraWeaponDebugSettings()
{
}

FName UGyraWeaponDebugSettings::GetCategoryName() const
{
	return FApp::GetProjectName();
}

