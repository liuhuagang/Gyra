// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "CommonUserWidget.h"
#include "GyraPerformanceStatTypes.h"

#include "GyraPerfStatContainerBase.generated.h"

class UObject;
struct FFrame;

/**
 * UGyraPerfStatsContainerBase
 *
 * Panel that contains a set of UGyraPerfStatWidgetBase widgets and manages
 * their visibility based on user settings.
 */
 UCLASS(Abstract)
class UGyraPerfStatContainerBase : public UCommonUserWidget
{
public:
	UGyraPerfStatContainerBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	GENERATED_BODY()

	//~UUserWidget interface
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	//~End of UUserWidget interface

	UFUNCTION(BlueprintCallable)
	void UpdateVisibilityOfChildren();

protected:
	// Are we showing text or graph stats?
	UPROPERTY(EditAnywhere, Category=Display)
	EGyraStatDisplayMode StatDisplayModeFilter = EGyraStatDisplayMode::TextAndGraph;
};
