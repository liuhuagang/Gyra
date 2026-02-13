// Copyright 2024 Huagang Liu. All Rights Reserved.

#include "IPickupable.h"

#include "GameFramework/Actor.h"
#include "GyraInventoryManagerComponent.h"
#include "UObject/ScriptInterface.h"
#include "GameplayTagContainer.h"
#include "GyraInventoryItemInstance.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(IPickupable)

class UActorComponent;

UPickupableStatics::UPickupableStatics()
	: Super(FObjectInitializer::Get())
{
}

TScriptInterface<IPickupable> UPickupableStatics::GetFirstPickupableFromActor(AActor* Actor)
{
	// If the actor is directly pickupable, return that.
	// 如果actor是可直接拾取的，返回它。
	TScriptInterface<IPickupable> PickupableActor(Actor);
	if (PickupableActor)
	{
		return PickupableActor;
	}

	// If the actor isn't pickupable, it might have a component that has a pickupable interface.
	// 如果actor不可拾取，它可能有一个具有可拾取接口的组件。
	TArray<UActorComponent*> PickupableComponents = Actor ? Actor->GetComponentsByInterface(UPickupable::StaticClass()) : TArray<UActorComponent*>();
	if (PickupableComponents.Num() > 0)
	{
		// Get first pickupable, if the user needs more sophisticated pickup distinction, will need to be solved elsewhere.
		// Get first pickup，如果用户需要更复杂的拾取区分，将需要在其他地方解决。
		return TScriptInterface<IPickupable>(PickupableComponents[0]);
	}

	return TScriptInterface<IPickupable>();
}

bool UPickupableStatics::AddPickupToInventory(UGyraInventoryManagerComponent* InventoryComponent, TScriptInterface<IPickupable> Pickup)
{
	bool bAdd =false;
		
	if (InventoryComponent && Pickup)
	{
		const FInventoryPickup& PickupInventory = Pickup->GetPickupInventory();

		int32 AddTypeNum = PickupInventory.Templates.Num()+ PickupInventory.Instances.Num() ;

		//合计只能一项
		check(AddTypeNum==1);

		for (const FPickupTemplate& Template : PickupInventory.Templates)
		{
	
			bAdd =InventoryComponent->OverAddItemDefinition(EGyraInventoryListType::Backpack,Template.ItemDef,Template.StackCount);
		}

		for (const FPickupInstance& Instance : PickupInventory.Instances)
		{

			bAdd =InventoryComponent->AddItemInstance(EGyraInventoryListType::Backpack, Instance.Item, Instance.StackCount);
		}
	}
	return bAdd;
}

bool UPickupableStatics::CheckAddPickupToInventory(UGyraInventoryManagerComponent* InventoryComponent, TScriptInterface<IPickupable> Pickup)
{

	bool bAdd = false;

	if (InventoryComponent && Pickup)
	{
		const FInventoryPickup& PickupInventory = Pickup->GetPickupInventory();

		int32 AddTypeNum = PickupInventory.Templates.Num() + PickupInventory.Instances.Num();

		//合计只能一项
		check(AddTypeNum == 1);

		for (const FPickupTemplate& Template : PickupInventory.Templates)
		{

			bAdd = InventoryComponent->CanAddItemDefinition(EGyraInventoryListType::Backpack, Template.ItemDef, Template.StackCount);
		}

		for (const FPickupInstance& Instance : PickupInventory.Instances)
		{

			bAdd = InventoryComponent->CanAddItemInstance(EGyraInventoryListType::Backpack, Instance.Item, Instance.StackCount);
		}
	}
	return bAdd;
}

