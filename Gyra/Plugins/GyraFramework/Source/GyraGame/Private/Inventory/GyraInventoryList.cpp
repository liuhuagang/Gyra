// Copyright 2024 Huagang Liu. All Rights Reserved.

#include "GyraInventoryList.h"

#include "Engine/World.h"
#include "GameFramework/GameplayMessageSubsystem.h"


#include "NativeGameplayTags.h"

#include "GyraInventoryIndexInfo.h"
#include "GyraInventoryItemDefinition.h"
#include "GyraInventoryItemInstance.h"
#include "GyraInventoryManagerComponent.h"


#include "GyraInventoryFunctionLibrary.h"

#include "DataRegistrySubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GyraInventoryList)

class FLifetimeProperty;
struct FReplicationFlags;

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Gyra_Inventory_Message_StackChanged, "Gyra.Inventory.Message.StackChanged");
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Gyra_Inventory_Message_ItemChanged, "Gyra.Inventory.Message.ItemChanged");

//////////////////////////////////////////////////////////////////////
// FGyraInventoryEntry
static FName NAME_InventoryRegistryID("InventoryList");

FString FGyraInventoryEntry::GetDebugString() const
{
	TSubclassOf<UGyraInventoryItemDefinition> ItemDef;
	if (Instance != nullptr)
	{
		ItemDef = Instance->GetItemDef();
	}

	return FString::Printf(TEXT("%s (%d x %s)"), *GetNameSafe(Instance), StackCount, *GetNameSafe(ItemDef));
}

bool FGyraInventoryEntry::IsUsedEntry()
{
	if (Instance && StackCount >= 1)
	{
		return true;
	}
	return false;
}

bool FGyraInventoryEntry::IsFreeEntry()
{
	return !IsUsedEntry();
}

//////////////////////////////////////////////////////////////////////
// FGyraInventoryList

void FGyraInventoryList::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	//在移除之前LastObservedCount必不为0,移除之后必为0
	for (int32 Index : RemovedIndices)
	{
		FGyraInventoryEntry& Stack = Entries[Index];
		BroadcastChangeMessage(Stack, /*OldCount=*/ Stack.StackCount, /*NewCount=*/ 0);
		Stack.LastObservedCount = 0;
	}
}

void FGyraInventoryList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	//在添加之前LastObservedCount必为0,添加之后至少为1
	for (int32 Index : AddedIndices)
	{
		FGyraInventoryEntry& Stack = Entries[Index];
		BroadcastChangeMessage(Stack, /*OldCount=*/ 0, /*NewCount=*/ Stack.StackCount);
		Stack.LastObservedCount = Stack.StackCount;
	}
}

void FGyraInventoryList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	//在改变时,LastObservedCount必不为-1或0,改变之后至少为1
	for (int32 Index : ChangedIndices)
	{
		FGyraInventoryEntry& Stack = Entries[Index];
		check(Stack.LastObservedCount != INDEX_NONE);
		BroadcastChangeMessage(Stack, /*OldCount=*/ Stack.LastObservedCount, /*NewCount=*/ Stack.StackCount);
		Stack.LastObservedCount = Stack.StackCount;
	}
}


TArray<UGyraInventoryItemInstance*> FGyraInventoryList::InitEmptyList(int32 InListNum)
{
	check(OwnerComponent);

	AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor->HasAuthority());

	TArray<UGyraInventoryItemInstance*> Results;
	Results.Reserve(InListNum);

	for (size_t i = 0; i < InListNum; i++)
	{
		FGyraInventoryEntry& NewEntry = Entries.AddDefaulted_GetRef();

		NewEntry.StackCount = 0;
		NewEntry.Instance = ConstructNewFreeItemInstance();
		Results.Add(NewEntry.Instance);

		MarkItemDirty(NewEntry);
	}

	NotifyAddReplicatedInventoryItemInstance(Results);

	//此处需要广播,避免主机玩家没有更新
	BroadcastItemChangeMessage();

	return Results;
}

