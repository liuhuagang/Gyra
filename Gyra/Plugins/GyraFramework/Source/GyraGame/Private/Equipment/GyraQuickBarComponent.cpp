// Copyright 2024 Huagang Liu. All Rights Reserved.

#include "GyraQuickBarComponent.h"

#include "Equipment/GyraEquipmentDefinition.h"
#include "Equipment/GyraEquipmentInstance.h"
#include "Equipment/GyraEquipmentManagerComponent.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameFramework/Pawn.h"
#include "InventoryFragment_EquippableItem.h"
#include "NativeGameplayTags.h"
#include "Net/UnrealNetwork.h"

#include "GyraInventoryFunctionLibrary.h"
#include "GyraPassiveBarComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GyraQuickBarComponent)

class FLifetimeProperty;
class UGyraEquipmentDefinition;

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Gyra_QuickBar_Message_SlotsChanged, "Gyra.QuickBar.Message.SlotsChanged");
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Gyra_QuickBar_Message_ActiveIndexChanged, "Gyra.QuickBar.Message.ActiveIndexChanged");

UGyraQuickBarComponent::UGyraQuickBarComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(true);
}

void UGyraQuickBarComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, Slots);
	DOREPLIFETIME(ThisClass, ActiveSlotIndex);
}



void UGyraQuickBarComponent::CycleActiveSlotForward()
{
	if (Slots.Num() < 2)
	{
		return;
	}

	const int32 OldIndex = (ActiveSlotIndex < 0 ? Slots.Num()-1 : ActiveSlotIndex);
	int32 NewIndex = ActiveSlotIndex;
	do
	{
		NewIndex = (NewIndex + 1) % Slots.Num();
		if (Slots[NewIndex] != nullptr)
		{
			SetActiveSlotIndex(NewIndex);
			return;
		}
	} while (NewIndex != OldIndex);
}

void UGyraQuickBarComponent::CycleActiveSlotBackward()
{
	if (Slots.Num() < 2)
	{
		return;
	}

	const int32 OldIndex = (ActiveSlotIndex < 0 ? Slots.Num()-1 : ActiveSlotIndex);
	int32 NewIndex = ActiveSlotIndex;
	do
	{
		NewIndex = (NewIndex - 1 + Slots.Num()) % Slots.Num();
		if (Slots[NewIndex] != nullptr)
		{
			SetActiveSlotIndex(NewIndex);
			return;
		}
	} while (NewIndex != OldIndex);
}

void UGyraQuickBarComponent::CycleActiveFromIndexZero()
{
	if (Slots.Num() < 0)
	{
		return;
	}

	for (int32 Index = 0; Index < Slots.Num(); Index++)
	{
		if (Slots[Index]!=nullptr)
		{
			SetActiveSlotIndex(Index);
			break;
		}
	}
}
void UGyraQuickBarComponent::CallServerCycleActiveFromIndexZero_Implementation()
{
	CycleActiveFromIndexZero();
}


void UGyraQuickBarComponent::SetActiveSlotIndex_Implementation(int32 NewIndex)
{
	if (!Slots.IsValidIndex(NewIndex))
	{
		return;
	}
	
	bool bNewActive = (ActiveSlotIndex != NewIndex);

	if (bNewActive)
	{
		UnequipItemInSlot();

		ActiveSlotIndex = NewIndex;

		EquipItemInSlot();

		OnRep_Slots();

		OnRep_ActiveSlotIndex();
	}
}

UGyraInventoryItemInstance* UGyraQuickBarComponent::GetActiveSlotItem() const
{
	return Slots.IsValidIndex(ActiveSlotIndex) ? Slots[ActiveSlotIndex] : nullptr;
}

int32 UGyraQuickBarComponent::GetNextFreeItemSlot() const
{
	int32 SlotIndex = 0;
	for (const TObjectPtr<UGyraInventoryItemInstance>& ItemPtr : Slots)
	{
		if (ItemPtr == nullptr)
		{
			return SlotIndex;
		}
		++SlotIndex;
	}

	return INDEX_NONE;
}

void UGyraQuickBarComponent::AddItemToSlot(int32 SlotIndex, UGyraInventoryItemInstance* Item)
{
	EGyraInventoryItemType ItemType =  UGyraInventoryFunctionLibrary::FindItemTypeByInstance(Item);

	check(ItemType== EGyraInventoryItemType::Equipment);

	if (Slots.IsValidIndex(SlotIndex) && (Item != nullptr))
	{
		if (Slots[SlotIndex] == nullptr)
		{
			Slots[SlotIndex] = Item;
			OnRep_Slots();
		}
	}


}

void UGyraQuickBarComponent::CallServerAddItemToSlot_Implementation(int32 SlotIndex, UGyraInventoryItemInstance* Item)
{
	AddItemToSlot(SlotIndex, Item);
}



