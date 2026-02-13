// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "GameSettingValueDiscrete.h"

#include "GyraSettingValueDiscrete_PerfStat.generated.h"

enum class EGyraDisplayablePerformanceStat : uint8;
enum class EGyraStatDisplayMode : uint8;

class UObject;

UCLASS()
class UGyraSettingValueDiscrete_PerfStat : public UGameSettingValueDiscrete
{
	GENERATED_BODY()

public:

	UGyraSettingValueDiscrete_PerfStat();

	void SetStat(EGyraDisplayablePerformanceStat InStat);

	/** UGameSettingValue */
	virtual void StoreInitial() override;
	virtual void ResetToDefault() override;
	virtual void RestoreToInitial() override;

	/** UGameSettingValueDiscrete */
	virtual void SetDiscreteOptionByIndex(int32 Index) override;
	virtual int32 GetDiscreteOptionIndex() const override;
	virtual TArray<FText> GetDiscreteOptions() const override;

protected:
	/** UGameSettingValue */
	virtual void OnInitialized() override;

	void AddMode(FText&& Label, EGyraStatDisplayMode Mode);
protected:
	TArray<FText> Options;
	TArray<EGyraStatDisplayMode> DisplayModes;

	EGyraDisplayablePerformanceStat StatToDisplay;
	EGyraStatDisplayMode InitialMode;
};
