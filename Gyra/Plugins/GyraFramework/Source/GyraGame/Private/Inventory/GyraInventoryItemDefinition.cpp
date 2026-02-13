// Copyright 2024 Huagang Liu. All Rights Reserved.

#include "GyraInventoryItemDefinition.h"

#include "Templates/SubclassOf.h"
#include "UObject/ObjectPtr.h"



#include UE_INLINE_GENERATED_CPP_BY_NAME(GyraInventoryItemDefinition)

//////////////////////////////////////////////////////////////////////
// UGyraInventoryItemDefinition

UGyraInventoryItemDefinition::UGyraInventoryItemDefinition(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

const UGyraInventoryItemFragment* UGyraInventoryItemDefinition::FindFragmentByClass(TSubclassOf<UGyraInventoryItemFragment> FragmentClass) const
{
	if (FragmentClass != nullptr)
	{
		for (UGyraInventoryItemFragment* Fragment : Fragments)
		{
			if (Fragment && Fragment->IsA(FragmentClass))
			{
				return Fragment;
			}
		}
	}

	return nullptr;
}

//////////////////////////////////////////////////////////////////////
// UGyraInventoryItemDefinition
