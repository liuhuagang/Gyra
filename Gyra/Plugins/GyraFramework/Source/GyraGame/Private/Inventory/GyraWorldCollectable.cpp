// Copyright Epic Games, Inc. All Rights Reserved.

#include "GyraWorldCollectable.h"

#include "Async/TaskGraphInterfaces.h"
#include "Net/UnrealNetwork.h"
#include "GyraInventoryItemInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GyraWorldCollectable)

struct FInteractionQuery;

AGyraWorldCollectable::AGyraWorldCollectable()
{
	bReplicates =true;
	bNetLoadOnClient =true;
}

void AGyraWorldCollectable::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGyraWorldCollectable, ReplicatedPickupInfo);
}

void AGyraWorldCollectable::GatherInteractionOptions(const FInteractionQuery& InteractQuery, FInteractionOptionBuilder& InteractionBuilder)
{
	InteractionBuilder.AddInteractionOption(Option);
}

FInventoryPickup AGyraWorldCollectable::GetPickupInventory() const
{
	return StaticInventory;
}

void AGyraWorldCollectable::CancelSpecialEffects()
{
	K2_CancelSpecialEffects();
}

void AGyraWorldCollectable::SetStaticInventory(FInventoryPickup InFInventoryPickup)
{
	StaticInventory = InFInventoryPickup;

	if (StaticInventory.Instances.Num()>0)
	{
		ReplicatedPickupInfo.ItemDef = StaticInventory.Instances[0].Item->GetItemDef();
		ReplicatedPickupInfo.StackCount=StaticInventory.Instances[0].StackCount;

	}

	if (StaticInventory.Templates.Num() > 0)
	{
		ReplicatedPickupInfo = StaticInventory.Templates[0];
	
	}
	OnRep_PickupInfo();
}

void AGyraWorldCollectable::OnRep_PickupInfo()
{
	K2_InitFromItemDefination(ReplicatedPickupInfo);
}
