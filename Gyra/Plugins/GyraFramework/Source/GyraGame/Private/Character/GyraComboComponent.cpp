// Copyright 2024 Huagang Liu. All Rights Reserved.

#include "GyraComboComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

#include "GyraAbilitySystemComponent.h"

#include "GyraGameplayTags.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GyraComboComponent)

UGyraComboComponent::UGyraComboComponent(const FObjectInitializer& ObjectInitializer)
:Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
}

int32 UGyraComboComponent::GetComboIndex()
{
	return ComboIndex;
}

void UGyraComboComponent::SetComboAnimMontages(TArray<UAnimMontage*> InMontages)
{
	ComboMontages = InMontages;
	ComboMaxIndex = InMontages.Num()-1;
}

UAnimMontage* UGyraComboComponent::GetComboAnimMontage()
{
	LastPlayMontage = ComboMontages[ComboIndex];

	return ComboMontages[ComboIndex];

}

UAnimMontage* UGyraComboComponent::GetLastPlayAnimMontage()
{
	return LastPlayMontage;
}

void UGyraComboComponent::UpdateComboIndex()
{
	ComboIndex++;

	if (ComboIndex > ComboMaxIndex)
	{
		ComboIndex = 0;
	}
}

void UGyraComboComponent::ResetComboIndex()
{
	ComboIndex = 0;
}

void UGyraComboComponent::SetPressed()
{
	bShortPress = true;
}

void UGyraComboComponent::ResetPressed()
{
	bShortPress = false;
}

bool UGyraComboComponent::IsStillPressed()
{
	return bShortPress;
}

void UGyraComboComponent::ComboMelee()
{
	AActor* ThisOwner = GetOwner();

	if (ThisOwner->GetLocalRole() == ENetRole::ROLE_AutonomousProxy
	|| ThisOwner->GetLocalRole() ==ENetRole::ROLE_Authority
	)
	{
		if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(ThisOwner))
		{
			ASC->StopMontageIfCurrent(*LastPlayMontage);

			FGameplayTag InputTag = GyraGameplayTags::FindTagByString(TEXT("InputTag.Weapon.Fire"), true);

			Cast<UGyraAbilitySystemComponent>(ASC)->AbilityInputTagPressed(InputTag);
		}
	}

}

void UGyraComboComponent::BeginPlay()
{
	Super::BeginPlay();

	ComboMaxIndex = ComboMontages.Num() - 1;
}
