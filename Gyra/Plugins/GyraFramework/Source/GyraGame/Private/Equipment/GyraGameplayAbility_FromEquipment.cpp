// Copyright 2024 Huagang Liu. All Rights Reserved.

#include "GyraGameplayAbility_FromEquipment.h"
#include "GyraEquipmentInstance.h"
#include "Inventory/GyraInventoryItemInstance.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

#include UE_INLINE_GENERATED_CPP_BY_NAME(GyraGameplayAbility_FromEquipment)

UGyraGameplayAbility_FromEquipment::UGyraGameplayAbility_FromEquipment(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UGyraEquipmentInstance* UGyraGameplayAbility_FromEquipment::GetAssociatedEquipment() const
{
	if (FGameplayAbilitySpec* Spec = UGameplayAbility::GetCurrentAbilitySpec())
	{
		return Cast<UGyraEquipmentInstance>(Spec->SourceObject.Get());
	}

	return nullptr;
}

UGyraInventoryItemInstance* UGyraGameplayAbility_FromEquipment::GetAssociatedItem() const
{
	if (UGyraEquipmentInstance* Equipment = GetAssociatedEquipment())
	{
		return Cast<UGyraInventoryItemInstance>(Equipment->GetInstigator());
	}
	return nullptr;
}


#if WITH_EDITOR
EDataValidationResult UGyraGameplayAbility_FromEquipment::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);
PRAGMA_DISABLE_DEPRECATION_WARNINGS
	if (InstancingPolicy == EGameplayAbilityInstancingPolicy::NonInstanced)
PRAGMA_ENABLE_DEPRECATION_WARNINGS
	{
		Context.AddError(NSLOCTEXT("Gyra", "EquipmentAbilityMustBeInstanced", "Equipment ability must be instanced"));
		Result = EDataValidationResult::Invalid;
	}

	return Result;
}

#endif
