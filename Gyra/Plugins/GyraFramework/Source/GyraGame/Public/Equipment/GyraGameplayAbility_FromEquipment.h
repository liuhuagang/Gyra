// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "GyraGameplayAbility.h"

#include "GyraGameplayAbility_FromEquipment.generated.h"

class UGyraEquipmentInstance;
class UGyraInventoryItemInstance;

/**
 * UGyraGameplayAbility_FromEquipment
 *
 * An ability granted by and associated with an equipment instance
 */
UCLASS()
class UGyraGameplayAbility_FromEquipment : public UGyraGameplayAbility
{
	GENERATED_BODY()

public:

	UGyraGameplayAbility_FromEquipment(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category="Gyra|Ability")
	UGyraEquipmentInstance* GetAssociatedEquipment() const;

	UFUNCTION(BlueprintCallable, Category = "Gyra|Ability")
	UGyraInventoryItemInstance* GetAssociatedItem() const;

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif

};
