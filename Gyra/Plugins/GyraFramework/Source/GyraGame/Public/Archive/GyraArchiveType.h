// Copyright 2025 Huagang Liu. All Rights Reserved.

#pragma once

#include "GameFramework/SaveGame.h"
#include "GyraGameplayTags.h"

#include "GyraArchiveType.generated.h"

USTRUCT(BlueprintType)
struct FGyraInventoryTagStack
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Gyra|Archive")
	FGameplayTag AttributeTag = FGameplayTag::EmptyTag;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Gyra|Archive")
	int32 AttributeValue = 0;


};

USTRUCT(BlueprintType)
struct FGyraArchiveInventoryItemInfomation
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Gyra|Archive")
	FName InventoryName = NAME_None;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Gyra|Archive")
	int32 InventoryStack = -1;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Gyra|Archive")
	TArray<FGyraInventoryTagStack> Tags;

};

USTRUCT(BlueprintType)
struct FGyraArchiveListInfomation
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Gyra|Archive")
	TArray<FGyraArchiveInventoryItemInfomation> List;

};
