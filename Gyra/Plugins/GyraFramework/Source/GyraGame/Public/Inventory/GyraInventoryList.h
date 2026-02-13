// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "Components/ActorComponent.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "GyraArchiveSlotInformation.h"
#include "GyraInventoryItemDefinition.h"
#include "GyraInventoryList.generated.h"

class UGyraInventoryItemDefinition;
class UGyraInventoryItemInstance;
class UGyraInventoryManagerComponent;
class UObject;
struct FFrame;
struct FGyraInventoryList;
struct FNetDeltaSerializeInfo;
struct FReplicationFlags;


/** A message when an item is added to the inventory */
/** 当一个项目被添加到库存时的消息 */
USTRUCT(BlueprintType)
struct FGyraInventoryChangeMessage
{
	GENERATED_BODY()

	//@TODO: Tag based names+owning actors for inventories instead of directly exposing the component?
	UPROPERTY(BlueprintReadOnly, Category = Inventory)
	TObjectPtr<UActorComponent> InventoryOwner = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = Inventory)
	TObjectPtr<UGyraInventoryItemInstance> Instance = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = Inventory)
	int32 NewCount = 0;

	UPROPERTY(BlueprintReadOnly, Category = Inventory)
	int32 Delta = 0;
};


/** A single entry in an inventory */
/** 单条目的库存 */
USTRUCT(BlueprintType)
struct FGyraInventoryEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

	FGyraInventoryEntry()
	{
	}

	FString GetDebugString() const;

public:
	bool IsUsedEntry();
	bool IsFreeEntry();

private:
	friend FGyraInventoryList;
	friend UGyraInventoryManagerComponent;

	UPROPERTY()
	TObjectPtr<UGyraInventoryItemInstance> Instance = nullptr;

	UPROPERTY()
	int32 StackCount = 0;

	UPROPERTY(NotReplicated)
	int32 LastObservedCount = INDEX_NONE;
};

/** List of inventory items */
USTRUCT(BlueprintType)
struct FGyraInventoryList : public FFastArraySerializer
{
	GENERATED_BODY()

	FGyraInventoryList()
		: OwnerComponent(nullptr)
	{
	}

	FGyraInventoryList(UActorComponent* InOwnerComponent)
		: OwnerComponent(InOwnerComponent)
	{
	}

public:
	//~FFastArraySerializer contract
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);
	//~End of FFastArraySerializer contract

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FGyraInventoryEntry, FGyraInventoryList>(
			Entries, DeltaParms, *this);
	}

