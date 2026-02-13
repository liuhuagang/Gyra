// Copyright 2025 Huagang Liu. All Rights Reserved.

#include "GyraEditorModule.h"

#include "AbilitySystemGlobals.h"
#include "AssetToolsModule.h"
#include "AssetTypeActions_GyraContextEffectsLibrary.h"
#include "DataValidationModule.h"
#include "GameEditorStyle.h"
#include "Development/GyraDeveloperSettings.h"
#include "Editor/UnrealEdEngine.h"
#include "Engine/GameInstance.h"
#include "Framework/Application/SlateApplication.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "GameplayAbilitiesEditorModule.h"
#include "GameplayCueInterface.h"
#include "GameplayCueNotify_Burst.h"
#include "GameplayCueNotify_BurstLatent.h"
#include "GameplayCueNotify_Looping.h"
#include "GyraExperienceManager.h"
#include "ToolMenu.h"
#include "ToolMenus.h"
#include "UObject/UObjectIterator.h"
#include "UnrealEdGlobals.h"
#include "Validation/EditorValidator.h"
#include "LogGyraEditor.h"
class SWidget;

#define LOCTEXT_NAMESPACE "GyraEditor"

// This function tells the GameplayCue editor what classes to expose when creating new notifies.
// 此函数告知“游戏玩法提示”编辑器在创建新通知时要公开哪些类。
static void GetGameplayCueDefaultClasses(TArray<UClass*>& Classes)
{
	Classes.Empty();
	Classes.Add(UGameplayCueNotify_Burst::StaticClass());
	Classes.Add(AGameplayCueNotify_BurstLatent::StaticClass());
	Classes.Add(AGameplayCueNotify_Looping::StaticClass());
}

// This function tells the GameplayCue editor what classes to search for GameplayCue events.
// 此函数向游戏玩法提示编辑器指定应查找哪些类别的游戏玩法提示事件。
static void GetGameplayCueInterfaceClasses(TArray<UClass*>& Classes)
{
	Classes.Empty();

	for (UClass* Class : TObjectRange<UClass>())
	{
		if (Class->IsChildOf<AActor>() && Class->ImplementsInterface(UGameplayCueInterface::StaticClass()))
		{
			Classes.Add(Class);
		}
	}
}


// This function tells the GameplayCue editor where to create the GameplayCue notifies based on their tag.
// 此函数会根据游戏玩法提示的标签指示，告诉游戏玩法提示编辑器应在何处创建这些游戏玩法提示。
static FString GetGameplayCuePath(FString GameplayCueTag)
{
	FString Path = FString(TEXT("/Game"));

	//@TODO: Try plugins (e.g., GameplayCue.ShooterGame.Foo should be in ShooterCore or something)
	//@待办事项：尝试使用插件（例如，GameplayCue.ShooterGame.Foo 应该放在 ShooterCore 或类似的目录下）
	
	// Default path to the first entry in the UAbilitySystemGlobals::GameplayCueNotifyPaths.
	// UAbilitySystemGlobals::GameplayCueNotifyPaths 中第一个条目的默认路径。
	if (IGameplayAbilitiesModule::IsAvailable())
	{
		IGameplayAbilitiesModule& GameplayAbilitiesModule = IGameplayAbilitiesModule::Get();

		if (GameplayAbilitiesModule.IsAbilitySystemGlobalsAvailable())
		{
			UAbilitySystemGlobals* AbilitySystemGlobals = GameplayAbilitiesModule.GetAbilitySystemGlobals();
			check(AbilitySystemGlobals);

			TArray<FString> GetGameplayCueNotifyPaths = AbilitySystemGlobals->GetGameplayCueNotifyPaths();

			if (GetGameplayCueNotifyPaths.Num() > 0)
			{
				Path = GetGameplayCueNotifyPaths[0];
			}
		}
	}

	GameplayCueTag.RemoveFromStart(TEXT("GameplayCue."));

	FString NewDefaultPathName = FString::Printf(TEXT("%s/GCN_%s"), *Path, *GameplayCueTag);

	return NewDefaultPathName;
}

static bool HasPlayWorld()
{
	return GEditor->PlayWorld != nullptr;
}

static bool HasNoPlayWorld()
{
	return !HasPlayWorld();
}

static bool HasPlayWorldAndRunning()
{
	return HasPlayWorld() && !GUnrealEd->PlayWorld->bDebugPauseExecution;
}

