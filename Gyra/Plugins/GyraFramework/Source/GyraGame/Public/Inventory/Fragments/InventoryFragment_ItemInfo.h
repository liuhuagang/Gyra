// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "UObject/ObjectPtr.h"
#include "GameplayTagContainer.h"


#include "GyraInventoryItemDefinition.h"
#include "GyraInventoryItemInfo.h"
#include "GyraInventoryItemInstance.h"


#include "InventoryFragment_ItemInfo.generated.h"



class UObject;
class USkeletalMesh;
class UTexture2D;
class UStaticMesh;

UCLASS()
class UInventoryFragment_ItemInfo : public UGyraInventoryItemFragment
{
	GENERATED_BODY()

public:
	UInventoryFragment_ItemInfo();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Inventory)
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Inventory)
	TObjectPtr<UTexture2D> Icon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Inventory)
	EGyraInventoryItemType Type;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Inventory)
	EGyraInventoryItemQuality Quality;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Inventory)
	TObjectPtr<UStaticMesh> Mesh;


public:

	virtual void OnInstanceCreated(UGyraInventoryItemInstance* Instance) const override;



};
