// Copyright 2024 Huagang Liu. All Rights Reserved.

#include "GyraAssetManager.h"

#include "AbilitySystemGlobals.h"
#include "Misc/App.h"
#include "Stats/StatsMisc.h"
#include "Engine/Engine.h"
#include "Misc/ScopedSlowTask.h"

#include "LogGyraGame.h"
#include "GyraGameplayTags.h"
#include "GyraGameData.h"
#include "GyraGameplayCueManager.h"
#include "GyraPawnData.h"
#include "GyraAssetManagerStartupJob.h"






#include UE_INLINE_GENERATED_CPP_BY_NAME(GyraAssetManager)

const FName FGyraBundles::Equipped("Equipped");
//////////////////////////////////////////////////////////////////////

static FAutoConsoleCommand CVarDumpLoadedAssets(
	TEXT("Gyra.DumpLoadedAssets"),
	TEXT("Shows all assets that were loaded via the asset manager and are currently in memory."),
	FConsoleCommandDelegate::CreateStatic(UGyraAssetManager::DumpLoadedAssets)
);

//////////////////////////////////////////////////////////////////////

#define STARTUP_JOB_WEIGHTED(JobFunc, JobWeight) StartupJobs.Add(FGyraAssetManagerStartupJob(#JobFunc, [this](const FGyraAssetManagerStartupJob& StartupJob, TSharedPtr<FStreamableHandle>& LoadHandle){JobFunc;}, JobWeight))
#define STARTUP_JOB(JobFunc) STARTUP_JOB_WEIGHTED(JobFunc, 1.f)

//////////////////////////////////////////////////////////////////////


UGyraAssetManager::UGyraAssetManager()
{
	DefaultPawnData = nullptr;
}

UGyraAssetManager& UGyraAssetManager::Get()
{
	check(GEngine);

	if (UGyraAssetManager* Singleton = Cast<UGyraAssetManager>(GEngine->AssetManager))
	{
		return *Singleton;
	}

	UE_LOG(LogGyraGame, Fatal, TEXT("Invalid AssetManagerClassName in DefaultEngine.ini.  It must be set to GyraAssetManager!"));

	// Fatal error above prevents this from being called.
	return *NewObject<UGyraAssetManager>();
}

void UGyraAssetManager::DumpLoadedAssets()
{
	UE_LOG(LogGyraGame, Log, TEXT("========== Start Dumping Loaded Assets =========="));

	for (const UObject* LoadedAsset : Get().LoadedAssets)
	{
		UE_LOG(LogGyraGame, Log, TEXT("  %s"), *GetNameSafe(LoadedAsset));
	}

	UE_LOG(LogGyraGame, Log, TEXT("... %d assets in loaded pool"), Get().LoadedAssets.Num());
	UE_LOG(LogGyraGame, Log, TEXT("========== Finish Dumping Loaded Assets =========="));


}

const UGyraGameData& UGyraAssetManager::GetGameData()
{
	return GetOrLoadTypedGameData<UGyraGameData>(GyraGameDataPath);
}

const UGyraPawnData* UGyraAssetManager::GetDefaultPawnData() const
{
	return GetAsset(DefaultPawnData);
}

UObject* UGyraAssetManager::SynchronousLoadAsset(const FSoftObjectPath& AssetPath)
{
	if (AssetPath.IsValid())
	{
		TUniquePtr<FScopeLogTime> LogTimePtr;

		if (ShouldLogAssetLoads())
		{
			LogTimePtr = MakeUnique<FScopeLogTime>(*FString::Printf(TEXT("Synchronously loaded asset [%s]"),
			*AssetPath.ToString()), nullptr, FScopeLogTime::ScopeLog_Seconds);
		}

		if (UAssetManager::IsInitialized())
		{
			return UAssetManager::GetStreamableManager().LoadSynchronous(AssetPath, false);
		}

		// 如果资产管理器还没有准备好，使用LoadObject。
		return AssetPath.TryLoad();
	}

	return nullptr;
}

bool UGyraAssetManager::ShouldLogAssetLoads()
{
	static bool bLogAssetLoads = FParse::Param(FCommandLine::Get(), TEXT("LogAssetLoads"));
	return bLogAssetLoads;
}

void UGyraAssetManager::AddLoadedAsset(const UObject* Asset)
{
	if (ensureAlways(Asset))
	{
		FScopeLock LoadedAssetsLock(&LoadedAssetsCritical);
		LoadedAssets.Add(Asset);
	}
}

void UGyraAssetManager::StartInitialLoading()
{
	SCOPED_BOOT_TIMING("UGyraAssetManager::StartInitialLoading");

	// This does all of the scanning, need to do this now even if loads are deferred
	// 这将完成所有的扫描，即使加载被延迟，现在也需要这样做
	Super::StartInitialLoading();

	STARTUP_JOB(InitializeGameplayCueManager());

	{
		// Load base game data asset
		STARTUP_JOB_WEIGHTED(GetGameData(), 25.f);
	}

	// Run all the queued up startup jobs
	DoAllStartupJobs();


}
#if WITH_EDITOR
void UGyraAssetManager::PreBeginPIE(bool bStartSimulate)
{
	Super::PreBeginPIE(bStartSimulate);


	{
		FScopedSlowTask SlowTask(0, NSLOCTEXT("GyraEditor", "BeginLoadingPIEData", "Loading PIE Data"));
		const bool bShowCancelButton = false;
		const bool bAllowInPIE = true;
		SlowTask.MakeDialog(bShowCancelButton, bAllowInPIE);

		const UGyraGameData& LocalGameDataCommon = GetGameData();

		// Intentionally after GetGameData to avoid counting GameData time in this timer
		// 故意在GetGameData之后，以避免在此计时器中计算GameData时间
		SCOPE_LOG_TIME_IN_SECONDS(TEXT("PreBeginPIE asset preloading complete"), nullptr);

		// You could add preloading of anything else needed for the experience we'll be using here
		// (e.g., by grabbing the default experience from the world settings + the experience override in developer settings)
		// 你可以添加预加载我们将在这里使用的体验所需的任何东西
		// （例如，从世界设置中获取默认体验+开发者设置中的体验覆盖）
	}

}
#endif

