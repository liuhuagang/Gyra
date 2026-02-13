// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "UObject/Interface.h"

#include "GyraWeaponTraceInterface.generated.h"

class UObject;
class UPhysicalMaterial;
struct FGameplayTagContainer;

/**
 * 动画通知开启检测的接口
 */
UINTERFACE()
class UGyraWeaponTraceInterface : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class IGyraWeaponTraceInterface
{
	GENERATED_IINTERFACE_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, Category = "Gyra|Weapon")
	void TraceStart();
	UFUNCTION(BlueprintImplementableEvent, Category = "Gyra|Weapon")
	void TraceEnd();
};
