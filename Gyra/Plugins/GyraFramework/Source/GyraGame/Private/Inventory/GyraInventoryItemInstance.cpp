// Copyright 2024 Huagang Liu. All Rights Reserved.

#include "GyraInventoryItemInstance.h"

#include "Inventory/GyraInventoryItemDefinition.h"
#include "Net/UnrealNetwork.h"

#include "InventoryFragment_ItemInfo.h"
#include "GyraInventoryItemDefinition.h"
#include "GyraInventoryFunctionLibrary.h"

#if UE_WITH_IRIS
#include "Iris/ReplicationSystem/ReplicationFragmentUtil.h"
#endif // UE_WITH_IRIS

#include UE_INLINE_GENERATED_CPP_BY_NAME(GyraInventoryItemInstance)

class FLifetimeProperty;

UGyraInventoryItemInstance::UGyraInventoryItemInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UGyraInventoryItemInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, StatTags);
	DOREPLIFETIME(ThisClass, ItemDef);
}

#if UE_WITH_IRIS

void UGyraInventoryItemInstance::RegisterReplicationFragments(UE::Net::FFragmentRegistrationContext& Context, UE::Net::EFragmentRegistrationFlags RegistrationFlags)
{
	using namespace UE::Net;

	// Build descriptors and allocate PropertyReplicationFragments for this object
	FReplicationFragmentUtil::CreateAndRegisterFragmentsForObject(this, Context, RegistrationFlags);
}
#endif // UE_WITH_IRIS

void UGyraInventoryItemInstance::AddStatTagStack(FGameplayTag Tag, int32 StackCount)
{
	StatTags.AddStack(Tag, StackCount);
}

void UGyraInventoryItemInstance::RemoveStatTagStack(FGameplayTag Tag, int32 StackCount)
{
	StatTags.RemoveStack(Tag, StackCount);
}

int32 UGyraInventoryItemInstance::GetStatTagStackCount(FGameplayTag Tag) const
{
	return StatTags.GetStackCount(Tag);
}

bool UGyraInventoryItemInstance::HasStatTag(FGameplayTag Tag) const
{
	return StatTags.ContainsTag(Tag);
}

TMap<FGameplayTag, float> UGyraInventoryItemInstance::GetAllStatsTag() const
{
	TMap<FGameplayTag, float> OutTagMap;
	
	for ( const auto&Tmp: StatTags.GetTagToCountMap())
	{
	//因为要存档所以要全部显示
		//if (Tmp.Key.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("SetByCaller"))))
		{
			OutTagMap.Add(Tmp.Key, Tmp.Value);
		};

	}

	return OutTagMap;
}

void UGyraInventoryItemInstance::SetItemDef(TSubclassOf<UGyraInventoryItemDefinition> InDef)
{
	ItemDef = InDef;
}

const UGyraInventoryItemFragment* UGyraInventoryItemInstance::FindFragmentByClass(TSubclassOf<UGyraInventoryItemFragment> FragmentClass) const
{
	if ((ItemDef != nullptr) && (FragmentClass != nullptr))
	{
		return GetDefault<UGyraInventoryItemDefinition>(ItemDef)->FindFragmentByClass(FragmentClass);
	}

	return nullptr;
}

EGyraInventoryItemType UGyraInventoryItemInstance::GetItemTypeOnCreate() const
{

	return UGyraInventoryFunctionLibrary::FindItemTypeByInstance(this);
}

TArray<FGyraInventoryTagStack> UGyraInventoryItemInstance::GetArchiveTagArray()
{
	return StatTags.GetArchiveTagArray();
}
