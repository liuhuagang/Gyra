// Copyright 2024 Huagang Liu. All Rights Reserved.

#include "GyraInventoryFunctionLibrary.h"


#include "GyraInventoryItemDefinition.h"
#include "GyraInventoryItemInstance.h"
#include "GyraInventoryItemDefinition.h"


#include "InventoryFragment_EquippableItem.h"
#include "InventoryFragment_ItemInfo.h"
#include "InventoryFragment_SetStats.h"

#include "GyraGameplayTags.h"
#include "GyraGameSettings.h"

const UGyraInventoryItemFragment* UGyraInventoryFunctionLibrary::FindItemDefinitionFragment(TSubclassOf<UGyraInventoryItemDefinition> ItemDef, TSubclassOf<UGyraInventoryItemFragment> FragmentClass)
{
	if ((ItemDef != nullptr) && (FragmentClass != nullptr))
	{
		return GetDefault<UGyraInventoryItemDefinition>(ItemDef)->FindFragmentByClass(FragmentClass);
	}
	return nullptr;
}

const FName UGyraInventoryFunctionLibrary::FindItemDefinitionUnique(TSubclassOf<UGyraInventoryItemDefinition> ItemDef)
{
	if ((ItemDef != nullptr))
	{
		return GetDefault<UGyraInventoryItemDefinition>(ItemDef)->UniqueName;
	}
	return NAME_None;
}

const int32 UGyraInventoryFunctionLibrary::FindItemMaxStackByDefintion(TSubclassOf<UGyraInventoryItemDefinition> InItemDefClass)
{
	int32 MaxStack = 1;

	if (InItemDefClass != nullptr)
	{
		UGyraInventoryFunctionLibrary::FindItemDefinitionFragment(InItemDefClass, UInventoryFragment_SetStats::StaticClass());
		const UGyraInventoryItemFragment* ItemFragmeny = UGyraInventoryFunctionLibrary::FindItemDefinitionFragment(InItemDefClass, UInventoryFragment_SetStats::StaticClass());
		const UInventoryFragment_SetStats* Fragment_SetStats = Cast<UInventoryFragment_SetStats>(ItemFragmeny);

		if (Fragment_SetStats != nullptr)
		{
			MaxStack = Fragment_SetStats->GetItemStatByTag(GyraGameplayTags::Gyra_ShooterGame_Inventory_MaxStackCount);
		}

	}

	return MaxStack;
}

const int32 UGyraInventoryFunctionLibrary::FindItemMaxStackByInstance(const UGyraInventoryItemInstance* InItemInstance)
{
	int32 MaxStack = 1;

	if (InItemInstance != nullptr)
	{
		MaxStack = FindItemMaxStackByDefintion(InItemInstance->GetItemDef());

	}

	return MaxStack;
}

const EGyraInventoryItemType UGyraInventoryFunctionLibrary::FindItemTypeByDefintion(const TSubclassOf<UGyraInventoryItemDefinition> InItemDefClass)
{
	EGyraInventoryItemType ItemType = EGyraInventoryItemType::None;

	if (InItemDefClass != nullptr)
	{
		UGyraInventoryItemDefinition* ItemDefCDO = Cast<UGyraInventoryItemDefinition>(InItemDefClass->GetDefaultObject());


		const UInventoryFragment_ItemInfo* Fragment_ItemInfo =
			Cast <UInventoryFragment_ItemInfo>(ItemDefCDO->FindFragmentByClass(UInventoryFragment_ItemInfo::StaticClass()));

		if (Fragment_ItemInfo)
		{
			ItemType = Fragment_ItemInfo->Type;
		}
	}


	return ItemType;
}
const EGyraInventoryItemType UGyraInventoryFunctionLibrary::FindItemTypeByInstance(const UGyraInventoryItemInstance* InItemInstance)
{
	EGyraInventoryItemType ItemType = EGyraInventoryItemType::None;

	if (InItemInstance != nullptr)
	{
		UGyraInventoryItemDefinition* ItemDefCDO = Cast<UGyraInventoryItemDefinition>(InItemInstance->GetItemDef()->GetDefaultObject());

		ItemType = FindItemTypeByDefintion(InItemInstance->GetItemDef());
	}


	return ItemType;
}
const EGyraEquipmentItemType UGyraInventoryFunctionLibrary::FindItemEquipmentTypeByDefintion(const TSubclassOf<UGyraInventoryItemDefinition> InItemDefClass)
{
	EGyraEquipmentItemType EquipmentType = EGyraEquipmentItemType::None;

	if (InItemDefClass != nullptr)
	{
		UGyraInventoryItemDefinition* ItemDefCDO = Cast<UGyraInventoryItemDefinition>(InItemDefClass->GetDefaultObject());


		const UInventoryFragment_EquippableItem* Fragment_EquippableItem =
			Cast <UInventoryFragment_EquippableItem>(ItemDefCDO->FindFragmentByClass(UInventoryFragment_EquippableItem::StaticClass()));

		if (Fragment_EquippableItem)
		{
			EquipmentType = Fragment_EquippableItem->EquipmentType;
		}
	}

	return EquipmentType;
}



