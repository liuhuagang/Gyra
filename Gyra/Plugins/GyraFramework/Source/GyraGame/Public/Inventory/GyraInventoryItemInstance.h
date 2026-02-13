// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "System/GameplayTagStack.h"
#include "Templates/SubclassOf.h"

#include "GyraInventoryItemInstance.generated.h"

class FLifetimeProperty;

class UGyraInventoryItemDefinition;
class UGyraInventoryItemFragment;
struct FFrame;
struct FGameplayTag;
struct FGyraInventoryItemIndexInfo;

enum class EGyraInventoryItemType : uint8;
enum class EGyraInventoryItemQuality :uint8;

/**
 * UGyraInventoryItemInstance
 * 物品的实例化对象,用于网络数据的同步
 */
UCLASS(BlueprintType)
class UGyraInventoryItemInstance : public UObject
{
	GENERATED_BODY()

public:
	UGyraInventoryItemInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~UObject interface
	virtual bool IsSupportedForNetworking() const override { return true; }
	//~End of UObject interface

	// Adds a specified number of stacks to the tag (does nothing if StackCount is below 1)
	// 为标签添加指定数量的堆栈（如果StackCount低于1，则不执行任何操作）
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Inventory)
	void AddStatTagStack(FGameplayTag Tag, int32 StackCount);

	// Removes a specified number of stacks from the tag (does nothing if StackCount is below 1)
	// 从标签中移除指定数量的堆栈（如果StackCount低于1，则不执行任何操作）
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category= Inventory)
	void RemoveStatTagStack(FGameplayTag Tag, int32 StackCount);

	// Returns the stack count of the specified tag (or 0 if the tag is not present)
	// 返回指定标签的堆栈计数（如果标签不存在，则返回0）
	UFUNCTION(BlueprintCallable, Category=Inventory)
	int32 GetStatTagStackCount(FGameplayTag Tag) const;

	// Returns true if there is at least one stack of the specified tag
	// 如果指定的标记至少有一个堆栈，则返回true
	UFUNCTION(BlueprintCallable, Category=Inventory)
	bool HasStatTag(FGameplayTag Tag) const;

	/**
	 * 拿到所有的装备属性Tag
	 * 
	 * @return 
	 */
	UFUNCTION(BlueprintCallable, Category = Inventory)
	TMap<FGameplayTag,float> GetAllStatsTag () const;

	//获取到物品定义
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Inventory)
	TSubclassOf<UGyraInventoryItemDefinition> GetItemDef() { return ItemDef; };

	//获取到物品定义
	TSubclassOf<UGyraInventoryItemDefinition> GetItemDef() const
	{
		return ItemDef;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Inventory)
	EGyraInventoryItemType GetItemTypeOnCreate() const;

	//获取所有的存档Tag
	TArray<FGyraInventoryTagStack> GetArchiveTagArray();

	//获取指定的碎片类型
	UFUNCTION(BlueprintCallable, BlueprintPure=false, meta=(DeterminesOutputType=FragmentClass))
	const UGyraInventoryItemFragment* FindFragmentByClass(TSubclassOf<UGyraInventoryItemFragment> FragmentClass) const;
	
	//获取指定的碎片类型的内部泛型方法
	template <typename ResultClass>
	const ResultClass* FindFragmentByClass() const
	{
		return (ResultClass*)FindFragmentByClass(ResultClass::StaticClass());
	}

private:
#if UE_WITH_IRIS
	/** Register all replication fragments */
	virtual void RegisterReplicationFragments(UE::Net::FFragmentRegistrationContext& Context, UE::Net::EFragmentRegistrationFlags RegistrationFlags) override;
#endif // UE_WITH_IRIS

	void SetItemDef(TSubclassOf<UGyraInventoryItemDefinition> InDef);

	friend struct FGyraInventoryList;

private:

	UPROPERTY(Replicated)
	FGameplayTagStackContainer StatTags;

	
	// The item definition
	UPROPERTY(Replicated)
	TSubclassOf<UGyraInventoryItemDefinition> ItemDef;
};