TArray<UGyraInventoryItemInstance*> FGyraInventoryList::InitArchiveList(
	const FGyraArchiveListInfomation& InListInformation, int32 InListNum)
{
	check(OwnerComponent);

	AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor->HasAuthority());

	TArray<UGyraInventoryItemInstance*> Results;
	Results.Reserve(InListNum);

	for (size_t i = 0; i < InListNum; i++)
	{
		FGyraInventoryEntry& NewEntry = Entries.AddDefaulted_GetRef();

		if (InListInformation.List.IsValidIndex(i))
		{
			FGyraArchiveInventoryItemInfomation InventoryItemInfomation = InListInformation.List[i];

			FDataRegistryId ItemID(NAME_InventoryRegistryID, InventoryItemInfomation.InventoryName);
			const FGyraInventoryItemIndexInfo* ItemIndexInfo = UDataRegistrySubsystem::Get()->GetCachedItem<
				FGyraInventoryItemIndexInfo>(ItemID);

			if (ItemIndexInfo)
			{
				NewEntry.StackCount = InventoryItemInfomation.InventoryStack;
				NewEntry.Instance = ConstructNewFreeItemInstance(ItemIndexInfo->InventoryItemDefinition);


				//必须添加词缀
				for (auto& Tmp : InventoryItemInfomation.Tags)
				{
					NewEntry.Instance->AddStatTagStack(Tmp.AttributeTag, Tmp.AttributeValue);
				}

				//放到蓝图去处理
				////临时处理 将武器添加到快捷操作
				//if (InListNum==3)
				//{
				//	UGyraQuickBarComponent* EMC =OwningActor->FindComponentByClass<UGyraQuickBarComponent>();

				//	if (EMC)
				//	{
				//		EMC->AddItemToSlot(i, NewEntry.Instance);
				//	}

				//}
				////临时处理 将装备添加到被动装备栏
				//if (InListNum == 6)
				//{
				//	UGyraPassiveBarComponent* EMC = OwningActor->FindComponentByClass<UGyraPassiveBarComponent>();

				//	if (EMC)
				//	{
				//		EMC->AddItemToSlot(i, NewEntry.Instance);
				//	}

				//}
			}
			else
			{
				NewEntry.StackCount = 0;
				NewEntry.Instance = ConstructNewFreeItemInstance();
			}
		}
		else
		{
			NewEntry.StackCount = 0;
			NewEntry.Instance = ConstructNewFreeItemInstance();
		}


		Results.Add(NewEntry.Instance);

		MarkItemDirty(NewEntry);
	}

	NotifyAddReplicatedInventoryItemInstance(Results);

	//此处需要广播,避免主机玩家没有更新
	BroadcastItemChangeMessage();

	return Results;
}

FGyraArchiveListInfomation FGyraInventoryList::ExportArchiveList()
{
	FGyraArchiveListInfomation ArchiveListInfomation;

	for (auto& TmpEntry : Entries)
	{
		FGyraArchiveInventoryItemInfomation& EmptyItemInfo = ArchiveListInfomation.List.AddDefaulted_GetRef();

		if (TmpEntry.StackCount > 0 && TmpEntry.Instance != nullptr)
		{
			FName ItemName = UGyraInventoryFunctionLibrary::FindItemDefinitionUnique(TmpEntry.Instance->GetItemDef());
			EmptyItemInfo.InventoryName = ItemName;
			EmptyItemInfo.InventoryStack = TmpEntry.StackCount;
			EmptyItemInfo.Tags = TmpEntry.Instance->GetArchiveTagArray();
		}
	}


	return ArchiveListInfomation;
}


