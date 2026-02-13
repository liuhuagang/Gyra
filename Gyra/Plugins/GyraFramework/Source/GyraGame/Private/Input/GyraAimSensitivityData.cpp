// Copyright 2024 Huagang Liu. All Rights Reserved.

#include "GyraAimSensitivityData.h"

#include "GyraSettingsShared.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GyraAimSensitivityData)

UGyraAimSensitivityData::UGyraAimSensitivityData(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SensitivityMap =
	{
		{ EGyraGamepadSensitivity::Slow,			0.5f },
		{ EGyraGamepadSensitivity::SlowPlus,		0.75f },
		{ EGyraGamepadSensitivity::SlowPlusPlus,	0.9f },
		{ EGyraGamepadSensitivity::Normal,		1.0f },
		{ EGyraGamepadSensitivity::NormalPlus,	1.1f },
		{ EGyraGamepadSensitivity::NormalPlusPlus,1.25f },
		{ EGyraGamepadSensitivity::Fast,			1.5f },
		{ EGyraGamepadSensitivity::FastPlus,		1.75f },
		{ EGyraGamepadSensitivity::FastPlusPlus,	2.0f },
		{ EGyraGamepadSensitivity::Insane,		2.5f },
	};
}

const float UGyraAimSensitivityData::SensitivtyEnumToFloat(const EGyraGamepadSensitivity InSensitivity) const
{
	if (const float* Sens = SensitivityMap.Find(InSensitivity))
	{
		return *Sens;
	}

	return 1.0f;
}

