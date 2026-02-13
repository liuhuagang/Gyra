// Copyright 2024 Huagang Liu. All Rights Reserved.

#include "InventoryFragment_SetStats.h"

#include "GyraInventoryItemInstance.h"
#include "GyraGameplayTags.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(InventoryFragment_SetStats)

UInventoryFragment_SetStats::UInventoryFragment_SetStats()
{
	//在这里,我们给一个默认的最大堆叠数量为1.
	InitialItemStats.Add(GyraGameplayTags::Gyra_ShooterGame_Inventory_MaxStackCount,1);

}

void UInventoryFragment_SetStats::OnInstanceCreated(UGyraInventoryItemInstance* Instance) const
{
	for (const auto& KVP : InitialItemStats)
	{
		Instance->AddStatTagStack(KVP.Key, KVP.Value);
	}

}

int32 UInventoryFragment_SetStats::GetItemStatByTag(FGameplayTag Tag) const
{
	if (const int32* StatPtr = InitialItemStats.Find(Tag))
	{
		return *StatPtr;
	}

	return 0;
}