static void OpenCommonMap_Clicked(const FString MapPath)
{
	if (ensure(MapPath.Len()))
	{
		GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OpenEditorForAsset(MapPath);
	}
}

static bool CanShowCommonMaps()
{
	return HasNoPlayWorld() && !GetDefault<UGyraDeveloperSettings>()->CommonEditorMaps.IsEmpty();
}


static TSharedRef<SWidget> GetCommonMapsDropdown()
{
	FMenuBuilder MenuBuilder(true, nullptr);

	for (const FSoftObjectPath& Path : GetDefault<UGyraDeveloperSettings>()->CommonEditorMaps)
	{
		if (!Path.IsValid())
		{
			continue;
		}

		const FText DisplayName = FText::FromString(Path.GetAssetName());
		MenuBuilder.AddMenuEntry(
			DisplayName,
			LOCTEXT("CommonPathDescription", "Opens this map in the editor"),
			FSlateIcon(),
			FUIAction(
				FExecuteAction::CreateStatic(&OpenCommonMap_Clicked, Path.ToString()),
				FCanExecuteAction::CreateStatic(&HasNoPlayWorld),
				FIsActionChecked(),
				FIsActionButtonVisible::CreateStatic(&HasNoPlayWorld)
			)
		);
	}

	return MenuBuilder.MakeWidget();
}

static void CheckGameContent_Clicked()
{
	UEditorValidator::ValidateCheckedOutContent(/*bInteractive=*/true, EDataValidationUsecase::Manual);
}

static void RegisterGameEditorMenus()
{
	UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
	FToolMenuSection& Section = Menu->AddSection("PlayGameExtensions", TAttribute<FText>(), FToolMenuInsert("Play", EToolMenuInsertType::After));

	// Uncomment this to add a custom toolbar that is displayed during PIE
	// Useful for making easy access to changing game state artificially, adding cheats, etc
	// FToolMenuEntry BlueprintEntry = FToolMenuEntry::InitComboButton(
	// 	"OpenGameMenu",
	// 	FUIAction(
	// 		FExecuteAction(),
	// 		FCanExecuteAction::CreateStatic(&HasPlayWorld),
	// 		FIsActionChecked(),
	// 		FIsActionButtonVisible::CreateStatic(&HasPlayWorld)),
	// 	FOnGetContent::CreateStatic(&YourCustomMenu),
	// 	LOCTEXT("GameOptions_Label", "Game Options"),
	// 	LOCTEXT("GameOptions_ToolTip", "Game Options"),
	// 	FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.OpenLevelBlueprint")
	// );
	// BlueprintEntry.StyleNameOverride = "CalloutToolbar";
	// Section.AddEntry(BlueprintEntry);

	FToolMenuEntry CheckContentEntry = FToolMenuEntry::InitToolBarButton(
		"CheckContent",
		FUIAction(
			FExecuteAction::CreateStatic(&CheckGameContent_Clicked),
			FCanExecuteAction::CreateStatic(&HasNoPlayWorld),
			FIsActionChecked(),
			FIsActionButtonVisible::CreateStatic(&HasNoPlayWorld)),
		LOCTEXT("CheckContentButton", "Check Content"),
		LOCTEXT("CheckContentDescription", "Runs the Content Validation job on all checked out assets to look for warnings and errors"),
		FSlateIcon(FGameEditorStyle::GetStyleSetName(), "GameEditor.CheckContent")
	);
	CheckContentEntry.StyleNameOverride = "CalloutToolbar";
	Section.AddEntry(CheckContentEntry);

	FToolMenuEntry CommonMapEntry = FToolMenuEntry::InitComboButton(
		"CommonMapOptions",
		FUIAction(
			FExecuteAction(),
			FCanExecuteAction::CreateStatic(&HasNoPlayWorld),
			FIsActionChecked(),
			FIsActionButtonVisible::CreateStatic(&CanShowCommonMaps)),
		FOnGetContent::CreateStatic(&GetCommonMapsDropdown),
		LOCTEXT("CommonMaps_Label", "Common Maps"),
		LOCTEXT("CommonMaps_ToolTip", "Some commonly desired maps while using the editor"),
		FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.Level")
	);
	CommonMapEntry.StyleNameOverride = "CalloutToolbar";
	Section.AddEntry(CommonMapEntry);
}

