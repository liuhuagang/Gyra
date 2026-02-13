// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "CommonUserWidget.h"

#include "GyraPerfStatWidgetBase.generated.h"

enum class EGyraDisplayablePerformanceStat : uint8;

class UGyraPerformanceStatSubsystem;
class UObject;
struct FFrame;

/**
 * UGyraPerfStatWidgetBase
 *
 * Base class for a widget that displays a single stat, e.g., FPS, ping, etc...
 */
 UCLASS(Abstract)
class UGyraPerfStatWidgetBase : public UCommonUserWidget
{
public:
	UGyraPerfStatWidgetBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	GENERATED_BODY()

public:
	// Returns the stat this widget is supposed to display
	UFUNCTION(BlueprintPure)
	EGyraDisplayablePerformanceStat GetStatToDisplay() const
	{
		return StatToDisplay;
	}

	// Polls for the value of this stat (unscaled)
	UFUNCTION(BlueprintPure)
	double FetchStatValue();

protected:
	// Cached subsystem pointer
	UPROPERTY(Transient)
	TObjectPtr<UGyraPerformanceStatSubsystem> CachedStatSubsystem;

	// The stat to display
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Display)
	EGyraDisplayablePerformanceStat StatToDisplay;
 };
