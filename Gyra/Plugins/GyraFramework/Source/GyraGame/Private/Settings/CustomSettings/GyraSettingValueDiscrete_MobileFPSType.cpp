// Copyright 2024 Huagang Liu. All Rights Reserved.

#include "GyraSettingValueDiscrete_MobileFPSType.h"

#include "Performance/GyraPerformanceSettings.h"
#include "GyraSettingsLocal.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GyraSettingValueDiscrete_MobileFPSType)

#define LOCTEXT_NAMESPACE "GyraSettings"

UGyraSettingValueDiscrete_MobileFPSType::UGyraSettingValueDiscrete_MobileFPSType()
{
}

void UGyraSettingValueDiscrete_MobileFPSType::OnInitialized()
{
	Super::OnInitialized();

	const UGyraPlatformSpecificRenderingSettings* PlatformSettings = UGyraPlatformSpecificRenderingSettings::Get();
	const UGyraSettingsLocal* UserSettings = UGyraSettingsLocal::Get();

	for (int32 TestLimit : PlatformSettings->MobileFrameRateLimits)
	{
		if (UGyraSettingsLocal::IsSupportedMobileFramePace(TestLimit))
		{
			FPSOptions.Add(TestLimit, MakeLimitString(TestLimit));
		}
	}

	const int32 FirstFrameRateWithQualityLimit = UserSettings->GetFirstFrameRateWithQualityLimit();
	if (FirstFrameRateWithQualityLimit > 0)
	{
		SetWarningRichText(FText::Format(LOCTEXT("MobileFPSType_Note", "<strong>Note: Changing the framerate setting to {0} or higher might lower your Quality Presets.</>"), MakeLimitString(FirstFrameRateWithQualityLimit)));
	}
}

int32 UGyraSettingValueDiscrete_MobileFPSType::GetDefaultFPS() const
{
	return UGyraSettingsLocal::GetDefaultMobileFrameRate();
}

FText UGyraSettingValueDiscrete_MobileFPSType::MakeLimitString(int32 Number)
{
	return FText::Format(LOCTEXT("MobileFrameRateOption", "{0} FPS"), FText::AsNumber(Number));
}

void UGyraSettingValueDiscrete_MobileFPSType::StoreInitial()
{
	InitialValue = GetValue();
}

void UGyraSettingValueDiscrete_MobileFPSType::ResetToDefault()
{
	SetValue(GetDefaultFPS(), EGameSettingChangeReason::ResetToDefault);
}

void UGyraSettingValueDiscrete_MobileFPSType::RestoreToInitial()
{
	SetValue(InitialValue, EGameSettingChangeReason::RestoreToInitial);
}

void UGyraSettingValueDiscrete_MobileFPSType::SetDiscreteOptionByIndex(int32 Index)
{
	TArray<int32> FPSOptionsModes;
	FPSOptions.GenerateKeyArray(FPSOptionsModes);

	int32 NewMode = FPSOptionsModes.IsValidIndex(Index) ? FPSOptionsModes[Index] : GetDefaultFPS();

	SetValue(NewMode, EGameSettingChangeReason::Change);
}

int32 UGyraSettingValueDiscrete_MobileFPSType::GetDiscreteOptionIndex() const
{
	TArray<int32> FPSOptionsModes;
	FPSOptions.GenerateKeyArray(FPSOptionsModes);
	return FPSOptionsModes.IndexOfByKey(GetValue());
}

TArray<FText> UGyraSettingValueDiscrete_MobileFPSType::GetDiscreteOptions() const
{
	TArray<FText> Options;
	FPSOptions.GenerateValueArray(Options);

	return Options;
}

int32 UGyraSettingValueDiscrete_MobileFPSType::GetValue() const
{
	return UGyraSettingsLocal::Get()->GetDesiredMobileFrameRateLimit();
}

void UGyraSettingValueDiscrete_MobileFPSType::SetValue(int32 NewLimitFPS, EGameSettingChangeReason InReason)
{
	UGyraSettingsLocal::Get()->SetDesiredMobileFrameRateLimit(NewLimitFPS);

	NotifySettingChanged(InReason);
}

#undef LOCTEXT_NAMESPACE

