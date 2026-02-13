// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "Components/ActorComponent.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "GyraInventoryList.h"
#include "GyraArchiveType.h"

#include "GyraInventoryManagerComponent.generated.h"

class UGyraInventoryItemDefinition;
class UGyraInventoryItemInstance;
class UGyraInventoryManagerComponent;
class UObject;
struct FFrame;
struct FGyraInventoryList;
struct FNetDeltaSerializeInfo;
struct FReplicationFlags;


UENUM(BlueprintType)
enum class EGyraInventoryListType :uint8
{
	None UMETA(DisplayName = "None"),
	QuickEquipment UMETA(DisplayName = "QuickEquipment"),
	PassiveEquipment UMETA(DisplayName = "PassiveEquipment"),
	Backpack UMETA(DisplayName = "Backpack"),
	Warehouse UMETA(DisplayName = "Warehouse"),
	Max UMETA(Hidden)
};

/**
 * Manages an inventory
 * 管理库存的组件,计划把快捷武器,背包和仓库都放到这里
 * 
 */
UCLASS(BlueprintType)
class GYRAGAME_API UGyraInventoryManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UGyraInventoryManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	
	// 列表的初始化操作
#pragma region ListInitOperation
		
	// 根据枚举获取要操作的容器列表的引用
	FGyraInventoryList& GetOperatedInventroyListByEnum(EGyraInventoryListType InListType);
	
	// 根据枚举获取同步的存档数据 必须由玩家客户端调用
	FGyraArchiveListInfomation GetArchiveListInfomationByEnum(EGyraInventoryListType InListType);

	// 构建空白的库存,必须在服务器上调用
	void InitInventoryList(EGyraInventoryListType InListType, int32 InListNum = 20);

	// 初始化列表的容量/存档内容,必须在服务器上调用
	UFUNCTION(Server, Reliable)
	void NetInitInventoryArchiveList(EGyraInventoryListType InListType,
									 FGyraArchiveListInfomation InListInfo, int32 InListNum = 20);
	
	// 由服务器通知指定的客户端,存档已经读取完毕
	UFUNCTION(Client, Reliable)
	void CallClientFinishInitInventoryArchiveList(EGyraInventoryListType InListType);
#pragma endregion ListInitOperation
	
	// 列表内部操作
