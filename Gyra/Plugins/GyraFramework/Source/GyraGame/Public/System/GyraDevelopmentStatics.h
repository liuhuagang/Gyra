// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "Templates/SubclassOf.h"

#include "GyraDevelopmentStatics.generated.h"

class UClass;
class UObject;
class UWorld;
struct FAssetData;
struct FFrame;

UCLASS()
class UGyraDevelopmentStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// Should game logic skip directly to gameplay (skipping any match warmup / waiting for players / etc... aspects)
	// Will always return false except when playing in the editor and bTestFullGameFlowInPIE (in Gyra Developer Settings) is false
	// 游戏逻辑是否应直接进入游戏玩法阶段（跳过任何匹配前的热身环节/等待玩家等环节）
	// 除在编辑器中进行游戏时且 bTestFullGameFlowInPIE（在 Gyra 开发者设置中）为 false 时外，总是返回 false
	UFUNCTION(BlueprintCallable, Category = "Gyra")
	static bool ShouldSkipDirectlyToGameplay();

	// Should game logic load cosmetic backgrounds in the editor?
	// Will always return true except when playing in the editor and bSkipLoadingCosmeticBackgroundsInPIE (in Gyra Developer Settings) is true
	// 游戏逻辑是否应在编辑器中加载外观背景？
	// 除在编辑器中运行且“bSkipLoadingCosmeticBackgroundsInPIE”（在 Gyra 开发者设置中）为真时外，总是返回真。
	UFUNCTION(BlueprintCallable, Category = "Gyra", meta = (ExpandBoolAsExecs = "ReturnValue"))
	static bool ShouldLoadCosmeticBackgrounds();

	// Should game logic load cosmetic backgrounds in the editor?
	// Will always return true except when playing in the editor and bSkipLoadingCosmeticBackgroundsInPIE (in Gyra Developer Settings) is true
	// 游戏逻辑是否应在编辑器中加载外观背景？
	// 除在编辑器中运行且“bSkipLoadingCosmeticBackgroundsInPIE”（在 Gyra 开发者设置中）为真时外，总是返回真。
	UFUNCTION(BlueprintCallable, Category = "Gyra")
	static bool CanPlayerBotsAttack();

	// Finds the most appropriate play-in-editor world to run 'server' cheats on
	//  This might be the only world if running standalone, the listen server, or the dedicated server
	// 找出最适合运行“服务器”作弊功能的游戏加载编辑器世界
	// 如果以独立模式、监听服务器模式或专用服务器模式运行，则此世界可能是唯一可用的世界
	static UWorld* FindPlayInEditorAuthorityWorld();

	// Tries to find a class by a short name (with some heuristics to improve the usability when done via a cheat console)
	// 尝试通过简短的类名来查找类（通过一些优化策略来提高通过作弊控制台进行查找时的可用性）
	static UClass* FindClassByShortName(const FString& SearchToken, UClass* DesiredBaseClass, bool bLogFailures = true);

	template <typename DesiredClass>
	static TSubclassOf<DesiredClass> FindClassByShortName(const FString& SearchToken, bool bLogFailures = true)
	{
		return FindClassByShortName(SearchToken, DesiredClass::StaticClass(), bLogFailures);
	}

private:
	static TArray<FAssetData> GetAllBlueprints();
	static UClass* FindBlueprintClass(const FString& TargetNameRaw, UClass* DesiredBaseClass);
};
