// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "CommonUserWidget.h"

#include "GyraWeaponUserInterface.generated.h"

class UGyraWeaponInstance;
class UObject;
struct FGeometry;

UCLASS()
class UGyraWeaponUserInterface : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	UGyraWeaponUserInterface(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION(BlueprintImplementableEvent)
	void OnWeaponChanged(UGyraWeaponInstance* OldWeapon, UGyraWeaponInstance* NewWeapon);

private:
	void RebuildWidgetFromWeapon();

private:
	UPROPERTY(Transient)
	TObjectPtr<UGyraWeaponInstance> CurrentInstance;
};
