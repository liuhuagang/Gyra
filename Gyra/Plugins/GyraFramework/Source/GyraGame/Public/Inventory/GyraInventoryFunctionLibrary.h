// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GyraInventoryItemInfo.h"

#include "GyraInventoryFunctionLibrary.generated.h"

template <typename T> class TSubclassOf;

class UGyraInventoryItemInstance;
class UGyraInventoryItemFragment;
class UStaticMesh;
class AGyraWorldCollectable;

//@TODO: Make into a subsystem instead?
//@TODO: 后续是否需要做一个子系统?
UCLASS()
class UGyraInventoryFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintPure, Category = "Gyra|Inventory", meta = (DeterminesOutputType = FragmentClass))
	static const UGyraInventoryItemFragment* FindItemDefinitionFragment(TSubclassOf<UGyraInventoryItemDefinition> ItemDef, TSubclassOf<UGyraInventoryItemFragment> FragmentClass);

	UFUNCTION(BlueprintPure, Category = "Gyra|Inventory")
	static const FName FindItemDefinitionUnique(TSubclassOf<UGyraInventoryItemDefinition> ItemDef);


	UFUNCTION(BlueprintPure, Category = "Gyra|Inventory")
	static const int32 FindItemMaxStackByDefintion(const TSubclassOf<UGyraInventoryItemDefinition> InItemDefClass);

	UFUNCTION(BlueprintPure, Category = "Gyra|Inventory")
	static const int32 FindItemMaxStackByInstance(const UGyraInventoryItemInstance* InItemInstance);

	/** 根据物品定义找到物品类型 */
	UFUNCTION(BlueprintPure, Category = "Gyra|Inventory")
	static const EGyraInventoryItemType FindItemTypeByDefintion(const TSubclassOf<UGyraInventoryItemDefinition> InItemDefClass);
	/** 根据物品实例找到物品类型 */
	UFUNCTION(BlueprintPure, Category = "Gyra|Inventory")
	static const EGyraInventoryItemType FindItemTypeByInstance(const UGyraInventoryItemInstance* InItemInstance);


	/** 根据物品定义找到物品的具体装备类型 */
	UFUNCTION( BlueprintPure, Category = "Gyra|Inventory")
	static const EGyraEquipmentItemType FindItemEquipmentTypeByDefintion(const TSubclassOf<UGyraInventoryItemDefinition> InItemDefClass);
	/** 根据物品实例找到物品的具体装备类型 */
	UFUNCTION(BlueprintPure, Category = "Gyra|Inventory")
	static const EGyraEquipmentItemType FindItemEquipmentTypeByInstance(const UGyraInventoryItemInstance* InItemInstance);






	UFUNCTION(BlueprintPure, Category = "Gyra|Inventory")
	static const EGyraInventoryItemQuality FindItemQualityByDefintion(const TSubclassOf<UGyraInventoryItemDefinition> InItemDefClass);

	UFUNCTION(BlueprintPure, Category = "Gyra|Inventory")
	static const EGyraInventoryItemQuality FindItemQualityByInstance(const UGyraInventoryItemInstance* InItemInstance);

	UFUNCTION(BlueprintPure, Category = "Gyra|Inventory")
	static const FLinearColor FindItemQualityColorByDefintion(const TSubclassOf<UGyraInventoryItemDefinition> InItemDefClass);

	UFUNCTION(BlueprintPure, Category = "Gyra|Inventory")
	static const FLinearColor FindItemQualityColorByInstance(const UGyraInventoryItemInstance* InItemInstance);

	UFUNCTION(BlueprintPure, Category = "Gyra|Inventory")
	static UStaticMesh* FindItemMeshByDefinition(const TSubclassOf<UGyraInventoryItemDefinition> InItemDefClass);





	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Gyra|Inventory", meta = (WorldContext = "WorldContextObject"))
	static AGyraWorldCollectable* SpawnDropItemInstance(
	UObject* WorldContextObject,
	AActor* InOwner,
	FVector InSpawnLocation,
	UGyraInventoryItemInstance* InItemInstance,
	int32 InStackNum);


};