void FGyraEditorModule::StartupModule()
{
	FGameEditorStyle::Initialize();

	if (!IsRunningGame())
	{
		FModuleManager::Get().OnModulesChanged().AddRaw(this, &FGyraEditorModule::ModulesChangedCallback);

		BindGameplayAbilitiesEditorDelegates();

		if (FSlateApplication::IsInitialized())
		{
			ToolMenusHandle = UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateStatic(&RegisterGameEditorMenus));
		}

		FEditorDelegates::BeginPIE.AddRaw(this, &ThisClass::OnBeginPIE);
		FEditorDelegates::EndPIE.AddRaw(this, &ThisClass::OnEndPIE);
	}

	// Register the Context Effects Library asset type actions.
	{
		IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
		TSharedRef<FAssetTypeActions_GyraContextEffectsLibrary> AssetAction = MakeShared<FAssetTypeActions_GyraContextEffectsLibrary>();
		GyraContextEffectsLibraryAssetAction = AssetAction;
		AssetTools.RegisterAssetTypeActions(AssetAction);
	}
}

void FGyraEditorModule::ShutdownModule()
{
	// Unregister the Context Effects Library asset type actions.
	{
		FAssetToolsModule* AssetToolsModule = FModuleManager::GetModulePtr<FAssetToolsModule>("AssetTools");
		TSharedPtr<IAssetTypeActions> AssetAction = GyraContextEffectsLibraryAssetAction.Pin();
		if (AssetToolsModule && AssetAction)
		{
			AssetToolsModule->Get().UnregisterAssetTypeActions(AssetAction.ToSharedRef());
		}
	}

	FEditorDelegates::BeginPIE.RemoveAll(this);
	FEditorDelegates::EndPIE.RemoveAll(this);

	// Undo UToolMenus
	if (UObjectInitialized() && ToolMenusHandle.IsValid())
	{
		UToolMenus::UnRegisterStartupCallback(ToolMenusHandle);
	}

	UnbindGameplayAbilitiesEditorDelegates();
	FModuleManager::Get().OnModulesChanged().RemoveAll(this);
	FGameEditorStyle::Shutdown();
	
}

void FGyraEditorModule::OnBeginPIE(bool bIsSimulating)
{
	UGyraExperienceManager* ExperienceManager = GEngine->GetEngineSubsystem<UGyraExperienceManager>();
	check(ExperienceManager);
	ExperienceManager->OnPlayInEditorBegun();
}

void FGyraEditorModule::OnEndPIE(bool bIsSimulating)
{
	
}

 void FGyraEditorModule::BindGameplayAbilitiesEditorDelegates()
{
	IGameplayAbilitiesEditorModule& GameplayAbilitiesEditorModule = IGameplayAbilitiesEditorModule::Get();

	GameplayAbilitiesEditorModule.GetGameplayCueNotifyClassesDelegate().BindStatic(&GetGameplayCueDefaultClasses);
	GameplayAbilitiesEditorModule.GetGameplayCueInterfaceClassesDelegate().BindStatic(&GetGameplayCueInterfaceClasses);
	GameplayAbilitiesEditorModule.GetGameplayCueNotifyPathDelegate().BindStatic(&GetGameplayCuePath);
}

void FGyraEditorModule::UnbindGameplayAbilitiesEditorDelegates()
{
	if (IGameplayAbilitiesEditorModule::IsAvailable())
	{
		IGameplayAbilitiesEditorModule& GameplayAbilitiesEditorModule = IGameplayAbilitiesEditorModule::Get();
		GameplayAbilitiesEditorModule.GetGameplayCueNotifyClassesDelegate().Unbind();
		GameplayAbilitiesEditorModule.GetGameplayCueInterfaceClassesDelegate().Unbind();
		GameplayAbilitiesEditorModule.GetGameplayCueNotifyPathDelegate().Unbind();
	}
}

void FGyraEditorModule::ModulesChangedCallback(FName ModuleThatChanged, EModuleChangeReason ReasonForChange)
{
	if ((ReasonForChange == EModuleChangeReason::ModuleLoaded) && (ModuleThatChanged.ToString() == TEXT("GameplayAbilitiesEditor")))
	{
		BindGameplayAbilitiesEditorDelegates();
	}
}


#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FGyraEditorModule, GyraEditor)