UGyraInventoryItemInstance* FGyraInventoryList::AddEntry(TSubclassOf<UGyraInventoryItemDefinition> ItemDefClass,
                                                         bool& bSuccess,
                                                         int32 StackCount,
                                                         bool bAddToFree)
{
	check(StackCount==1);

	bSuccess = false;
	UGyraInventoryItemInstance* Result = nullptr;

	check(ItemDefClass != nullptr);
	check(OwnerComponent);

	AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor->HasAuthority());


	FGyraInventoryEntry* HandleEntry = nullptr;

	if (bAddToFree)
	{
		//只添加到空闲的槽位

		//寻找首个空闲的槽位
		int32 FirstFreeEntry = FindFirstFreeEntry();

		if (FirstFreeEntry == INDEX_NONE)
		{
			//如果没有找到空余的槽位,直接进行返回

			return nullptr;
		}

		HandleEntry = &Entries[FirstFreeEntry];

		//我需要释放原先的空闲Object
		NotifyRemoveReplicatedInventoryItemInstance(HandleEntry->Instance);
	}
	else
	{
		//直接创建新的槽位
		//不应当这样去创建新的槽位
		//此处代码仍保留,但不使用
		FGyraInventoryEntry& NewEntry = Entries.AddDefaulted_GetRef();
		HandleEntry = &NewEntry;
	}


	HandleEntry->Instance = ConstructNewFreeItemInstance(ItemDefClass);

	//此处需要激活物品实例自动初始化规则
	const UGyraInventoryItemDefinition* ItemDefinitionCDO = GetDefault<UGyraInventoryItemDefinition>(ItemDefClass);

	for (UGyraInventoryItemFragment* Fragment : ItemDefinitionCDO->Fragments)
	{
		if (Fragment != nullptr)
		{
			Fragment->OnInstanceCreated(HandleEntry->Instance);
		}
	}

	HandleEntry->StackCount = StackCount;

	bSuccess = true;
	Result = HandleEntry->Instance;

	MarkItemDirty(*HandleEntry);


	NotifyAddReplicatedInventoryItemInstance(Result);

	//此处需要广播,避免主机玩家没有更新
	BroadcastChangeMessage(*HandleEntry, 0, HandleEntry->StackCount);

	return Result;
}


bool FGyraInventoryList::OverAddEntry(TSubclassOf<UGyraInventoryItemDefinition> ItemDefClass, int32 StackCount)
{
	//记录需要新增的Object
	TArray<UGyraInventoryItemInstance*> Results;

	check(ItemDefClass != nullptr);
	check(OwnerComponent);

	AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor->HasAuthority());

	bool bCheck = CheckOverAddEntry(ItemDefClass, StackCount);

	if (!bCheck)
	{
		return false;
	}

	int32 LeftToAddCount = StackCount;
	int32 MaxStatck = UGyraInventoryFunctionLibrary::FindItemMaxStackByDefintion(ItemDefClass);

	//创建迭代器循环遍历添加至已有的格子
	for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
	{
		if (LeftToAddCount == 0)
		{
			break;
		}

		FGyraInventoryEntry& Entry = *EntryIt;
		UGyraInventoryItemInstance* Instance = Entry.Instance;

		if (Instance->GetItemDef() == ItemDefClass)
		{
			int32 CurrentStatckCount = GetItemCountByInstance(Instance);


			check(MaxStatck >= 1);

			//如果现有数量超过或等于最大数量,则循环下一个
			if (CurrentStatckCount >= MaxStatck)
			{
				continue;
			}
			if (MaxStatck - CurrentStatckCount >= LeftToAddCount)
			{
				//当前组可以直接容纳所有添加数量

				Entry.StackCount += LeftToAddCount;
				LeftToAddCount = 0;

				MarkItemDirty(Entry);
				break;
			}
			else if (MaxStatck - CurrentStatckCount < LeftToAddCount)
			{
				//当前组可以容纳部分数量

				check(MaxStatck - CurrentStatckCount >= 0);

				Entry.StackCount = MaxStatck;

				LeftToAddCount = LeftToAddCount - (MaxStatck - CurrentStatckCount);

				MarkItemDirty(Entry);
			}
		}
	}

	//去创建新的占用格子
	while (LeftToAddCount > 0)
	{
		int32 FreeEntryIndex = FindFirstFreeEntry();

		UGyraInventoryItemInstance* NewInstance = ConstructNewFreeItemInstance();
		Results.Add(NewInstance);
		//完成数量的扣减
		int32 NewStackCount = LeftToAddCount < MaxStatck ? LeftToAddCount : MaxStatck;
		LeftToAddCount = LeftToAddCount - MaxStatck;


		//完成对象的初始化
		NewInstance->SetItemDef(ItemDefClass);
		const UGyraInventoryItemDefinition* ItemDefinitionCDO = GetDefault<UGyraInventoryItemDefinition>(ItemDefClass);

		for (UGyraInventoryItemFragment* Fragment : ItemDefinitionCDO->Fragments)
		{
			if (Fragment != nullptr)
			{
				Fragment->OnInstanceCreated(NewInstance);
			}
		}
		ReplaceEntry(FreeEntryIndex, NewInstance, NewStackCount);
	}

	// 如果有新创建的占用,此处需要通知新增UObject需要同步
	// 不需要了因为用了Replace这个函数,它已经通知了.
	// NotifyAddReplicatedInventoryItemInstance(Results);

	//此处需要广播,避免主机玩家没有更新
	BroadcastItemChangeMessage();

	return true;
}

