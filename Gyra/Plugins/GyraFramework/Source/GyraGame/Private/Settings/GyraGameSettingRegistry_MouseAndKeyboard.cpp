// Copyright 2024 Huagang Liu. All Rights Reserved.

#include "CommonInputBaseTypes.h"
#include "EnhancedInputSubsystems.h"
#include "GyraSettingKeyboardInput.h"
#include "DataSource/GameSettingDataSource.h"
#include "EditCondition/WhenCondition.h"
#include "GameSettingCollection.h"
#include "GameSettingValueDiscreteDynamic.h"
#include "GameSettingValueScalarDynamic.h"
#include "GyraGameSettingRegistry.h"
#include "GyraSettingsLocal.h"
#include "GyraSettingsShared.h"
#include "Player/GyraLocalPlayer.h"
#include "PlayerMappableInputConfig.h"

class ULocalPlayer;

#define LOCTEXT_NAMESPACE "Gyra"

UGameSettingCollection* UGyraGameSettingRegistry::InitializeMouseAndKeyboardSettings(UGyraLocalPlayer* InLocalPlayer)
{
	UGameSettingCollection* Screen = NewObject<UGameSettingCollection>();
	Screen->SetDevName(TEXT("MouseAndKeyboardCollection"));
	Screen->SetDisplayName(LOCTEXT("MouseAndKeyboardCollection_Name", "Mouse & Keyboard"));
	Screen->Initialize(InLocalPlayer);

	const TSharedRef<FWhenCondition> WhenPlatformSupportsMouseAndKeyboard = MakeShared<FWhenCondition>(
		[](const ULocalPlayer*, FGameSettingEditableState& InOutEditState)
		{
			const UCommonInputPlatformSettings* PlatformInput = UPlatformSettingsManager::Get().GetSettingsForPlatform<UCommonInputPlatformSettings>();
			if (!PlatformInput->SupportsInputType(ECommonInputType::MouseAndKeyboard))
			{
				InOutEditState.Kill(TEXT("Platform does not support mouse and keyboard"));
			}
		});

	// Mouse Sensitivity
	////////////////////////////////////////////////////////////////////////////////////
	{
		UGameSettingCollection* Sensitivity = NewObject<UGameSettingCollection>();
		Sensitivity->SetDevName(TEXT("MouseSensitivityCollection"));
		Sensitivity->SetDisplayName(LOCTEXT("MouseSensitivityCollection_Name", "Sensitivity"));
		Screen->AddSetting(Sensitivity);

		//----------------------------------------------------------------------------------
		{
			UGameSettingValueScalarDynamic* Setting = NewObject<UGameSettingValueScalarDynamic>();
			Setting->SetDevName(TEXT("MouseSensitivityYaw"));
			Setting->SetDisplayName(LOCTEXT("MouseSensitivityYaw_Name", "X-Axis Sensitivity"));
			Setting->SetDescriptionRichText(LOCTEXT("MouseSensitivityYaw_Description", "Sets the sensitivity of the mouse's horizontal (x) axis. With higher settings the camera will move faster when looking left and right with the mouse."));

			Setting->SetDynamicGetter(GET_SHARED_SETTINGS_FUNCTION_PATH(GetMouseSensitivityX));
			Setting->SetDynamicSetter(GET_SHARED_SETTINGS_FUNCTION_PATH(SetMouseSensitivityX));
			Setting->SetDefaultValue(GetDefault<UGyraSettingsShared>()->GetMouseSensitivityX());
			Setting->SetDisplayFormat(UGameSettingValueScalarDynamic::RawTwoDecimals);
			Setting->SetSourceRangeAndStep(TRange<double>(0, 10), 0.01);
			Setting->SetMinimumLimit(0.01);

			Setting->AddEditCondition(WhenPlatformSupportsMouseAndKeyboard);

			Sensitivity->AddSetting(Setting);
		}
		//----------------------------------------------------------------------------------
		{
			UGameSettingValueScalarDynamic* Setting = NewObject<UGameSettingValueScalarDynamic>();
			Setting->SetDevName(TEXT("MouseSensitivityPitch"));
			Setting->SetDisplayName(LOCTEXT("MouseSensitivityPitch_Name", "Y-Axis Sensitivity"));
			Setting->SetDescriptionRichText(LOCTEXT("MouseSensitivityPitch_Description", "Sets the sensitivity of the mouse's vertical (y) axis. With higher settings the camera will move faster when looking up and down with the mouse."));

			Setting->SetDynamicGetter(GET_SHARED_SETTINGS_FUNCTION_PATH(GetMouseSensitivityY));
			Setting->SetDynamicSetter(GET_SHARED_SETTINGS_FUNCTION_PATH(SetMouseSensitivityY));
			Setting->SetDefaultValue(GetDefault<UGyraSettingsShared>()->GetMouseSensitivityY());
			Setting->SetDisplayFormat(UGameSettingValueScalarDynamic::RawTwoDecimals);
			Setting->SetSourceRangeAndStep(TRange<double>(0, 10), 0.01);
			Setting->SetMinimumLimit(0.01);

			Setting->AddEditCondition(WhenPlatformSupportsMouseAndKeyboard);

			Sensitivity->AddSetting(Setting);
		}
		//----------------------------------------------------------------------------------
		{
			UGameSettingValueScalarDynamic* Setting = NewObject<UGameSettingValueScalarDynamic>();
			Setting->SetDevName(TEXT("MouseTargetingMultiplier"));
			Setting->SetDisplayName(LOCTEXT("MouseTargetingMultiplier_Name", "Targeting Sensitivity"));
			Setting->SetDescriptionRichText(LOCTEXT("MouseTargetingMultiplier_Description", "Sets the modifier for reducing mouse sensitivity when targeting. 100% will have no slow down when targeting. Lower settings will have more slow down when targeting."));

			Setting->SetDynamicGetter(GET_SHARED_SETTINGS_FUNCTION_PATH(GetTargetingMultiplier));
			Setting->SetDynamicSetter(GET_SHARED_SETTINGS_FUNCTION_PATH(SetTargetingMultiplier));
			Setting->SetDefaultValue(GetDefault<UGyraSettingsShared>()->GetTargetingMultiplier());
			Setting->SetDisplayFormat(UGameSettingValueScalarDynamic::RawTwoDecimals);
			Setting->SetSourceRangeAndStep(TRange<double>(0, 10), 0.01);
			Setting->SetMinimumLimit(0.01);

			Setting->AddEditCondition(WhenPlatformSupportsMouseAndKeyboard);

			Sensitivity->AddSetting(Setting);
		}
		//----------------------------------------------------------------------------------
		{
			UGameSettingValueDiscreteDynamic_Bool* Setting = NewObject<UGameSettingValueDiscreteDynamic_Bool>();
			Setting->SetDevName(TEXT("InvertVerticalAxis"));
			Setting->SetDisplayName(LOCTEXT("InvertVerticalAxis_Name", "Invert Vertical Axis"));
			Setting->SetDescriptionRichText(LOCTEXT("InvertVerticalAxis_Description", "Enable the inversion of the vertical look axis."));

			Setting->SetDynamicGetter(GET_SHARED_SETTINGS_FUNCTION_PATH(GetInvertVerticalAxis));
			Setting->SetDynamicSetter(GET_SHARED_SETTINGS_FUNCTION_PATH(SetInvertVerticalAxis));
			Setting->SetDefaultValue(GetDefault<UGyraSettingsShared>()->GetInvertVerticalAxis());

			Setting->AddEditCondition(WhenPlatformSupportsMouseAndKeyboard);

			Sensitivity->AddSetting(Setting);
		}
		//----------------------------------------------------------------------------------
		{
			UGameSettingValueDiscreteDynamic_Bool* Setting = NewObject<UGameSettingValueDiscreteDynamic_Bool>();
			Setting->SetDevName(TEXT("InvertHorizontalAxis"));
			Setting->SetDisplayName(LOCTEXT("InvertHorizontalAxis_Name", "Invert Horizontal Axis"));
			Setting->SetDescriptionRichText(LOCTEXT("InvertHorizontalAxis_Description", "Enable the inversion of the Horizontal look axis."));

			Setting->SetDynamicGetter(GET_SHARED_SETTINGS_FUNCTION_PATH(GetInvertHorizontalAxis));
			Setting->SetDynamicSetter(GET_SHARED_SETTINGS_FUNCTION_PATH(SetInvertHorizontalAxis));
			Setting->SetDefaultValue(GetDefault<UGyraSettingsShared>()->GetInvertHorizontalAxis());

			Setting->AddEditCondition(WhenPlatformSupportsMouseAndKeyboard);

			Sensitivity->AddSetting(Setting);
		}
		//----------------------------------------------------------------------------------
	}

	// Bindings for Mouse & Keyboard - Automatically Generated
	////////////////////////////////////////////////////////////////////////////////////
	{
		UGameSettingCollection* KeyBinding = NewObject<UGameSettingCollection>();
		KeyBinding->SetDevName(TEXT("KeyBindingCollection"));
		KeyBinding->SetDisplayName(LOCTEXT("KeyBindingCollection_Name", "Keyboard & Mouse"));
		Screen->AddSetting(KeyBinding);

		const UEnhancedInputLocalPlayerSubsystem* EISubsystem = InLocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
		const UEnhancedInputUserSettings* UserSettings = EISubsystem->GetUserSettings();

		// If you want to just get one profile pair, then you can do UserSettings->GetCurrentProfile

		// A map of key bindings mapped to their display category
		TMap<FString, UGameSettingCollection*> CategoryToSettingCollection;

		// Returns an existing setting collection for the display category if there is one.
		// If there isn't one, then it will create a new one and initialize it
		auto GetOrCreateSettingCollection = [&CategoryToSettingCollection, &Screen](FText DisplayCategory) -> UGameSettingCollection*
			{
				static const FString DefaultDevName = TEXT("Default_KBM");
				static const FText DefaultDevDisplayName = NSLOCTEXT("GyraInputSettings", "GyraInputDefaults", "Default Experiences");

				if (DisplayCategory.IsEmpty())
				{
					DisplayCategory = DefaultDevDisplayName;
				}

				FString DisplayCatString = DisplayCategory.ToString();

				if (UGameSettingCollection** ExistingCategory = CategoryToSettingCollection.Find(DisplayCatString))
				{
					return *ExistingCategory;
				}

				UGameSettingCollection* ConfigSettingCollection = NewObject<UGameSettingCollection>();
				ConfigSettingCollection->SetDevName(FName(DisplayCatString));
				ConfigSettingCollection->SetDisplayName(DisplayCategory);
				Screen->AddSetting(ConfigSettingCollection);
				CategoryToSettingCollection.Add(DisplayCatString, ConfigSettingCollection);

				return ConfigSettingCollection;
			};

		static TSet<FName> CreatedMappingNames;
		CreatedMappingNames.Reset();

		// 这里需要解释一下为什么编辑器看不到键位选项 是因为没有注册输入配置 说白了就是空的!!!! 因为编辑器下和打包之后 那个UserSettings的实例化时机不一样
		// 如何修复也很简单 直接在Experience里面添加以下指定的设置即可!
		for (const TPair<FString, TObjectPtr<UEnhancedPlayerMappableKeyProfile>>& ProfilePair : UserSettings->GetAllAvailableKeyProfiles())
		{
			const FString& ProfileName = ProfilePair.Key;
			const TObjectPtr<UEnhancedPlayerMappableKeyProfile>& Profile = ProfilePair.Value;

			for (const TPair<FName, FKeyMappingRow>& RowPair : Profile->GetPlayerMappingRows())
			{
				// Create a setting row for anything with valid mappings and that we haven't created yet
				// 为具有有效映射关系且尚未创建的任何内容创建一个设置行
				if (RowPair.Value.HasAnyMappings() /* && !CreatedMappingNames.Contains(RowPair.Key)*/)
				{
					// We only want keyboard keys on this settings screen, so we will filter down by mappings
					// that are set to keyboard keys
					// 我们只希望在该设置界面中显示键盘按键，因此我们将通过筛选具有键盘按键映射设置的选项来进行过滤。
					FPlayerMappableKeyQueryOptions Options = {};
					Options.KeyToMatch = EKeys::W;
					Options.bMatchBasicKeyTypes = true;
															
					const FText& DesiredDisplayCategory = RowPair.Value.Mappings.begin()->GetDisplayCategory();
					
					if (UGameSettingCollection* Collection = GetOrCreateSettingCollection(DesiredDisplayCategory))
					{
						// Create the settings widget and initialize it, adding it to this config's section
						UGyraSettingKeyboardInput* InputBinding = NewObject<UGyraSettingKeyboardInput>();

						InputBinding->InitializeInputData(Profile, RowPair.Value, Options);
						InputBinding->AddEditCondition(WhenPlatformSupportsMouseAndKeyboard);

						Collection->AddSetting(InputBinding);
						CreatedMappingNames.Add(RowPair.Key);
					}
					else
					{
						ensure(false);
					}
				}
			}
		}
		
	}

	return Screen;
}

#undef LOCTEXT_NAMESPACE