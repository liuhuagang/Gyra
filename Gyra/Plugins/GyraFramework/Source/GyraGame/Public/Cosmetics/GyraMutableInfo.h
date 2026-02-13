// Copyright 2025 Huagang Liu. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GyraMutableInfo.generated.h"

class UCustomizableObjectInstance;

USTRUCT(BlueprintType)
struct FGyraMutableParameterInfo
{
	GENERATED_BODY()

	FGyraMutableParameterInfo();

	FGyraMutableParameterInfo(FString InParameterName, FString InParameterIntValue);

	FGyraMutableParameterInfo(FString InParameterName, bool InParameterBoolValue);

	virtual ~FGyraMutableParameterInfo();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Gyra|Mutable")
	bool bIsBool =false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Gyra|Mutable")
	FString ParameterName =TEXT("None");

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Gyra|Mutable")
	FString ParameterIntValue = TEXT("None");

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Gyra|Mutable")
	bool ParameterBoolValue = false;
};

USTRUCT(BlueprintType)
struct FGyraMutableInfo
{
	GENERATED_BODY()

public:

	FGyraMutableInfo();

	virtual ~FGyraMutableInfo();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Gyra|Mutable")
	bool bMale =false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Gyra|Mutable")
	TArray<FGyraMutableParameterInfo> ParameterList;

	void Add(FString InParameterName,FString InParameterIntValue);
	//void Add(FString InParameterName, bool InParameterBoolValue);

	void SerializeMutableInfo(UCustomizableObjectInstance* InObject);

	void DeserializeMutableInfo(UCustomizableObjectInstance* InObject);


};