bool FGyraInventoryList::CheckOverAddEntry(TSubclassOf<UGyraInventoryItemDefinition> ItemDefClass, int32 StackCount)
{
	check(ItemDefClass != nullptr);
	check(OwnerComponent);

	AActor* OwningActor = OwnerComponent->GetOwner();
	//check(OwningActor->HasAuthority());

	//该物品的最大堆叠数量
	int32 MaxStack = UGyraInventoryFunctionLibrary::FindItemMaxStackByDefintion(ItemDefClass);

	// 1.如果只添加一个且有空余位置,那么直接允许添加
	// 至于是添加到一个新的占用,还是在老的占用里面,我们在这里并不关心
	if (StackCount == 1 && FindFirstFreeEntry() != INDEX_NONE)
	{
		return true;
	}

	//2.现在考虑是否可以直接添加到已有的占用
	int32 LeftToAddCount = StackCount;

	for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
	{
		//如果已经添加完毕,则退出循环
		if (LeftToAddCount == 0)
		{
			break;
		}

		//拿到该物品实例
		FGyraInventoryEntry& Entry = *EntryIt;
		UGyraInventoryItemInstance* Instance = Entry.Instance;

		if (Instance->GetItemDef() == ItemDefClass)
		{
			//拿到该物品实例的现有数量
			int32 CurrentStatckCount = GetItemCountByInstance(Instance);

			//如果现有数量超过或等于最大数量,则循环下一个
			if (CurrentStatckCount >= MaxStack)
			{
				continue;
			}


			if (MaxStack - CurrentStatckCount >= LeftToAddCount)
			{
				//当前组可以直接容纳所有添加数量
				LeftToAddCount = 0;
				break;
			}
			else if (MaxStack - CurrentStatckCount < LeftToAddCount)
			{
				//当前组可以容纳部分数量
				check(MaxStack - CurrentStatckCount >= 0);
				LeftToAddCount = LeftToAddCount - (MaxStack - CurrentStatckCount);
			}
		}
	}

	//此处证明可以全部容纳于现有的格子
	if (LeftToAddCount == 0)
	{
		return true;
	}


	// 3.如果无法添加已有的占用,那么考虑创建新的占用,新的占用可以是1个或者更多个,对比新的占用是否超过的空余个数.
	// 计算剩余的数量需要创建几个格子
	// 整数相除向上取整
	int32 NeedCount = (LeftToAddCount + MaxStack) / MaxStack;

	if (NeedCount <= GetFreeCapacityNum())
	{
		return true;
	}

	return false;
}

bool FGyraInventoryList::CheckAddEntry(UGyraInventoryItemInstance* InInstance, int32 InStackCount)
{
	check(InInstance != nullptr);
	check(OwnerComponent);
	AActor* OwningActor = OwnerComponent->GetOwner();
	//check(OwningActor->HasAuthority());

	return FindFirstFreeEntry() != INDEX_NONE;
}

bool FGyraInventoryList::AddEntry(UGyraInventoryItemInstance* InInstance, int32 InStackCount)
{
	check(InInstance != nullptr);
	check(OwnerComponent);

	AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor->HasAuthority());


	check(InStackCount > 0);

	bool bCheck = CheckAddEntry(InInstance, InStackCount);

	if (!bCheck)
	{
		return false;
	}

	int32 FreeIndex = FindFirstFreeEntry();

	ReplaceEntry(FreeIndex, InInstance, InStackCount);

	return true;
}

TArray<UGyraInventoryItemInstance*> FGyraInventoryList::GetAllItems() const
{
	TArray<UGyraInventoryItemInstance*> Results;
	Results.Reserve(Entries.Num());

	for (const FGyraInventoryEntry& Entry : Entries)
	{
		if (Entry.Instance != nullptr) //@TODO: Would prefer to not deal with this here and hide it further?
		//不想在这里处理这件事，把它藏得更深？
		{
			Results.Add(Entry.Instance);
		}
	}
	return Results;
}


int32 FGyraInventoryList::GetItemCountByInstance(UGyraInventoryItemInstance* ItemInstance)
{
	int32 CurrentCount = 0;

	for (const FGyraInventoryEntry& Entry : Entries)
	{
		UGyraInventoryItemInstance* Instance = Entry.Instance;

		if (IsValid(Instance) && Instance == ItemInstance)
		{
			CurrentCount = Entry.StackCount;
			return CurrentCount;
		}
	}
	check(false);

	return CurrentCount;
}

