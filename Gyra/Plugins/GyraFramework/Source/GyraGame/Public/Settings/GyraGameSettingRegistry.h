// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "DataSource/GameSettingDataSourceDynamic.h" // IWYU pragma: keep
#include "GameSettingRegistry.h"
#include "GyraSettingsLocal.h" // IWYU pragma: keep

#include "GyraGameSettingRegistry.generated.h"

class ULocalPlayer;
class UObject;

//--------------------------------------
// UGyraGameSettingRegistry
//--------------------------------------

class UGameSettingCollection;
class UGyraLocalPlayer;

DECLARE_LOG_CATEGORY_EXTERN(LogGyraGameSettingRegistry, Log, Log);

#define GET_SHARED_SETTINGS_FUNCTION_PATH(FunctionOrPropertyName)							\
	MakeShared<FGameSettingDataSourceDynamic>(TArray<FString>({								\
		GET_FUNCTION_NAME_STRING_CHECKED(UGyraLocalPlayer, GetSharedSettings),				\
		GET_FUNCTION_NAME_STRING_CHECKED(UGyraSettingsShared, FunctionOrPropertyName)		\
	}))

#define GET_LOCAL_SETTINGS_FUNCTION_PATH(FunctionOrPropertyName)							\
	MakeShared<FGameSettingDataSourceDynamic>(TArray<FString>({								\
		GET_FUNCTION_NAME_STRING_CHECKED(UGyraLocalPlayer, GetLocalSettings),				\
		GET_FUNCTION_NAME_STRING_CHECKED(UGyraSettingsLocal, FunctionOrPropertyName)		\
	}))

/**
 *
 */
UCLASS()
class UGyraGameSettingRegistry : public UGameSettingRegistry
{
	GENERATED_BODY()

public:
	UGyraGameSettingRegistry();

	static UGyraGameSettingRegistry* Get(UGyraLocalPlayer* InLocalPlayer);

	virtual void SaveChanges() override;

protected:
	virtual void OnInitialize(ULocalPlayer* InLocalPlayer) override;
	virtual bool IsFinishedInitializing() const override;

	UGameSettingCollection* InitializeVideoSettings(UGyraLocalPlayer* InLocalPlayer);
	void InitializeVideoSettings_FrameRates(UGameSettingCollection* Screen, UGyraLocalPlayer* InLocalPlayer);
	void AddPerformanceStatPage(UGameSettingCollection* Screen, UGyraLocalPlayer* InLocalPlayer);

	UGameSettingCollection* InitializeAudioSettings(UGyraLocalPlayer* InLocalPlayer);
	UGameSettingCollection* InitializeGameplaySettings(UGyraLocalPlayer* InLocalPlayer);

	UGameSettingCollection* InitializeMouseAndKeyboardSettings(UGyraLocalPlayer* InLocalPlayer);
	UGameSettingCollection* InitializeGamepadSettings(UGyraLocalPlayer* InLocalPlayer);

	UPROPERTY()
	TObjectPtr<UGameSettingCollection> VideoSettings;

	UPROPERTY()
	TObjectPtr<UGameSettingCollection> AudioSettings;

	UPROPERTY()
	TObjectPtr<UGameSettingCollection> GameplaySettings;

	UPROPERTY()
	TObjectPtr<UGameSettingCollection> MouseAndKeyboardSettings;

	UPROPERTY()
	TObjectPtr<UGameSettingCollection> GamepadSettings;
};