#pragma region ListInnerOperation
	/**
	 * 
	 * 是否可以添加指定类型和数量的物品到列表
	 * 
	 * 此函数推荐在服务器上执行!
	 * 考虑叠加的情况,不会创建新的格子,但是可能会创建新的格子的占用或者复用老的格子的占用
	 * 
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Inventory)
	bool CanAddItemDefinition(EGyraInventoryListType InListType,
	                          TSubclassOf<UGyraInventoryItemDefinition> ItemDef, int32 StackCount = 1);

	/**
	 * 直接添加指定类型和数量为1的物品到列表
	 * 
	 * 此函数必须在服务器执行
	 * 
	 * 没有空闲槽位时会失败
	 * 尝试添加前,可以进行容量判定
	 * 
	 * @param InListType
	 * @param ItemDef
	 * @param bSuccess
	 * 
	 * @return
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Inventory)
	UGyraInventoryItemInstance* AddItemDefinition(EGyraInventoryListType InListType,
	                                              TSubclassOf<UGyraInventoryItemDefinition> ItemDef,
	                                              bool& bSuccess);


	/**
	 * 添加指定类型和数量的物品到列表
	 * 
	 * 该函数必须在服务器上执行
	 * 考虑了叠加,先使用旧的格子的占用,有需要会创建新的格子的占用,但是绝对不会创建新的格子
	 * 
	 * 尝试添加前,需要进行容量判定,此接口调用不一定成功
	 * 
	 * @param InListType
	 * @param ItemDef
	 * @param StackCount
	 * @return
	 * 
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Inventory)
	bool OverAddItemDefinition(EGyraInventoryListType InListType,
	                           TSubclassOf<UGyraInventoryItemDefinition> ItemDef,
	                           int32 StackCount = 1);


	/**
	 *
	 * 是否可以添加指定物品实例到列表
	 *
	 * 此函数必须在服务器执行
	 *
	 * 不考虑叠加的情况,不会创建新的格子,但是会创建新的格子的占用
	 *
	 *
	 * @param InListType
	 * @param ItemDef
	 * @param StackCount
	 * @return
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Inventory)
	bool CanAddItemInstance(EGyraInventoryListType InListType,
	                        UGyraInventoryItemInstance* InItemInstance, int32 StackCount = 1);


	/**
	 * 尝试添加指定物品实例到列表
	 * 
	 * 此函数在服务器上执行
	 * 
	 * 不需要考虑叠加问题,因为强制其数量不会超过其最大堆叠数量
	 * 不会创建新的格子,但是会创建新的格子的占用
	 * 
	 * 该函数可能会失败,在没有空闲位置时
	 * 
	 * @param InListType
	 * @param InItemInstance
	 * @param InStackCount
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Inventory)
	bool AddItemInstance(EGyraInventoryListType InListType,
	                     UGyraInventoryItemInstance* InItemInstance,
	                     int32 InStackCount);
	
	/**
	 * 移除指定物品实例从列表
	 * 
	 * 
	 * @param InListType
	 * @param ItemInstance
	 * 
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Inventory)
	void RemoveItemInstance(EGyraInventoryListType InListType, UGyraInventoryItemInstance* ItemInstance);

	/**
	 * 移除指定物品实例从列表
	 * 
	 * 由客户端调用,在服务器上执行
	 *
	 * @param InListType
	 * @param ItemInstance
	 *
	 */
	UFUNCTION(BlueprintCallable, Server, Reliable, Category = Inventory)
	void CallServerRemoveItemInstance(EGyraInventoryListType InListType, UGyraInventoryItemInstance* ItemInstance);

	/**
	 * 将指定的物品在列表中移除,并在场景中生成
	 * 
	 * @param InListType
	 * @param ItemInstance
	 */
	UFUNCTION(BlueprintCallable, Server, Reliable, Category = Inventory)
	void CallServerDropItemInstance(EGyraInventoryListType InListType, UGyraInventoryItemInstance* ItemInstance);


	/**
	 * 将背包中的所有物品移动到仓库
	 *
	 * @param InListType
	 * @param ItemInstance
	 */
	UFUNCTION(BlueprintCallable, Server, Reliable, Category = Inventory)
	void CallServerMoveAllItemFromBackToWarehouseInstance();


	/**
	 * 移除指定数量的指定类型的物品
	 * 
	 * 必须在服务器上执行,不推荐使用
	 * 
	 * 该移除方法是基于物品格子去移除,不考虑单个物品的堆叠
	 * 会产生失败的不完全移除
	 *
	 * 
	 * @param InListType
	 * @param ItemDef
	 * @param NumToConsume
	 * @return
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Inventory)
	bool ConsumeItemsByDefinition(EGyraInventoryListType InListType, TSubclassOf<UGyraInventoryItemDefinition> ItemDef,
	                              int32 NumToConsume);


	/**
	 * 检查是否可以移除指定数量的指定类型的物品
	 * 
	 * @param InListType
	 * @param ItemDef
	 * @param NumToConsume
	 * @return 
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Inventory)
	bool CheckConsumeItemStacksByDefinition(EGyraInventoryListType InListType,
	                                        TSubclassOf<UGyraInventoryItemDefinition> ItemDef, int32 NumToConsume);

	/**
	 * 移除指定数量的指定类型的物品
	 * 
	 * 必须在服务器上执行
	 * 
	 * 该移除方法是考虑了物品各自堆叠数量的情况
	 * 必须满足条件才能进行完全移除
	 *
	 * 
	 * @param InListType
	 * @param ItemDef
	 * @param NumToConsume
	 * @return
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Inventory)
	bool ConsumeItemStacksByDefinition(EGyraInventoryListType InListType,
	                                   TSubclassOf<UGyraInventoryItemDefinition> ItemDef, int32 NumToConsume);


	/**
	 * 获取所有的物品实例从列表
	 * 
	 * @param InListType
	 * @return 
	 */
	UFUNCTION(BlueprintCallable, Category = Inventory, BlueprintPure)
	TArray<UGyraInventoryItemInstance*> GetAllItems(EGyraInventoryListType InListType);

	/**
	 * 找到第一个该类型的物品实例从列表
	 * 
	 * 
	 * @param InListType
	 * @param ItemDef
	 * @return 
	 */
	UFUNCTION(BlueprintCallable, Category = Inventory, BlueprintPure)
	UGyraInventoryItemInstance* FindFirstItemStackByDefinition(EGyraInventoryListType InListType,
	                                                           TSubclassOf<UGyraInventoryItemDefinition> ItemDef);


	/**
	 * 找到该类型物品的所有物品实例从列表
	 * 
	 * 
	 * @param InListType
	 * @param ItemDef
	 * @return 
	 */
	UFUNCTION(BlueprintPure, Category = Inventory, BlueprintPure)
	TArray<UGyraInventoryItemInstance*> FindAllItemWithDefinition(EGyraInventoryListType InListType,
	                                                              TSubclassOf<UGyraInventoryItemDefinition> ItemDef);

	/**
	 * 找到该物品实例的索引如果未找到则返回-1
	 * 
	 * 
	 * @param InListType
	 * @param InInstance
	 * @return 
	 */
	UFUNCTION(BlueprintPure, Category = Inventory)
	int32 FindItemIndexByInstance(EGyraInventoryListType InListType, UGyraInventoryItemInstance* InInstance);


	/**
	 * 得到该类型的物品有几个实例,占用了几个格子
	 * 并非该类型的物品总数量个数
	 * 不考虑叠加的数量
	 * 
	 * @param InListType
	 * @param ItemDef
	 * @return 
	 */
	UFUNCTION(BlueprintCallable, Category = Inventory, BlueprintPure)
	int32 GetItemCountByDefinition(EGyraInventoryListType InListType,
	                               TSubclassOf<UGyraInventoryItemDefinition> ItemDef);


	/**
	 * 得到该类型的物品的总数量
	 * 考虑了叠加的数量
	 * 
	 * @param InListType
	 * @param ItemDef
	 * @return 
	 */
	UFUNCTION(BlueprintCallable, Category = Inventory, BlueprintPure)
	int32 GetItemStackCountByDefinition(EGyraInventoryListType InListType,
	                                    TSubclassOf<UGyraInventoryItemDefinition> ItemDef);

	/**
	* 
	 * 获取指定物品实例的数量
	 * 这个是单个格子的叠加之后的数量
	 * 一个物品实例理论上只对应一个格子
	 * 
	 * 
	 * @param InListType
	 * @param ItemInstance
	 * @return 
	 */
	UFUNCTION(BlueprintCallable, Category = Inventory)
	int32 GetItemCountByInstance(EGyraInventoryListType InListType, UGyraInventoryItemInstance* ItemInstance);

	/**
	 * 交换两个物品实例所在的槽位 在列表执行
	 * 
	 * 由客户端调用,必须在服务器上执行!
	 * 
	 *
	 * @param InListType
	 * @param InInstanceA
	 * @param InInstanceB
	 * 
	 */
	UFUNCTION(BlueprintCallable, Server, Reliable, Category = Inventory)
	void SwapEntryInner(EGyraInventoryListType InListType, UGyraInventoryItemInstance* InInstanceA,
	                    UGyraInventoryItemInstance* InInstanceB);