int32 FGyraInventoryList::FindIndexByInstance(UGyraInventoryItemInstance* ItemInstance)
{
	int32 ItemIndex = -1;


	for (size_t Index = 0; Index < Entries.Num(); Index++)
	{
		UGyraInventoryItemInstance* Instance = Entries[Index].Instance;

		if (IsValid(Instance) && Instance == ItemInstance)
		{
			ItemIndex = Index;
			break;
		}
	}

	return ItemIndex;
}

UGyraInventoryItemInstance* FGyraInventoryList::FindIndexByInstance(int32 InItemIndex)
{
	if (Entries.IsValidIndex(InItemIndex))
	{
		return Entries[InItemIndex].Instance;
	}
	return nullptr;
}

int32 FGyraInventoryList::FindFirstFreeEntry()
{
	int32 FirstFreeEntryIndex = INDEX_NONE;

	for (int32 Index = 0; Index < Entries.Num(); Index++)
	{
		if (Entries[Index].IsFreeEntry())
		{
			FirstFreeEntryIndex = Index;
			return FirstFreeEntryIndex;
		}
	}

	return INDEX_NONE;
}

int32 FGyraInventoryList::GetFreeCapacityNum()
{
	return GetMaxCapacityNum() - GetUsedCapacityNum();
}

int32 FGyraInventoryList::GetUsedCapacityNum()
{
	int32 UsedCapacityNum = 0;

	for (auto& TmpEntry : Entries)
	{
		if (TmpEntry.IsUsedEntry())
		{
			UsedCapacityNum++;
		}
	}

	check(UsedCapacityNum <= GetMaxCapacityNum());

	return UsedCapacityNum;
}

int32 FGyraInventoryList::GetMaxCapacityNum()
{
	return Entries.Num();
}


bool FGyraInventoryList::CheckRemoveEntry(TSubclassOf<UGyraInventoryItemDefinition> ItemClass, int32 StackCount)
{
	bool bCheckRemove = false;


	int32 LeftToRemoveCount = StackCount;

	for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
	{
		if (LeftToRemoveCount == 0)
		{
			break;
		}

		FGyraInventoryEntry& Entry = *EntryIt;

		UGyraInventoryItemInstance* Instance = Entry.Instance;

		if (Instance->GetItemDef() == ItemClass)
		{
			//如果数量超过等待移除数量,则直接修改数量即可
			if (Entry.StackCount > LeftToRemoveCount)
			{
				LeftToRemoveCount = 0;
				break;
			}

			//如果现有数量等于移除数量,则需要移除自己,
			//这里的逻辑需要完善
			//需要检测是否重置为零
			if (Entry.StackCount == LeftToRemoveCount)
			{
				LeftToRemoveCount = 0;
				break;
			}

			//如果现有数量小于移除数量,则需要移除自己,并更新剩下的移除数量
			//这里的逻辑需要完善
			if (Entry.StackCount < LeftToRemoveCount)
			{
				LeftToRemoveCount = LeftToRemoveCount - Entry.StackCount;
			}
		}
	}

	if (LeftToRemoveCount == 0)
	{
		bCheckRemove = true;
	}

	return bCheckRemove;
}

