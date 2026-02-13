// Copyright 2024 Huagang Liu. All Rights Reserved.

#include "GyraGameSettingRegistry.h"

#include "GameSettingCollection.h"
#include "GyraSettingsLocal.h"
#include "GyraSettingsShared.h"
#include "GyraLocalPlayer.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GyraGameSettingRegistry)

DEFINE_LOG_CATEGORY(LogGyraGameSettingRegistry);

#define LOCTEXT_NAMESPACE "Gyra"

//--------------------------------------
// UGyraGameSettingRegistry
//--------------------------------------

UGyraGameSettingRegistry::UGyraGameSettingRegistry()
{
}

UGyraGameSettingRegistry* UGyraGameSettingRegistry::Get(UGyraLocalPlayer* InLocalPlayer)
{
	UGyraGameSettingRegistry* Registry = FindObject<UGyraGameSettingRegistry>(InLocalPlayer, TEXT("GyraGameSettingRegistry"), EFindObjectFlags::ExactClass);
	if (Registry == nullptr)
	{
		Registry = NewObject<UGyraGameSettingRegistry>(InLocalPlayer, TEXT("GyraGameSettingRegistry"));
		Registry->Initialize(InLocalPlayer);
	}

	return Registry;
}

bool UGyraGameSettingRegistry::IsFinishedInitializing() const
{
	if (Super::IsFinishedInitializing())
	{
		if (UGyraLocalPlayer* LocalPlayer = Cast<UGyraLocalPlayer>(OwningLocalPlayer))
		{
			if (LocalPlayer->GetSharedSettings() == nullptr)
			{
				return false;
			}
		}

		return true;
	}

	return false;
}

void UGyraGameSettingRegistry::OnInitialize(ULocalPlayer* InLocalPlayer)
{
	UGyraLocalPlayer* GyraLocalPlayer = Cast<UGyraLocalPlayer>(InLocalPlayer);

	VideoSettings = InitializeVideoSettings(GyraLocalPlayer);
	InitializeVideoSettings_FrameRates(VideoSettings, GyraLocalPlayer);
	RegisterSetting(VideoSettings);

	AudioSettings = InitializeAudioSettings(GyraLocalPlayer);
	RegisterSetting(AudioSettings);

	GameplaySettings = InitializeGameplaySettings(GyraLocalPlayer);
	RegisterSetting(GameplaySettings);

	MouseAndKeyboardSettings = InitializeMouseAndKeyboardSettings(GyraLocalPlayer);
	RegisterSetting(MouseAndKeyboardSettings);

	GamepadSettings = InitializeGamepadSettings(GyraLocalPlayer);
	RegisterSetting(GamepadSettings);
}

void UGyraGameSettingRegistry::SaveChanges()
{
	Super::SaveChanges();

	if (UGyraLocalPlayer* LocalPlayer = Cast<UGyraLocalPlayer>(OwningLocalPlayer))
	{
		// Game user settings need to be applied to handle things like resolution, this saves indirectly
		LocalPlayer->GetLocalSettings()->ApplySettings(false);

		LocalPlayer->GetSharedSettings()->ApplySettings();
		LocalPlayer->GetSharedSettings()->SaveSettings();
	}
}

#undef LOCTEXT_NAMESPACE

