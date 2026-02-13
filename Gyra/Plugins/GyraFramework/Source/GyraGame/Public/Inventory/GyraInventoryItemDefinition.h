// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "GyraInventoryItemInfo.h"
#include "GyraInventoryItemDefinition.generated.h"

template <typename T> class TSubclassOf;

class UGyraInventoryItemInstance;
struct FFrame;

//////////////////////////////////////////////////////////////////////

// Represents a fragment of an item definition
UCLASS(DefaultToInstanced, EditInlineNew, Abstract)
class GYRAGAME_API UGyraInventoryItemFragment : public UObject
{
	GENERATED_BODY()

public:
	virtual void OnInstanceCreated(UGyraInventoryItemInstance* Instance) const {}
};

//////////////////////////////////////////////////////////////////////

/**
 * UGyraInventoryItemDefinition
 */
UCLASS(Blueprintable, Const, Abstract)
class GYRAGAME_API UGyraInventoryItemDefinition : public UObject
{
	GENERATED_BODY()

public:
	UGyraInventoryItemDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Display)
	FName UniqueName=NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Display, Instanced)
	TArray<TObjectPtr<UGyraInventoryItemFragment>> Fragments;

public:
	const UGyraInventoryItemFragment* FindFragmentByClass(TSubclassOf<UGyraInventoryItemFragment> FragmentClass) const;
};

