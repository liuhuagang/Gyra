// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "Interaction/IInteractableTarget.h"
#include "Interaction/InteractionOption.h"
#include "Inventory/IPickupable.h"

#include "GyraWorldCollectable.generated.h"

class UObject;
struct FInteractionQuery;

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class AGyraWorldCollectable : public AActor, public IInteractableTarget, public IPickupable
{
	GENERATED_BODY()

public:

	AGyraWorldCollectable();

	// 重写复制属性函数
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void GatherInteractionOptions(const FInteractionQuery& InteractQuery, FInteractionOptionBuilder& InteractionBuilder) override;
	virtual FInventoryPickup GetPickupInventory() const override;

	virtual void CancelSpecialEffects();

	UFUNCTION(BlueprintCallable)
	void SetStaticInventory(FInventoryPickup InFInventoryPickup);

	UFUNCTION(BlueprintImplementableEvent)
	void K2_CancelSpecialEffects();

	UFUNCTION(BlueprintImplementableEvent)
	void K2_InitFromItemDefination(FPickupTemplate InPickupTemplate);
	
	UFUNCTION(BlueprintImplementableEvent)
	void K2_InitFromItemInstance(FPickupInstance InPickupInstance);

	UFUNCTION()
	void OnRep_PickupInfo();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FInteractionOption Option;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FInventoryPickup StaticInventory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,ReplicatedUsing = OnRep_PickupInfo)
	FPickupTemplate ReplicatedPickupInfo;

};
