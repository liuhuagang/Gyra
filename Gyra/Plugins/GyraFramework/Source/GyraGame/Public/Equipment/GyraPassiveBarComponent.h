// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "Components/ControllerComponent.h"
#include "Inventory/GyraInventoryItemInstance.h"

#include "GyraPassiveBarComponent.generated.h"

class AActor;
class UGyraEquipmentInstance;
class UGyraEquipmentManagerComponent;
class UObject;
struct FFrame;



UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent))
class UGyraPassiveBarComponent : public UControllerComponent
{
	GENERATED_BODY()

public:

	UGyraPassiveBarComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** 拿到所有的被动装备栏 */
	UFUNCTION(BlueprintCallable, BlueprintPure = false)
	TArray<UGyraInventoryItemInstance*> GetSlots() const { return Slots; }

	/** 装备该武器,目标位置必须为空 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Gyra|QuickBarComponent")
	void AddItemToSlot(int32 SlotIndex, UGyraInventoryItemInstance* Item);

	/** 呼叫服务器,在该位置装备该武器 */
	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "Gyra|QuickBarComponent")
	void CallServerAddItemToSlot(int32 SlotIndex, UGyraInventoryItemInstance* Item);

	/** 根据索引移除目标位置的物品实例 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Gyra|QuickBarComponent")
	UGyraInventoryItemInstance* RemoveItemFromSlot(int32 SlotIndex);

	/** 移除目标物品实例 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Gyra|QuickBarComponent")
	UGyraInventoryItemInstance* RemoveItemByInstance(UGyraInventoryItemInstance* InInstance);

	/** 呼叫服务器移除该物品实例 */
	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "Gyra|QuickBarComponent")
	void CallServerRemoveItemByInstance(UGyraInventoryItemInstance* InInstance);

	/** 找到物品实例的快捷槽索引 */
	UFUNCTION(BlueprintPure, Category = "Gyra|QuickBarComponent")
	int32 FindSlotIndexByItemInstance(UGyraInventoryItemInstance* InInstance);

public:

	virtual void BeginPlay() override;

private:

	void EquipItemInSlot(UGyraInventoryItemInstance* InInstance);

	void UnequipItemInSlot(UGyraInventoryItemInstance* InInstance);

	UGyraEquipmentManagerComponent* FindEquipmentManager() const;

protected:

	UPROPERTY()
	int32 NumSlots = 6;

	UFUNCTION()
	void OnRep_Slots();

private:

	UPROPERTY(ReplicatedUsing = OnRep_Slots)
	TArray<TObjectPtr<UGyraInventoryItemInstance>> Slots;

	UPROPERTY()
	TArray<TObjectPtr<UGyraEquipmentInstance>> EquippedItems;

};

USTRUCT(BlueprintType)
struct FGyraPassiveBarSlotsChangedMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = Inventory)
	TObjectPtr<AActor> Owner = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = Inventory)
	TArray<TObjectPtr<UGyraInventoryItemInstance>> Slots;
};

