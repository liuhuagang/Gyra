// Copyright 2025 Huagang Liu. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GyraMutableFunctionLibrary.generated.h"

class UCustomizableObjectInstance;
struct FGyraMutableInfo;

UCLASS()
class UGyraMutableFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "Gyra|Mutable")
	static void SerializeMutableInfo(UCustomizableObjectInstance* InObject, FGyraMutableInfo& OutInfo);

	UFUNCTION(BlueprintCallable, Category = "Gyra|Mutable")
	static void DeserializeMutableInfo(UCustomizableObjectInstance* InObject, FGyraMutableInfo InInfo);

};
