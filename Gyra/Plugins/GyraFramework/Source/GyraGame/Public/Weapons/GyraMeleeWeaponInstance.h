// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "GyraWeaponInstance.h"
#include "GyraAbilitySourceInterface.h"

#include "GyraMeleeWeaponInstance.generated.h"

class UPhysicalMaterial;


/**
 * UGyraMeleeWeaponInstance
 *
 * 近战武器的实例化对象
 * 
 */
UCLASS()
class UGyraMeleeWeaponInstance : public UGyraWeaponInstance, public IGyraAbilitySourceInterface
{
	GENERATED_BODY()

public:
	UGyraMeleeWeaponInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void PostLoad() override;



#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

public:

	void Tick(float DeltaSeconds);

	//~UGyraEquipmentInstance interface
	virtual void OnEquipped();
	virtual void OnUnequipped();
	//~End of UGyraEquipmentInstance interface

	//~IGyraAbilitySourceInterface interface
	virtual float GetDistanceAttenuation(float Distance, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr) const override;
	virtual float GetPhysicalMaterialAttenuation(const UPhysicalMaterial* PhysicalMaterial, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr) const override;
	//~End of IGyraAbilitySourceInterface interface

protected:
	// List of special tags that affect how damage is dealt
	// These tags will be compared to tags in the physical material of the thing being hit
	// If more than one tag is present, the multipliers will be combined multiplicatively
		UPROPERTY(EditAnywhere, Category = "Weapon Config")
		TMap<FGameplayTag, float> MaterialDamageMultiplier;
};