public:
	/**
	 * 初始化仓库组件的容量,必须在服务器上调用
	 *
	 * @param InListNum
	 */
	TArray<UGyraInventoryItemInstance*> InitEmptyList(int32 InListNum = 20);

	/**
	 * 从存档初始化仓库组件,必须在服务器上调用
	 * 需要考虑版本变动而导致损坏的物品该如何处理
	 * 
	 * @param InListNum
	 */
	TArray<UGyraInventoryItemInstance*> InitArchiveList(const FGyraArchiveListInfomation& InListInformation,
	                                                    int32 InListNum);

	/**
	 * 导出仓库库存内容,是否考虑必须在服务器上处理?
	 * 不需要
	 * 
	 * @return 
	 */
	FGyraArchiveListInfomation ExportArchiveList();


	/**
	 * 在一个物品格子内添加指定类型和数量的物品
	 * 不考虑最大堆叠数量,默认为1.
	 * 
	 * 该方法只用作添加数量为1的装备时使用,其余时候都考虑使用OverAddEntry
	 * 
	 * 这个函数只在服务器上进行
	 * 不会和其他格子产生叠加,可能会突破最大堆叠数量,强制添加数量为1
	 * 此函数有可能成功,有可能失败
	 * 当默认添加至空闲槽位时,没有空闲槽位,那么此时添加失败,返回空指针.其余时候均返回新创建的物品实例.
	 * 
	 * @param ItemDef		物品类型的定义
	 * @param StackCount	添加的StackCount数量
	 * @param bAddToFree	是否只添加到空闲的槽位中,当无空闲槽位时会返回空指针表示添加失败.
	 * 
	 * @return
	 */
	UGyraInventoryItemInstance* AddEntry(TSubclassOf<UGyraInventoryItemDefinition> ItemDefClass, bool& bSuccess,
	                                     int32 StackCount = 1, bool bAddToFree = true);


	/**
	 * 计算单个格子的最大堆叠数量,添加指定类型和数量的物品
	 * 
	 * 这个函数只在服务器上进行
	 * 可能会产生多个格子的新占用,也有可能不产生新的格子占用
	 * 此函数绝对不会创建新的格子
	 * 如果没有办法全部添加,则视为添加失败,不做任何改变.
	 * 
	 * @param ItemClass		物品类型定义
	 * @param StackCount	添加的StackCount数量
	 * @return
	 */
	bool OverAddEntry(TSubclassOf<UGyraInventoryItemDefinition> ItemDefClass, int32 StackCount);

	/**
	 * 检测是否可以添加该类型的物品和数量,会考虑最大堆叠数量
	 * 
	 * 此函数在服务器和客户端都可执行
	 * 
	 * @param ItemClass		物品类型定义
	 * @param StackCount	添加的StackCount数量
	 * @return 
	 */
	bool CheckOverAddEntry(TSubclassOf<UGyraInventoryItemDefinition> ItemDef, int32 StackCount);


	/**
	 * 检测是否可以添加一个物品实例
	 * 
	 * 此函数推荐在服务器上执行
	 * 
	 * 不考虑堆叠,直接创建新的占用
	 * 
	 * @param InInstance		物品实例
	 * @param InStackCount		物品数量
	 */
	bool CheckAddEntry(UGyraInventoryItemInstance* InInstance, int32 InStackCount = 1);


	/**
	 * 添加实例化后的物品
	 * 
	 * 必须在服务器上执行
	 * 
	 * 
	 * 此方法必然不会产生一个新的格子,但会产生一个新的格子占用
	 * 
	 *
	 * 不考虑最大数量的计算,如果超过最大堆叠数量,自动锁定为最大堆叠数量
	 *
	 * @param Instance
	 */

	bool AddEntry(UGyraInventoryItemInstance* InInstance, int32 InStackCount = 1);


	/**
	 * 拿到所有的实例化物品
	 *
	 * @return
	 */
	TArray<UGyraInventoryItemInstance*> GetAllItems() const;

	/**
	 * 寻找指定实例化的物品数量,未找到返回0
	 * 通常一个格子只对应一个实例化的物品
	 *
	 * @param ItemInstance
	 * @return
	 */
	int32 GetItemCountByInstance(UGyraInventoryItemInstance* ItemInstance);


	/**
	 * 通过传入物品实例,找到目标得索引,如果没有则返回-1
	 *
	 * @param ItemInstance
	 * @return
	 */
	int32 FindIndexByInstance(UGyraInventoryItemInstance* ItemInstance);

	/**
	 * 通过传入物品索引,找到目标的实例,如果没有则返回空指针
	 *
	 * @param ItemInstance
	 * @return
	 */
	UGyraInventoryItemInstance* FindIndexByInstance(int32 InItemIndex);


	/**
	 * 找到第一个空闲的槽位
	 * 如果没有返回-1
	 * @return 
	 */
	int32 FindFirstFreeEntry();

	/**
	 * 获取剩下的可用容量
	 * 
	 * @return 
	 */
	int32 GetFreeCapacityNum();

	/**
	 * 获取已使用的容量
	 *
	 * @return
	 */
	int32 GetUsedCapacityNum();

	/**
	 * 获取预先设置的最大容量
	 *
	 * @return
	 */
	int32 GetMaxCapacityNum();

	/**
	 * 检查是否可以移除指定类型的物品的指定数量
	 * 
	 * 该函数推荐在服务器上执行
	 * 
	 * 
	 * @param ItemClass		物品类型
	 * @param StackCount	物品数量
	 * @return 
	 */
	bool CheckRemoveEntry(TSubclassOf<UGyraInventoryItemDefinition> ItemClass, int32 StackCount);

	/**
	 * 移除指定物品类的,指定数量.如果移除的数量超过拥有的数量,则不移除.
	 * 
	 * 该函数必须在服务器上执行
	 * 需要向上返回已经移除的实例结果
	 *
	 * @param ItemClass
	 * @param StackCount
	 * @return
	 */
	bool RemoveEntry(TSubclassOf<UGyraInventoryItemDefinition> ItemClass, int32 StackCount,
	                 TArray<UGyraInventoryItemInstance*>& OutRemovedResults, bool bRemovedToFree = true);

	/**
	 * 移除指定实例化的物品
	 *
	 * @param Instance
	 */
	bool RemoveEntry(UGyraInventoryItemInstance* Instance, bool bRemovedToFree = true);

	/**
	 * 用来交换两个物品实例的位置
	 *
	 * @param InInstanceA
	 * @param InInstanceB
	 * @return
	 */
	bool SwapEntry(UGyraInventoryItemInstance* InInstanceA, UGyraInventoryItemInstance* InInstanceB);

	/**
	 * 替换指定索引处的物品实例
	 *
	 * @param InRelaceIndex
	 * @param InRelaceInstance
	 * @param InReleaceNum
	 */
	void ReplaceEntry(int32 InReplaceIndex, UGyraInventoryItemInstance* InNewInstance, int32 InNewStatCount);
	void ReplaceEntry(UGyraInventoryItemInstance* InOldInstance, UGyraInventoryItemInstance* InNewInstance,
	                  int32 InNewStatCount);

	void ReplaceEntryWithFree(int32 InReplaceIndex);

	/**
	 * 构建一个新的物品实例
	 * 
	 * @param InItemDefClass
	 * @return 
	 */
	UGyraInventoryItemInstance* ConstructNewFreeItemInstance(
		TSubclassOf<UGyraInventoryItemDefinition> InItemDefClass = UGyraInventoryItemDefinition::StaticClass());