#pragma endregion ListInnerOperation

	//列表 跨表操作
#pragma region ListCrossOperation


	/**
	 * 获取它所在的列表
	 * 
	 * @param InItemInstance
	 * @return 
	 */
	EGyraInventoryListType GetItemInstanceInList(UGyraInventoryItemInstance* InItemInstance);


	/**
	 * 替换指定索引处的物品实例
	 *
	 * @param InListType
	 * @param InRelaceIndex
	 * @param InRelaceInstance
	 * @param InReleaceNum
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Inventory)
	void ReplaceItemByInstance(EGyraInventoryListType InListType,
	                           UGyraInventoryItemInstance* InOldInstance,
	                           UGyraInventoryItemInstance* InNewInstance,
	                           int32 InNewStatCount);


	/**
	 * 交换两个物品实例所在的槽位 跨列表执行
	 *
	 * 由客户端调用,必须在服务器上执行!
	 *
	 *
	 * @param InListTypeA
	 * @param InListTypeB
	 * 
	 * @param InInstanceA
	 * @param InInstanceB
	 * 
	 *
	 */
	UFUNCTION(BlueprintCallable, Server, Reliable, Category = Inventory)
	void SwapEntryCross(EGyraInventoryListType InListTypeA,
	                    UGyraInventoryItemInstance* InInstanceA,
	                    EGyraInventoryListType InListTypeB,
	                    UGyraInventoryItemInstance* InInstanceB);


#pragma endregion ListCrossOperation

public:
	//~UActorComponent interface
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~End of UActorComponent interface


	//~UObject interface
	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch,
	                                 FReplicationFlags* RepFlags) override;
	virtual void ReadyForReplication() override;
	//~End of UObject interface

	void NotifyAddReplicatedInventoryItemInstance(UGyraInventoryItemInstance* InItemInstance);
	void NotifyRemoveReplicatedInventoryItemInstance(UGyraInventoryItemInstance* InItemInstance);

protected:
	virtual void OnUnregister() override;
	void OnExperienceLoaded(const class UGyraExperienceDefinition* Experience);
	
	void ReadArchive();
	void WriteArchive();
	void AutoWriteArchive();



private:
	// 玩家快捷武器的数组
	UPROPERTY(Replicated)
	FGyraInventoryList QuickEquipmentList;

	//  玩家被动装备的数组
	UPROPERTY(Replicated)
	FGyraInventoryList PassiveEquipmentList;

	// 玩家背包的数组
	UPROPERTY(Replicated)
	FGyraInventoryList BackpackList;

	// 玩家仓库的数组
	UPROPERTY(Replicated)
	FGyraInventoryList WarehouseList;

	// 临时的体验 方便获取体验相关数据
	UPROPERTY()
	TObjectPtr<UGyraExperienceDefinition> TmpExperience = nullptr;

	// 是否已经读取过存档.
	bool bReadArchive = false;

	// 存档读取是否下发到客户端了
	TMap<EGyraInventoryListType, bool> RepArchiveMap;
};
