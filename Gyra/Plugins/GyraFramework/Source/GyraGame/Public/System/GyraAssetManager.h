// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "Engine/AssetManager.h"
#include "GyraAssetManagerStartupJob.h"
#include "Templates/SubclassOf.h"

#include "GyraAssetManager.generated.h"

class UPrimaryDataAsset;

class UGyraGameData;
class UGyraPawnData;

struct FGyraBundles
{
	static const FName Equipped;
};

/**
 * UGyraAssetManager
 *
 * Game implementation of the asset manager that overrides functionality and stores game-specific types.
 * It is expected that most games will want to override AssetManager as it provides a good place for game-specific loading logic.
 * This class is used by setting 'AssetManagerClassName' in DefaultEngine.ini.
 * 
 * 资产管理器的游戏执行，覆盖功能并存储游戏特定类型。
 * 预计大多数游戏都想要覆盖AssetManager，因为它为游戏特定的加载逻辑提供了一个很好的地方。
 * 这个类通过在defaultenengine .ini中设置“AssetManagerClassName”来使用。
 * 
 * 
 */
UCLASS(Config = Game)
class UGyraAssetManager : public UAssetManager
{
	GENERATED_BODY()

public:

	UGyraAssetManager();

	// Returns the AssetManager singleton object.
	// 返回AssetManager单例对象。
	static UGyraAssetManager& Get();

	// Returns the asset referenced by a TSoftObjectPtr.  This will synchronously load the asset if it's not already loaded.
	// 返回由TSoftObjectPtr对象引用的资源。这将同步加载尚未加载的资源。
	template<typename AssetType>
	static AssetType* GetAsset(const TSoftObjectPtr<AssetType>& AssetPointer, bool bKeepInMemory = true);

	// Returns the subclass referenced by a TSoftClassPtr.  This will synchronously load the asset if it's not already loaded
	// 返回由TSoftClassPtr引用的子类。这将同步加载尚未加载的资源
	template<typename AssetType>
	static TSubclassOf<AssetType> GetSubclass(const TSoftClassPtr<AssetType>& AssetPointer, bool bKeepInMemory = true);

	// Logs all assets currently loaded and tracked by the asset manager.
	// 记录当前被资产管理器加载和跟踪的所有资产。
	static void DumpLoadedAssets();

	const UGyraGameData& GetGameData();
	const UGyraPawnData* GetDefaultPawnData() const;

protected:

	template <typename GameDataClass>
	const GameDataClass& GetOrLoadTypedGameData(const TSoftObjectPtr<GameDataClass>& DataPath)
	{
		if (TObjectPtr<UPrimaryDataAsset> const* pResult = GameDataMap.Find(GameDataClass::StaticClass()))
		{
			return *CastChecked<GameDataClass>(*pResult);
		}

		// Does a blocking load if needed
		return *CastChecked<const GameDataClass>(LoadGameDataOfClass(GameDataClass::StaticClass(), DataPath, GameDataClass::StaticClass()->GetFName()));
	}

	static UObject* SynchronousLoadAsset(const FSoftObjectPath& AssetPath);
	static bool ShouldLogAssetLoads();


	// Thread safe way of adding a loaded asset to keep in memory.
	// 线程安全的方式添加一个加载的资源保存在内存中。
	void AddLoadedAsset(const UObject* Asset);

	//~UAssetManager interface
	virtual void StartInitialLoading() override;
#if WITH_EDITOR
	virtual void PreBeginPIE(bool bStartSimulate) override;
#endif
	//~End of UAssetManager interface

	UPrimaryDataAsset* LoadGameDataOfClass(TSubclassOf<UPrimaryDataAsset> DataClass, 
	const TSoftObjectPtr<UPrimaryDataAsset>& DataClassPath, FPrimaryAssetType PrimaryAssetType);

protected:

	// Global game data asset to use.
	// 使用全局游戏数据资产。
	UPROPERTY(Config)
	TSoftObjectPtr<UGyraGameData> GyraGameDataPath;

	// Loaded version of the game data
	// 加载版本的游戏数据
	UPROPERTY(Transient)
	TMap<TObjectPtr<UClass>, TObjectPtr<UPrimaryDataAsset>> GameDataMap;

	// Pawn data used when spawning player pawns if there isn't one set on the player state.
	// 如果玩家状态没有设置，则在生成玩家的棋子时使用默认棋子数据。
	UPROPERTY(Config)
	TSoftObjectPtr<UGyraPawnData> DefaultPawnData;

private:
	
	// Flushes the StartupJobs array. Processes all startup work.
	// 刷新StartupJobs数组。处理所有启动工作。
	void DoAllStartupJobs();

	// Sets up the ability system
	// 设置能力系统
	void InitializeGameplayCueManager();

	// Called periodically during loads, could be used to feed the status to a loading screen
	// 在加载期间定期调用，可用于向加载屏幕提供状态
	void UpdateInitialGameContentLoadPercent(float GameContentPercent);

	// The list of tasks to execute on startup. Used to track startup progress.
	// 启动时要执行的任务列表。用于跟踪启动进度。
	TArray<FGyraAssetManagerStartupJob> StartupJobs;

private:

	// Assets loaded and tracked by the asset manager.
	// 资产管理器加载和跟踪的资产。
	UPROPERTY()
	TSet<TObjectPtr<const UObject>> LoadedAssets;

	// Used for a scope lock when modifying the list of load assets.
	// 用于修改加载资源列表时的作用域锁定。
	FCriticalSection LoadedAssetsCritical;

};

template<typename AssetType>
AssetType* UGyraAssetManager::GetAsset(const TSoftObjectPtr<AssetType>& AssetPointer, bool bKeepInMemory)
{
	AssetType* LoadedAsset = nullptr;

	const FSoftObjectPath& AssetPath = AssetPointer.ToSoftObjectPath();

	if (AssetPath.IsValid())
	{
		LoadedAsset = AssetPointer.Get();
		if (!LoadedAsset)
		{
			LoadedAsset = Cast<AssetType>(SynchronousLoadAsset(AssetPath));
			ensureAlwaysMsgf(LoadedAsset, TEXT("Failed to load asset [%s]"), *AssetPointer.ToString());
		}

		if (LoadedAsset && bKeepInMemory)
		{
			// Added to loaded asset list.
			// 添加到加载的资产列表。
			Get().AddLoadedAsset(Cast<UObject>(LoadedAsset));
		}
	}

	return LoadedAsset;
}



template<typename AssetType>
TSubclassOf<AssetType> UGyraAssetManager::GetSubclass(const TSoftClassPtr<AssetType>& AssetPointer, bool bKeepInMemory)
{
	TSubclassOf<AssetType> LoadedSubclass;

	const FSoftObjectPath& AssetPath = AssetPointer.ToSoftObjectPath();

	if (AssetPath.IsValid())
	{
		LoadedSubclass = AssetPointer.Get();
		if (!LoadedSubclass)
		{
			LoadedSubclass = Cast<UClass>(SynchronousLoadAsset(AssetPath));
			ensureAlwaysMsgf(LoadedSubclass, TEXT("Failed to load asset class [%s]"), *AssetPointer.ToString());
		}

		if (LoadedSubclass && bKeepInMemory)
		{
			// Added to loaded asset list.
			// 添加到加载的资产列表。
			Get().AddLoadedAsset(Cast<UObject>(LoadedSubclass));
		}
	}

	return LoadedSubclass;
}
