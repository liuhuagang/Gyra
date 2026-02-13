// Copyright 2024 Huagang Liu. All Rights Reserved.

#include "GyraMeleeWeaponInstance.h"

#include "NativeGameplayTags.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/GyraCameraComponent.h"
#include "Physics/PhysicalMaterialWithTags.h"
#include "GyraWeaponInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GyraMeleeWeaponInstance)


UGyraMeleeWeaponInstance::UGyraMeleeWeaponInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void UGyraMeleeWeaponInstance::PostLoad()
{
	Super::PostLoad();

}

#if WITH_EDITOR
void UGyraMeleeWeaponInstance::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

}
#endif

void UGyraMeleeWeaponInstance::Tick(float DeltaSeconds)
{
	APawn* Pawn = GetPawn();
	check(Pawn != nullptr);
}
void UGyraMeleeWeaponInstance::OnEquipped()
{
	Super::OnEquipped();
}
void UGyraMeleeWeaponInstance::OnUnequipped()
{
	Super::OnUnequipped();
}

float UGyraMeleeWeaponInstance::GetDistanceAttenuation(float Distance, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags) const
{
	return 1.0f;
}

float UGyraMeleeWeaponInstance::GetPhysicalMaterialAttenuation(const UPhysicalMaterial* PhysicalMaterial, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags) const
{
	float CombinedMultiplier = 1.0f;
	if (const UPhysicalMaterialWithTags* PhysMatWithTags = Cast<const UPhysicalMaterialWithTags>(PhysicalMaterial))
	{
		for (const FGameplayTag MaterialTag : PhysMatWithTags->Tags)
		{
			if (const float* pTagMultiplier = MaterialDamageMultiplier.Find(MaterialTag))
			{
				CombinedMultiplier *= *pTagMultiplier;
			}
		}
	}

	return CombinedMultiplier;
}