UPrimaryDataAsset* UGyraAssetManager::LoadGameDataOfClass(TSubclassOf<UPrimaryDataAsset> DataClass, const TSoftObjectPtr<UPrimaryDataAsset>& DataClassPath, FPrimaryAssetType PrimaryAssetType)
{
	UPrimaryDataAsset* Asset = nullptr;
	DECLARE_SCOPE_CYCLE_COUNTER(TEXT("Loading GameData Object"), STAT_GameData, STATGROUP_LoadTime);
	if (!DataClassPath.IsNull())
	{
#if WITH_EDITOR
		FScopedSlowTask SlowTask(0, FText::Format(NSLOCTEXT("GyraEditor", 
		"BeginLoadingGameDataTask", "Loading GameData {0}"), FText::FromName(DataClass->GetFName())));
		const bool bShowCancelButton = false;
		const bool bAllowInPIE = true;
		SlowTask.MakeDialog(bShowCancelButton, bAllowInPIE);
#endif

		UE_LOG(LogGyraGame, Log, TEXT("Loading GameData: %s ..."), *DataClassPath.ToString());
		SCOPE_LOG_TIME_IN_SECONDS(TEXT("    ... GameData loaded!"), nullptr);

		// This can be called recursively in the editor because it is called on demand from PostLoad so force a sync load for primary asset and async load the rest in that case
		// 这可以在编辑器中递归调用，因为它是按需从PostLoad调用的，所以在这种情况下，强制同步加载主资产和异步加载其余资产
		if (GIsEditor)
		{
			Asset = DataClassPath.LoadSynchronous();
			LoadPrimaryAssetsWithType(PrimaryAssetType);
		}
		else
		{
			TSharedPtr<FStreamableHandle> Handle = LoadPrimaryAssetsWithType(PrimaryAssetType);
			if (Handle.IsValid())
			{
				Handle->WaitUntilComplete(0.0f, false);

				// This should always work
				Asset = Cast<UPrimaryDataAsset>(Handle->GetLoadedAsset());
			}
		}


	}
	if (Asset)
	{
		GameDataMap.Add(DataClass, Asset);
	}
	else
	{
		// It is not acceptable to fail to load any GameData asset. It will result in soft failures that are hard to diagnose.
		// 无法加载任何GameData资产是不可接受的。这将导致难以诊断的软故障。
		UE_LOG(LogGyraGame, Fatal, TEXT("Failed to load GameData asset at %s. Type %s. This is not recoverable and likely means you do not have the correct data to run %s."), 
		*DataClassPath.ToString(), *PrimaryAssetType.ToString(), FApp::GetProjectName());
	}

	return Asset;
}




void UGyraAssetManager::DoAllStartupJobs()
{
	SCOPED_BOOT_TIMING("UGyraAssetManager::DoAllStartupJobs");
	const double AllStartupJobsStartTime = FPlatformTime::Seconds();

	if (IsRunningDedicatedServer())
	{
		// No need for periodic progress updates, just run the jobs
		// 不需要定期更新进度，只需运行作业
		for (const FGyraAssetManagerStartupJob& StartupJob : StartupJobs)
		{
			StartupJob.DoJob();
		}
	}
	else
	{

		if (StartupJobs.Num() > 0)
		{
			float TotalJobValue = 0.0f;
			for (const FGyraAssetManagerStartupJob& StartupJob : StartupJobs)
			{
				TotalJobValue += StartupJob.JobWeight;
			}

			float AccumulatedJobValue = 0.0f;
			for (FGyraAssetManagerStartupJob& StartupJob : StartupJobs)
			{
				const float JobValue = StartupJob.JobWeight;
				StartupJob.SubstepProgressDelegate.BindLambda([This = this, AccumulatedJobValue, JobValue, TotalJobValue](float NewProgress)
					{
						const float SubstepAdjustment = FMath::Clamp(NewProgress, 0.0f, 1.0f) * JobValue;
						const float OverallPercentWithSubstep = (AccumulatedJobValue + SubstepAdjustment) / TotalJobValue;

						This->UpdateInitialGameContentLoadPercent(OverallPercentWithSubstep);
					});

				StartupJob.DoJob();

				StartupJob.SubstepProgressDelegate.Unbind();

				AccumulatedJobValue += JobValue;

				UpdateInitialGameContentLoadPercent(AccumulatedJobValue / TotalJobValue);
			}
		}
		else
		{
			UpdateInitialGameContentLoadPercent(1.0f);
		}



	}


	StartupJobs.Empty();

	UE_LOG(LogGyraGame, Display, TEXT("All startup jobs took %.2f seconds to complete"), FPlatformTime::Seconds() - AllStartupJobsStartTime);

}

void UGyraAssetManager::InitializeGameplayCueManager()
{
	SCOPED_BOOT_TIMING("UGyraAssetManager::InitializeGameplayCueManager");

	UGyraGameplayCueManager* GCM = UGyraGameplayCueManager::Get();
	check(GCM);
	GCM->LoadAlwaysLoadedCues();
}

void UGyraAssetManager::UpdateInitialGameContentLoadPercent(float GameContentPercent)
{	
	// Could route this to the early startup loading screen
}