bool FGyraInventoryList::RemoveEntry(TSubclassOf<UGyraInventoryItemDefinition> ItemClass,
                                     int32 StackCount,
                                     TArray<UGyraInventoryItemInstance*>& OutRemovedResults,
                                     bool bRemovedToFree
)
{
	bool bRemoved = false;
	OutRemovedResults.Empty();

	//判定是否可以移除
	if (!CheckRemoveEntry(ItemClass, StackCount))
	{
		return bRemoved;
	}

	int32 LeftToRemoveCount = StackCount;
	for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
	{
		if (LeftToRemoveCount == 0)
		{
			break;
		}

		FGyraInventoryEntry& Entry = *EntryIt;

		UGyraInventoryItemInstance* Instance = Entry.Instance;

		if (Instance->GetItemDef() == ItemClass)
		{
			//如果数量超过等待移除数量,则直接修改数量即可
			if (Entry.StackCount > LeftToRemoveCount)
			{
				Entry.StackCount = Entry.StackCount - LeftToRemoveCount;
				LeftToRemoveCount = 0;
				MarkItemDirty(Entry);
				break;
			}

			//如果现有数量等于移除数量,则需要移除自己,
			//这里的逻辑需要完善
			//需要检测是否重置为零
			if (Entry.StackCount == LeftToRemoveCount)
			{
				if (bRemovedToFree)
				{
					ReplaceEntryWithFree(EntryIt.GetIndex());
				}
				else
				{
					Entry.StackCount = 0;
					LeftToRemoveCount = 0;

					//告诉组件,该对象不需要了
					OutRemovedResults.Add(Instance);

					EntryIt.RemoveCurrent();
					MarkArrayDirty();
				}
				break;
			}

			//如果现有数量小于移除数量,则需要移除自己,并更新剩下的移除数量
			//这里的逻辑需要完善
			if (Entry.StackCount < LeftToRemoveCount)
			{
				if (bRemovedToFree)
				{
					ReplaceEntryWithFree(EntryIt.GetIndex());
				}
				else
				{
					LeftToRemoveCount = LeftToRemoveCount - Entry.StackCount;
					Entry.StackCount = 0;

					//告诉组件,该对象不需要了
					OutRemovedResults.Add(Instance);

					EntryIt.RemoveCurrent();

					MarkArrayDirty();
				}
			}
		}
	}


	if (LeftToRemoveCount == 0)
	{
		bRemoved = true;
	}

	NotifyRemoveReplicatedInventoryItemInstance(OutRemovedResults);

	//此处需要广播,避免主机玩家没有更新
	BroadcastItemChangeMessage();

	return bRemoved;
}

bool FGyraInventoryList::RemoveEntry(UGyraInventoryItemInstance* Instance, bool bRemovedToFree)
{
	bool bRemoved = false;
	for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
	{
		FGyraInventoryEntry& Entry = *EntryIt;

		if (Entry.Instance == Instance)
		{
			bRemoved = true;

			if (bRemovedToFree)
			{
				ReplaceEntryWithFree(EntryIt.GetIndex());

				MarkItemDirty(Entry);
			}
			else
			{
				EntryIt.RemoveCurrent();

				MarkArrayDirty();
				NotifyRemoveReplicatedInventoryItemInstance(Instance);
			}

			break;
		}
	}

	//此处需要广播,避免主机玩家没有更新
	BroadcastItemChangeMessage();

	return bRemoved;
}


bool FGyraInventoryList::SwapEntry(UGyraInventoryItemInstance* InInstanceA, UGyraInventoryItemInstance* InInstanceB)
{
	check(OwnerComponent);

	AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor->HasAuthority());


	int32 IndexA = FindIndexByInstance(InInstanceA);
	int32 IndexB = FindIndexByInstance(InInstanceB);

	if (IndexA != -1 && IndexB != -1)
	{
		FGyraInventoryEntry& EntryA = Entries[IndexA];
		FGyraInventoryEntry& EntryB = Entries[IndexB];


		FGyraInventoryEntry TempEntry;
		TempEntry.StackCount = EntryA.StackCount;
		TempEntry.Instance = EntryA.Instance;
		TempEntry.LastObservedCount = EntryA.LastObservedCount;


		EntryA.StackCount = EntryB.StackCount;
		EntryA.Instance = EntryB.Instance;
		EntryA.LastObservedCount = EntryB.LastObservedCount;


		EntryB.StackCount = TempEntry.StackCount;
		EntryB.Instance = TempEntry.Instance;
		EntryB.LastObservedCount = TempEntry.LastObservedCount;


		MarkItemDirty(EntryA);

		MarkItemDirty(EntryB);

		BroadcastItemChangeMessage();

		return true;
	}


	return false;
}


void FGyraInventoryList::ReplaceEntry(int32 InReplaceIndex,
                                      UGyraInventoryItemInstance* InNewInstance,
                                      int32 InNewStatCount)
{
	check(OwnerComponent);

	AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor->HasAuthority());

	check(IsValid(InNewInstance));

	FGyraInventoryEntry& RelpacedEntry = Entries[InReplaceIndex];

	UGyraInventoryItemInstance* OldItemInstance = RelpacedEntry.Instance;

	RelpacedEntry.Instance = InNewInstance;
	RelpacedEntry.StackCount = InNewStatCount;
	RelpacedEntry.LastObservedCount = INDEX_NONE;

	MarkItemDirty(RelpacedEntry);

	NotifyAddReplicatedInventoryItemInstance(InNewInstance);
	NotifyRemoveReplicatedInventoryItemInstance(OldItemInstance);

	//此处需要广播,避免主机玩家没有更新
	BroadcastItemChangeMessage();
}

