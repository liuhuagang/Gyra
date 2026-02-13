// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "Components/ControllerComponent.h"
#include "Inventory/GyraInventoryItemInstance.h"

#include "GyraQuickBarComponent.generated.h"

class AActor;
class UGyraEquipmentInstance;
class UGyraEquipmentManagerComponent;
class UObject;
struct FFrame;

UCLASS(Blueprintable, meta=(BlueprintSpawnableComponent))
class UGyraQuickBarComponent : public UControllerComponent
{
	GENERATED_BODY()

public:
	UGyraQuickBarComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/**
	 * 循环至下一个激活
	 * 
	 */
	UFUNCTION(BlueprintCallable, Category="Gyra")
	void CycleActiveSlotForward();

	/**
	 * 循环至上一个激活
	 * 
	 */
	UFUNCTION(BlueprintCallable, Category="Gyra")
	void CycleActiveSlotBackward();

	/**
	 * 从零开始激活
	 * 
	 */
	UFUNCTION(BlueprintCallable, Category = "Gyra")
	void CycleActiveFromIndexZero();

	/**
	 * 呼叫服务器从零开始激活
	 * 
	 */
	UFUNCTION(BlueprintCallable, Server, Reliable,Category = "Gyra")
	void CallServerCycleActiveFromIndexZero();

	/**
	 * 呼叫服务器激活快捷装备
	 * 
	 * @param NewIndex
	 */
	UFUNCTION(Server, Reliable, BlueprintCallable, Category="Gyra")
	void SetActiveSlotIndex(int32 NewIndex);

	/**
	 * 拿到所有的快捷装备栏
	 * 
	 * @return 
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure=false)
	TArray<UGyraInventoryItemInstance*> GetSlots() const
	{
		return Slots;
	}

	/**
	 * 拿到当前激活的索引
	 * 
	 * @return 
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure=false)
	int32 GetActiveSlotIndex() const { return ActiveSlotIndex; }

	/**
	 * 拿到当前激活的物品实例
	 * 
	 * @return 
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure = false)
	UGyraInventoryItemInstance* GetActiveSlotItem() const;

	/**
	 * 获取一个空的装备索引
	 * 
	 * @return 
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure=false)
	int32 GetNextFreeItemSlot() const;

	/**
	 * 装备该武器
	 * 目标位置必须为空
	 * 
	 * @param SlotIndex
	 * @param Item
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void AddItemToSlot(int32 SlotIndex, UGyraInventoryItemInstance* Item);

	/**
	 * 呼叫服务器在该位置装备该武器
	 * 
	 * @param SlotIndex
	 * @param Item
	 */
	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "Gyra|QuickBarComponent")
	void CallServerAddItemToSlot(int32 SlotIndex, UGyraInventoryItemInstance* Item);

	/**
	 * 移除目标位置的物品实例
	 * 
	 * @param SlotIndex
	 * @return 
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	UGyraInventoryItemInstance* RemoveItemFromSlot(int32 SlotIndex);

	/**
	 * 移除目标物品实例
	 * 
	 * @param InInstance
	 * @return 
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Gyra|QuickBarComponent")
	UGyraInventoryItemInstance* RemoveItemByInstance(UGyraInventoryItemInstance* InInstance);
	
	/**
	 * 
	 * 呼叫服务器移除该物品实例
	 * 
	 * @param InInstance
	 */
	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "Gyra|QuickBarComponent")
	void CallServerRemoveItemByInstance(UGyraInventoryItemInstance* InInstance);

	/**
	 * 交换两个物品实例所对应的快捷槽位
	 * 这里没办法使用实例因为有可能是空的
	 * 
	 * @param InInstance
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Gyra|QuickBarComponent")
	void SwapItemByInstance(int32 IndexA, int32 IndexB);

	/**
	 * 呼叫服务器移动位置,
	 * 谨慎使用该接口
	 * 
	 * @param IndexA
	 * @param IndexB
	 */
	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "Gyra|QuickBarComponent")
	void CallServerSwapItemByInstance(int32 IndexA, int32 IndexB);


	/**
	 * 找到物品实例的快捷槽索引
	 * 如果没找到返回-1
	 * 
	 * 
	 * @param InInstance
	 * @return 
	 */
	UFUNCTION(BlueprintPure, Category = "Gyra|QuickBarComponent")
	int32 FindSlotIndexByInstance(UGyraInventoryItemInstance* InInstance);

	/**
	 * 获取当前生成的装备实例
	 * 最好是在服务器上使用该接口
	 * 
	 * @return 
	 */
	UFUNCTION(BlueprintPure, Category = Gyra)
	UGyraEquipmentInstance* GetActiveEquipmentInstance();


public:

	virtual void BeginPlay() override;

private:
	void UnequipItemInSlot();
	void EquipItemInSlot();

	UGyraEquipmentManagerComponent* FindEquipmentManager() const;

protected:
	UPROPERTY()
	int32 NumSlots = 3;

	UFUNCTION()
	void OnRep_Slots();

	UFUNCTION()
	void OnRep_ActiveSlotIndex();

private:
	UPROPERTY(ReplicatedUsing=OnRep_Slots)
	TArray<TObjectPtr<UGyraInventoryItemInstance>> Slots;

	UPROPERTY(ReplicatedUsing=OnRep_ActiveSlotIndex)
	int32 ActiveSlotIndex = -1;

	UPROPERTY()
	TObjectPtr<UGyraEquipmentInstance> EquippedItem;
};


USTRUCT(BlueprintType)
struct FGyraQuickBarSlotsChangedMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category=Inventory)
	TObjectPtr<AActor> Owner = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = Inventory)
	TArray<TObjectPtr<UGyraInventoryItemInstance>> Slots;
};


USTRUCT(BlueprintType)
struct FGyraQuickBarActiveIndexChangedMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category=Inventory)
	TObjectPtr<AActor> Owner = nullptr;

	UPROPERTY(BlueprintReadOnly, Category=Inventory)
	int32 ActiveIndex = 0;
};