const EGyraEquipmentItemType UGyraInventoryFunctionLibrary::FindItemEquipmentTypeByInstance(const UGyraInventoryItemInstance* InItemInstance)
{
	
	EGyraEquipmentItemType EquipmentType = EGyraEquipmentItemType::None;

	if (InItemInstance != nullptr)
	{
		UGyraInventoryItemDefinition* ItemDefCDO = Cast<UGyraInventoryItemDefinition>(InItemInstance->GetItemDef()->GetDefaultObject());

		EquipmentType = FindItemEquipmentTypeByDefintion(InItemInstance->GetItemDef());
	}


	return EquipmentType;
}

const EGyraInventoryItemQuality UGyraInventoryFunctionLibrary::FindItemQualityByDefintion(const TSubclassOf<UGyraInventoryItemDefinition> InItemDefClass)
{
	EGyraInventoryItemQuality ItemQuality = EGyraInventoryItemQuality::Ordinary;

	if (InItemDefClass != nullptr)
	{
		UGyraInventoryItemDefinition* ItemDefCDO = Cast<UGyraInventoryItemDefinition>(InItemDefClass->GetDefaultObject());


		const UInventoryFragment_ItemInfo* Fragment_ItemInfo =
			Cast <UInventoryFragment_ItemInfo>(ItemDefCDO->FindFragmentByClass(UInventoryFragment_ItemInfo::StaticClass()));

		if (Fragment_ItemInfo)
		{
			ItemQuality = Fragment_ItemInfo->Quality;
		}
	}


	return ItemQuality;
}

const EGyraInventoryItemQuality UGyraInventoryFunctionLibrary::FindItemQualityByInstance(const UGyraInventoryItemInstance* InItemInstance)
{
	EGyraInventoryItemQuality ItemQuality = EGyraInventoryItemQuality::Ordinary;

	if (InItemInstance != nullptr)
	{
		UGyraInventoryItemDefinition* ItemDefCDO = Cast<UGyraInventoryItemDefinition>(InItemInstance->GetItemDef()->GetDefaultObject());

		ItemQuality = FindItemQualityByDefintion(InItemInstance->GetItemDef());
	}


	return ItemQuality;
}

const FLinearColor UGyraInventoryFunctionLibrary::FindItemQualityColorByDefintion(const TSubclassOf<UGyraInventoryItemDefinition> InItemDefClass)
{
	EGyraInventoryItemQuality ItemQuality = FindItemQualityByDefintion(InItemDefClass);

	return UGyraGameSettings::GetGyraGameSettings()->GetItemQualityColor(ItemQuality);
}

const FLinearColor UGyraInventoryFunctionLibrary::FindItemQualityColorByInstance(const UGyraInventoryItemInstance* InItemInstance)
{
	EGyraInventoryItemQuality ItemQuality = FindItemQualityByInstance(InItemInstance);

	return UGyraGameSettings::GetGyraGameSettings()->GetItemQualityColor(ItemQuality);

}

UStaticMesh* UGyraInventoryFunctionLibrary::FindItemMeshByDefinition(const TSubclassOf<UGyraInventoryItemDefinition> InItemDefClass)
{

	if (InItemDefClass != nullptr)
	{
		UGyraInventoryItemDefinition* ItemDefCDO = Cast<UGyraInventoryItemDefinition>(InItemDefClass->GetDefaultObject());


		const UInventoryFragment_ItemInfo* Fragment_ItemInfo =
			Cast <UInventoryFragment_ItemInfo>(ItemDefCDO->FindFragmentByClass(UInventoryFragment_ItemInfo::StaticClass()));

		if (Fragment_ItemInfo)
		{
			return  Fragment_ItemInfo->Mesh;
		}
	}

	return nullptr;
}

AGyraWorldCollectable* UGyraInventoryFunctionLibrary::SpawnDropItemInstance(UObject* WorldContextObject,
AActor* InOwner,
FVector InSpawnLocation,
UGyraInventoryItemInstance* InItemInstance, 
int32 InStackNum)
{	
	check(WorldContextObject);

	UWorld* ThisWorld = WorldContextObject->GetWorld();

	if (ThisWorld)
	{
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride= ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		SpawnParameters.Owner = InOwner;
		TSubclassOf<AGyraWorldCollectable> CollectableClass = UGyraGameSettings::GetGyraGameSettings()->GetWorldCollectableBase();
		AGyraWorldCollectable* CollectableItem =	 ThisWorld->SpawnActor<AGyraWorldCollectable>(CollectableClass, InSpawnLocation, FRotator::ZeroRotator, SpawnParameters);
		check(CollectableItem);
		
		FInventoryPickup InventoryPickup;

		FPickupInstance PickupInstance;
		PickupInstance.Item = InItemInstance;
		PickupInstance.StackCount = InStackNum;

		InventoryPickup.Instances.Add(PickupInstance);

		CollectableItem->SetStaticInventory(InventoryPickup);

		return  CollectableItem;
	}

	return nullptr;
}

