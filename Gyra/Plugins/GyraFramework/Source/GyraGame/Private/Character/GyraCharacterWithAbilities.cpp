// Copyright 2024 Huagang Liu. All Rights Reserved.

#include "GyraCharacterWithAbilities.h"

#include "GyraCombatSet.h"
#include "GyraHealthSet.h"
#include "AbilitySystem/GyraAbilitySystemComponent.h"
#include "Async/TaskGraphInterfaces.h"
#include "GyraPawnData.h"
#include "GyraAbilitySet.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(GyraCharacterWithAbilities)

AGyraCharacterWithAbilities::AGyraCharacterWithAbilities(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AbilitySystemComponent = ObjectInitializer.CreateDefaultSubobject<UGyraAbilitySystemComponent>(this, TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	// These attribute sets will be detected by AbilitySystemComponent::InitializeComponent. Keeping a reference so that the sets don't get garbage collected before that.
	HealthSet = CreateDefaultSubobject<UGyraHealthSet>(TEXT("HealthSet"));
	CombatSet = CreateDefaultSubobject<UGyraCombatSet>(TEXT("CombatSet"));

	// AbilitySystemComponent needs to be updated at a high frequency.
	SetNetUpdateFrequency(100.0f);
}

void AGyraCharacterWithAbilities::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	check(AbilitySystemComponent);
	AbilitySystemComponent->InitAbilityActorInfo(this, this);

}

UAbilitySystemComponent* AGyraCharacterWithAbilities::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}



