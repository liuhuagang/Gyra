// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "IndicatorLibrary.generated.h"

class AController;
class UGyraIndicatorManagerComponent;
class UObject;
struct FFrame;

UCLASS()
class GYRAGAME_API UIndicatorLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UIndicatorLibrary();
	
	/**  */
	UFUNCTION(BlueprintCallable, Category = Indicator)
	static UGyraIndicatorManagerComponent* GetIndicatorManagerComponent(AController* Controller);
};