private:
	void BroadcastChangeMessage(FGyraInventoryEntry& Entry, int32 OldCount, int32 NewCount);

	/**
	 * 广播列表收到了改变
	 * 目前是全域广播
	 *
	 */
	void BroadcastItemChangeMessage();


	/**
	 * 添加需要同步的物品实例,该函数由具体容器列表通知所属组件处理对象时调用
	 *
	 * @param InItemInstance
	 */
	void NotifyAddReplicatedInventoryItemInstance(UGyraInventoryItemInstance* InItemInstance);
	void NotifyAddReplicatedInventoryItemInstance(TArray<UGyraInventoryItemInstance*> InItemInstances);

	/**
	 * 移除需要同步的物品实例,该函数由具体容器列表通知所属组件处理对象时调用
	 *
	 * @param InItemInstance
	 */
	void NotifyRemoveReplicatedInventoryItemInstance(UGyraInventoryItemInstance* InItemInstance);
	void NotifyRemoveReplicatedInventoryItemInstance(TArray<UGyraInventoryItemInstance*> InItemInstances);

private:
	friend UGyraInventoryManagerComponent;

private:
	// Replicated list of items
	UPROPERTY()
	TArray<FGyraInventoryEntry> Entries;

	UPROPERTY(NotReplicated)
	TObjectPtr<UActorComponent> OwnerComponent;
};

template <>
struct TStructOpsTypeTraits<FGyraInventoryList> : public TStructOpsTypeTraitsBase2<FGyraInventoryList>
{
	enum { WithNetDeltaSerializer = true };
};