void FGyraInventoryList::ReplaceEntry(
	UGyraInventoryItemInstance* InOldInstance,
	UGyraInventoryItemInstance* InNewInstance,
	int32 InNewStatCount)
{
	check(OwnerComponent);

	AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor->HasAuthority());
	check(IsValid(InOldInstance));
	check(IsValid(InNewInstance));

	int32 ReplaceIndex = FindIndexByInstance(InOldInstance);

	check(ReplaceIndex != -1)

	ReplaceEntry(ReplaceIndex, InNewInstance, InNewStatCount);
}

void FGyraInventoryList::ReplaceEntryWithFree(int32 InReplaceIndex)
{
	UGyraInventoryItemInstance* NewInstance = ConstructNewFreeItemInstance();
	int32 NewStackCount = 0;

	ReplaceEntry(InReplaceIndex, NewInstance, NewStackCount);
}

UGyraInventoryItemInstance* FGyraInventoryList::ConstructNewFreeItemInstance(
	TSubclassOf<UGyraInventoryItemDefinition> InItemDefClass)
{
	//@TODO: Using the actor instead of component as the outer due to UE-127172
	//根据虚幻的规则,以Actor作为外部拥有者,而非Component
	UGyraInventoryItemInstance* NewInstance = NewObject<UGyraInventoryItemInstance>(OwnerComponent->GetOwner());
	NewInstance->SetItemDef(InItemDefClass);
	return NewInstance;
}

void FGyraInventoryList::BroadcastChangeMessage(FGyraInventoryEntry& Entry, int32 OldCount, int32 NewCount)
{
	FGyraInventoryChangeMessage Message;
	Message.InventoryOwner = OwnerComponent;
	Message.Instance = Entry.Instance;
	Message.NewCount = NewCount;
	Message.Delta = NewCount - OldCount;

	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(OwnerComponent->GetWorld());
	MessageSystem.BroadcastMessage(TAG_Gyra_Inventory_Message_StackChanged, Message);
}

void FGyraInventoryList::BroadcastItemChangeMessage()
{
	FGyraInventoryChangeMessage Message;
	Message.InventoryOwner = OwnerComponent;


	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(OwnerComponent->GetWorld());
	MessageSystem.BroadcastMessage(TAG_Gyra_Inventory_Message_ItemChanged, Message);
}

void FGyraInventoryList::NotifyAddReplicatedInventoryItemInstance(UGyraInventoryItemInstance* InItemInstance)
{
	check(OwnerComponent);
	check(InItemInstance);
	AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor->HasAuthority());

	UGyraInventoryManagerComponent* InventoryManagerComponent = Cast<UGyraInventoryManagerComponent>(OwnerComponent);
	check(InventoryManagerComponent);


	InventoryManagerComponent->NotifyAddReplicatedInventoryItemInstance(InItemInstance);
}

void FGyraInventoryList::NotifyAddReplicatedInventoryItemInstance(TArray<UGyraInventoryItemInstance*> InItemInstances)
{
	for (auto& TmpItemInstance : InItemInstances)
	{
		NotifyAddReplicatedInventoryItemInstance(TmpItemInstance);
	}
}

void FGyraInventoryList::NotifyRemoveReplicatedInventoryItemInstance(UGyraInventoryItemInstance* InItemInstance)
{
	check(OwnerComponent);
	check(InItemInstance);
	AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor->HasAuthority());

	UGyraInventoryManagerComponent* InventoryManagerComponent = Cast<UGyraInventoryManagerComponent>(OwnerComponent);
	check(InventoryManagerComponent);

	InventoryManagerComponent->NotifyRemoveReplicatedInventoryItemInstance(InItemInstance);
}

void FGyraInventoryList::NotifyRemoveReplicatedInventoryItemInstance(
	TArray<UGyraInventoryItemInstance*> InItemInstances)
{
	for (auto& TmpItemInstance : InItemInstances)
	{
		NotifyRemoveReplicatedInventoryItemInstance(TmpItemInstance);
	}
}
