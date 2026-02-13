// Copyright 2024 Huagang Liu. All Rights Reserved.

#include "InventoryFragment_RandomStats.h"
#include "GyraGameSettings.h"
#include "GyraInventoryItemInstance.h"
#include "GyraGameplayTags.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(InventoryFragment_RandomStats)

UInventoryFragment_RandomStats::UInventoryFragment_RandomStats()
{
	RandomTags =UGyraGameSettings::GetGyraGameSettings()->GetRandomTagDefaultList();
}

void UInventoryFragment_RandomStats::OnInstanceCreated(UGyraInventoryItemInstance* Instance) const
{
	TArray<FGyraItemInstanceTagDefiniton> LeftRandomTags;

	for (auto& Tmp: RandomTags)
	{
		if (Tmp.bEnableInRandom)
		{
			LeftRandomTags.Add(Tmp);
		}
	}
	int32 LeftRandomNum = RandomNum;
	
	InteralRandom(Instance, LeftRandomTags, LeftRandomNum);

}

bool UInventoryFragment_RandomStats::GetItemStatByTag(FGameplayTag Tag, FGyraItemInstanceTagDefiniton& OutItemInstanceTagDefiniton) const
{

	const FGyraItemInstanceTagDefiniton* FindTagfDef=RandomTags.FindByKey(Tag);

	if (FindTagfDef!=nullptr)
	{
		OutItemInstanceTagDefiniton = *FindTagfDef;

		return true;
	}

	return false;
}

void UInventoryFragment_RandomStats::InteralRandom(UGyraInventoryItemInstance* Instance, TArray<FGyraItemInstanceTagDefiniton>& LeftRandomTags, int32& LeftRandomNum) const
{
	if (LeftRandomTags.Num()==0|| LeftRandomNum==0)
	{
		return;
	}
	FGameplayTag OutTag;
	int32 OutValue = 1;
	int32 TagDefIndex =0;

	if (LeftRandomTags.Num()!=1)
	{
		TagDefIndex = FMath::RandRange(0, LeftRandomTags.Num() - 1);
	}


	GetOnceRandomTag(LeftRandomTags[TagDefIndex], OutTag, OutValue);

	Instance->AddStatTagStack(OutTag, OutValue);

	LeftRandomTags.RemoveAt(TagDefIndex);
	LeftRandomNum--;
	InteralRandom(Instance, LeftRandomTags, LeftRandomNum);
	return;

}

void UInventoryFragment_RandomStats::GetOnceRandomTag(const FGyraItemInstanceTagDefiniton& InTagDefintion, FGameplayTag& OutTag, int32& OutValue) const
{
	if (bForceTagFullValue)
	{
		OutTag = InTagDefintion.SetbyCallerTag;
		OutValue = InTagDefintion.Max;

	}
	else
	{
		OutTag = InTagDefintion.SetbyCallerTag;
		OutValue = FMath::RandRange(InTagDefintion.Min, InTagDefintion.Max);
	}

}

