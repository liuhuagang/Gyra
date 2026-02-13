// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#pragma once

#include "Components/GameStateComponent.h"
#include "LoadingProcessInterface.h"

#include "GyraExperienceManagerComponent.generated.h"


namespace UE::GameFeatures { struct FResult; }

class UGyraExperienceDefinition;


DECLARE_MULTICAST_DELEGATE_OneParam(FOnGyraExperienceLoaded, const UGyraExperienceDefinition* /*Experience*/);

enum class EGyraExperienceLoadState
{
	Unloaded,
	Loading,
	LoadingGameFeatures,
	LoadingChaosTestingDelay,
	ExecutingActions,
	Loaded,
	Deactivating
};



UCLASS()
class UGyraExperienceManagerComponent final : public UGameStateComponent, public ILoadingProcessInterface
{
	GENERATED_BODY()

public:

	UGyraExperienceManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~UActorComponent interface
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~End of UActorComponent interface

	//~ILoadingProcessInterface interface
	virtual bool ShouldShowLoadingScreen(FString& OutReason) const override;
	//~End of ILoadingProcessInterface

	// Tries to set the current experience, either a UI or gameplay one
	// 尝试设置当前体验，无论是UI还是玩法
	void SetCurrentExperience(FPrimaryAssetId ExperienceId);

	// Ensures the delegate is called once the experience has been loaded,
	// before others are called.
	// However, if the experience has already loaded, calls the delegate immediately.
	// 确保在加载体验后调用委托，然后再调用其他委托。
	// 但是，如果体验已经加载，则立即调用委托。
	void CallOrRegister_OnExperienceLoaded_HighPriority(FOnGyraExperienceLoaded::FDelegate&& Delegate);

	// Ensures the delegate is called once the experience has been loaded
	// If the experience has already loaded, calls the delegate immediately
	// 确保在加载体验后调用委托
	// 如果体验已经加载，立即调用委托
	void CallOrRegister_OnExperienceLoaded(FOnGyraExperienceLoaded::FDelegate&& Delegate);

	// Ensures the delegate is called once the experience has been loaded
	// If the experience has already loaded, calls the delegate immediately
	// 确保在加载体验后调用委托
	// 如果体验已经加载，立即调用委托
	void CallOrRegister_OnExperienceLoaded_LowPriority(FOnGyraExperienceLoaded::FDelegate&& Delegate);

	// This returns the current experience if it is fully loaded, asserting otherwise
	// (i.e., if you called it too soon)
	// 如果它是完全加载的，返回当前的体验，否则断言
	// （例如，如果你太早调用它）
	const UGyraExperienceDefinition* GetCurrentExperienceChecked() const;

	// Returns true if the experience is fully loaded
	// 如果体验已完全加载，则返回true
	bool IsExperienceLoaded() const;

private:
	UFUNCTION()
	void OnRep_CurrentExperience();

	void StartExperienceLoad();
	void OnExperienceLoadComplete();
	void OnGameFeaturePluginLoadComplete(const UE::GameFeatures::FResult& Result);
	void OnExperienceFullLoadCompleted();

	void OnActionDeactivationCompleted();
	void OnAllActionsDeactivated();

private:
	UPROPERTY(ReplicatedUsing = OnRep_CurrentExperience)
	TObjectPtr<const UGyraExperienceDefinition> CurrentExperience;

	EGyraExperienceLoadState LoadState = EGyraExperienceLoadState::Unloaded;

	int32 NumGameFeaturePluginsLoading = 0;
	TArray<FString> GameFeaturePluginURLs;

	int32 NumObservedPausers = 0;
	int32 NumExpectedPausers = 0;

	/**
	 * Delegate called when the experience has finished loading just before others
	 * (e.g., subsystems that set up for regular gameplay)
	 * 
	 * 在体验完成加载之前调用委托
	 *（例如，为常规玩法设置的子系统）
	 */
	FOnGyraExperienceLoaded OnExperienceLoaded_HighPriority;

	/** Delegate called when the experience has finished loading */
	/** 当体验完成加载时调用委托 */
	FOnGyraExperienceLoaded OnExperienceLoaded;

	/** Delegate called when the experience has finished loading */
	/** 当体验完成加载时调用委托 */
	FOnGyraExperienceLoaded OnExperienceLoaded_LowPriority;
};
