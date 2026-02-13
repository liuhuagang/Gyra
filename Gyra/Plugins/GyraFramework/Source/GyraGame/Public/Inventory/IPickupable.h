// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "Templates/SubclassOf.h"
#include "UObject/Interface.h"

#include "UObject/ObjectPtr.h"
#include "IPickupable.generated.h"

template <typename InterfaceType> class TScriptInterface;

class AActor;
class UGyraInventoryItemDefinition;
class UGyraInventoryItemInstance;
class UGyraInventoryManagerComponent;
class UObject;
struct FFrame;

USTRUCT(BlueprintType)
struct FPickupTemplate
{
	GENERATED_BODY()

public:

	/**
	 * 拾取的该物品数量
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 StackCount = 1;

	/**
	 * 物品类型定义
	 * 
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UGyraInventoryItemDefinition> ItemDef;
};

USTRUCT(BlueprintType)
struct FPickupInstance
{
	GENERATED_BODY()

public:

	/**
	 * 拾取的该物品数量
	 * 只有类似于子弹,消耗品之类的数量会大于1.其余时候应该等于1
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 StackCount = 1;


	/**
	 * 具体的物品实例
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UGyraInventoryItemInstance> Item = nullptr;
};

/**
 * 定义了可以拾取的物品内容
 */
USTRUCT(BlueprintType)
struct FInventoryPickup
{
	GENERATED_BODY()

public:

	/**
	 * 直接是物品实例
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FPickupInstance> Instances;

	/**
	 * 根据物品类型进行生成
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FPickupTemplate> Templates;
};

/**  */
UINTERFACE(MinimalAPI, BlueprintType, meta = (CannotImplementInterfaceInBlueprint))
class UPickupable : public UInterface
{
	GENERATED_BODY()
};

/**  */
class IPickupable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	virtual FInventoryPickup GetPickupInventory() const = 0;

	UFUNCTION(BlueprintCallable)
	virtual void CancelSpecialEffects() = 0;

};

/**  */
UCLASS()
class UPickupableStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UPickupableStatics();

public:
	UFUNCTION(BlueprintPure)
	static TScriptInterface<IPickupable> GetFirstPickupableFromActor(AActor* Actor);

	/**
	 * 向背包中添加物品
	 * 
	 * 注意只能合计是一项 无论是实例还是定义
	 * 
	 * @param InventoryComponent
	 * @param Pickup
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, meta = (WorldContext = "Ability"))
	static bool AddPickupToInventory(UGyraInventoryManagerComponent* InventoryComponent, TScriptInterface<IPickupable> Pickup);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "Ability"))
	static bool CheckAddPickupToInventory(UGyraInventoryManagerComponent* InventoryComponent, TScriptInterface<IPickupable> Pickup);


};
