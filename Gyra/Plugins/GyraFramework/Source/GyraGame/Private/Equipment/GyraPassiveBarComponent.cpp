// Copyright 2024 Huagang Liu. All Rights Reserved.

#include "GyraPassiveBarComponent.h"

#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameFramework/Pawn.h"
#include "NativeGameplayTags.h"
#include "Net/UnrealNetwork.h"

#include "Equipment/GyraEquipmentDefinition.h"
#include "Equipment/GyraEquipmentInstance.h"
#include "Equipment/GyraEquipmentManagerComponent.h"

#include "InventoryFragment_EquippableItem.h"


#include "GyraInventoryFunctionLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GyraPassiveBarComponent)

class FLifetimeProperty;
class UGyraEquipmentDefinition;

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Gyra_PassiveBar_Message_SlotsChanged, "Gyra.PassiveBar.Message.SlotsChanged");

UGyraPassiveBarComponent::UGyraPassiveBarComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(true);
}

void UGyraPassiveBarComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, Slots);
}

void UGyraPassiveBarComponent::AddItemToSlot(int32 SlotIndex, UGyraInventoryItemInstance* Item)
{
	EGyraInventoryItemType ItemType = UGyraInventoryFunctionLibrary::FindItemTypeByInstance(Item);
	EGyraEquipmentItemType EquipmentType = UGyraInventoryFunctionLibrary::FindItemEquipmentTypeByInstance(Item);


	check(ItemType == EGyraInventoryItemType::Equipment);
	check(EquipmentType != EGyraEquipmentItemType::None);

	if (Slots.IsValidIndex(SlotIndex) && (Item != nullptr))
	{
		if (Slots[SlotIndex] == nullptr)
		{
			Slots[SlotIndex] = Item;
			EquipItemInSlot(Item);
			
			OnRep_Slots();
		}
	}

}
void UGyraPassiveBarComponent::CallServerAddItemToSlot_Implementation(int32 SlotIndex, UGyraInventoryItemInstance* Item)
{
	AddItemToSlot(SlotIndex, Item);
}


UGyraInventoryItemInstance* UGyraPassiveBarComponent::RemoveItemFromSlot(int32 SlotIndex)
{
	UGyraInventoryItemInstance* Result = nullptr;

	if (Slots.IsValidIndex(SlotIndex))
	{
		Result = Slots[SlotIndex];

		if (Result != nullptr)
		{
			//先脱下装备
			if (EquippedItems[SlotIndex] != nullptr)
			{
				UnequipItemInSlot(Result);
			}
			//重置该栏位
			Slots[SlotIndex] = nullptr;

			OnRep_Slots();
		}
	}

	return Result;
}



UGyraInventoryItemInstance* UGyraPassiveBarComponent::RemoveItemByInstance(UGyraInventoryItemInstance* InInstance)
{
	int32 RemovedIndex = FindSlotIndexByItemInstance(InInstance);

	return RemoveItemFromSlot(RemovedIndex);
}

void UGyraPassiveBarComponent::CallServerRemoveItemByInstance_Implementation(UGyraInventoryItemInstance* InInstance)
{
	RemoveItemByInstance(InInstance);
}

int32 UGyraPassiveBarComponent::FindSlotIndexByItemInstance(UGyraInventoryItemInstance* InInstance)
{
	return Slots.Find(InInstance);
}

void UGyraPassiveBarComponent::BeginPlay()
{
	if (Slots.Num() < NumSlots)
	{
		Slots.AddDefaulted(NumSlots - Slots.Num());
	}

	if (EquippedItems.Num() < NumSlots)
	{
		EquippedItems.AddDefaulted(NumSlots - EquippedItems.Num());
	}

	Super::BeginPlay();
}

void UGyraPassiveBarComponent::EquipItemInSlot(UGyraInventoryItemInstance* InInstance)
{

	int32 SlotIndex = FindSlotIndexByItemInstance(InInstance);

	check(Slots.IsValidIndex(SlotIndex));

	check(EquippedItems[SlotIndex] == nullptr);

	TObjectPtr<UGyraEquipmentInstance>& EquippedItemRef = EquippedItems[SlotIndex];

	//装备该项物品
	if (UGyraInventoryItemInstance* SlotItem = Slots[SlotIndex])
	{
		if (const UInventoryFragment_EquippableItem* EquipInfo = SlotItem->FindFragmentByClass<UInventoryFragment_EquippableItem>())
		{
			TSubclassOf<UGyraEquipmentDefinition> EquipDef = EquipInfo->EquipmentDefinition;
			if (EquipDef != nullptr)
			{
				if (UGyraEquipmentManagerComponent* EquipmentManager = FindEquipmentManager())
				{
					//这里出入的该物品的所有属性Tag
					EquippedItemRef = EquipmentManager->EquipItem(EquipDef, SlotItem->GetAllStatsTag());

					if (EquippedItemRef != nullptr)
					{
						EquippedItemRef->SetInstigator(SlotItem);
					}
				}
			}
		}
	}
}

void UGyraPassiveBarComponent::UnequipItemInSlot(UGyraInventoryItemInstance* InInstance)
{
	int32 SlotIndex = FindSlotIndexByItemInstance(InInstance);

	check(Slots.IsValidIndex(SlotIndex));

	check(EquippedItems[SlotIndex] != nullptr);

	TObjectPtr<UGyraEquipmentInstance>& EquippedItemRef = EquippedItems[SlotIndex];

	if (UGyraEquipmentManagerComponent* EquipmentManager = FindEquipmentManager())
	{
		if (EquippedItemRef != nullptr)
		{
			EquipmentManager->UnequipItem(EquippedItemRef);
			EquippedItemRef = nullptr;
		}
	}
}

UGyraEquipmentManagerComponent* UGyraPassiveBarComponent::FindEquipmentManager() const
{
	if (AController* OwnerController = Cast<AController>(GetOwner()))
	{
		if (APawn* Pawn = OwnerController->GetPawn())
		{
			return Pawn->FindComponentByClass<UGyraEquipmentManagerComponent>();
		}
	}
	return nullptr;
}

void UGyraPassiveBarComponent::OnRep_Slots()
{
	FGyraPassiveBarSlotsChangedMessage Message;
	Message.Owner = GetOwner();
	Message.Slots = Slots;

	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(this);
	MessageSystem.BroadcastMessage(TAG_Gyra_PassiveBar_Message_SlotsChanged, Message);

}