UGyraInventoryItemInstance* UGyraQuickBarComponent::RemoveItemFromSlot(int32 SlotIndex)
{
	UGyraInventoryItemInstance* Result = nullptr;

	if (ActiveSlotIndex == SlotIndex)
	{
		UnequipItemInSlot();
		ActiveSlotIndex = -1;
	}

	if (Slots.IsValidIndex(SlotIndex))
	{
		Result = Slots[SlotIndex];

		if (Result != nullptr)
		{
			Slots[SlotIndex] = nullptr;
			OnRep_Slots();
		}
	}

	return Result;
}



UGyraInventoryItemInstance* UGyraQuickBarComponent::RemoveItemByInstance(UGyraInventoryItemInstance* InInstance)
{
	int32 RemovedIndex =FindSlotIndexByInstance(InInstance);

	return RemoveItemFromSlot(RemovedIndex);
}

void UGyraQuickBarComponent::CallServerRemoveItemByInstance_Implementation(UGyraInventoryItemInstance* InInstance)
{
	 RemoveItemByInstance(InInstance);

}


void UGyraQuickBarComponent::SwapItemByInstance(int32 IndexA, int32 IndexB)
{
	//这里需要考虑下数组索引的有效性

	if (Slots.IsValidIndex(IndexA)&& Slots.IsValidIndex(IndexB))
	{
		Slots.Swap(IndexA, IndexB);

		if (ActiveSlotIndex == IndexA)
		{
			ActiveSlotIndex= IndexB;
		}
		else if(ActiveSlotIndex== IndexB)
		{
			ActiveSlotIndex= IndexA;
		}
		OnRep_Slots();
		OnRep_ActiveSlotIndex();
	}


}

void UGyraQuickBarComponent::CallServerSwapItemByInstance_Implementation(int32 IndexA, int32 IndexB)
{
	SwapItemByInstance(IndexA, IndexB);

}







int32 UGyraQuickBarComponent::FindSlotIndexByInstance(UGyraInventoryItemInstance* InInstance)
{
	return Slots.Find(InInstance);
}

UGyraEquipmentInstance* UGyraQuickBarComponent::GetActiveEquipmentInstance()
{
	return EquippedItem;
}

void UGyraQuickBarComponent::BeginPlay()
{
	if (Slots.Num() < NumSlots)
	{
		Slots.AddDefaulted(NumSlots - Slots.Num());
	}

	Super::BeginPlay();
}
void UGyraQuickBarComponent::EquipItemInSlot()
{
	check(Slots.IsValidIndex(ActiveSlotIndex));
	check(EquippedItem == nullptr);

	//装备该项物品
	if (UGyraInventoryItemInstance* SlotItem = Slots[ActiveSlotIndex])
	{
		if (const UInventoryFragment_EquippableItem* EquipInfo = SlotItem->FindFragmentByClass<UInventoryFragment_EquippableItem>())
		{
			TSubclassOf<UGyraEquipmentDefinition> EquipDef = EquipInfo->EquipmentDefinition;
			if (EquipDef != nullptr)
			{
				if (UGyraEquipmentManagerComponent* EquipmentManager = FindEquipmentManager())
				{
					//这里出入的该物品的所有属性Tag
					EquippedItem = EquipmentManager->EquipItem(EquipDef, SlotItem->GetAllStatsTag());

					if (EquippedItem != nullptr)
					{
						EquippedItem->SetInstigator(SlotItem);
					}
				}
			}
		}
	}
}

void UGyraQuickBarComponent::UnequipItemInSlot()
{
	if (UGyraEquipmentManagerComponent* EquipmentManager = FindEquipmentManager())
	{
		if (EquippedItem != nullptr)
		{
			EquipmentManager->UnequipItem(EquippedItem);
			EquippedItem = nullptr;
		}
	}
}

UGyraEquipmentManagerComponent* UGyraQuickBarComponent::FindEquipmentManager() const
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
void UGyraQuickBarComponent::OnRep_Slots()
{
	FGyraQuickBarSlotsChangedMessage Message;
	Message.Owner = GetOwner();
	Message.Slots = Slots;

	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(this);
	MessageSystem.BroadcastMessage(TAG_Gyra_QuickBar_Message_SlotsChanged, Message);
}

void UGyraQuickBarComponent::OnRep_ActiveSlotIndex()
{
	FGyraQuickBarActiveIndexChangedMessage Message;
	Message.Owner = GetOwner();
	Message.ActiveIndex = ActiveSlotIndex;

	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(this);
	MessageSystem.BroadcastMessage(TAG_Gyra_QuickBar_Message_ActiveIndexChanged, Message);
}

