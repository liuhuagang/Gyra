// Copyright 2024 Huagang Liu. All Rights Reserved.

#include "GyraMonster.h"

#include "Components/SkeletalMeshComponent.h"
#include "TimerManager.h"
#include "GyraAbilitySystemComponent.h"


#include "GyraStatsComponent.h"
#include "GyraHealthSet.h"
#include "GyraCombatSet.h"


AGyraMonster::AGyraMonster(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)

{
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	AbilitySystemComponent = ObjectInitializer.CreateDefaultSubobject<UGyraAbilitySystemComponent>(
		this, TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	// These attribute sets will be detected by AbilitySystemComponent::InitializeComponent. Keeping a reference so that the sets don't get garbage collected before that.
	// 这些属性集将被AbilitySystemComponent::InitializeComponent检测。保持引用，使集合在此之前不会被垃圾收集。
	HealthSet = CreateDefaultSubobject<UGyraHealthSet>(TEXT("HealthSet"));
	CombatSet = CreateDefaultSubobject<UGyraCombatSet>(TEXT("CombatSet"));

	MagicSet = CreateDefaultSubobject<UGyraMagicSet>(TEXT("MagicSet"));
	StatsSet = CreateDefaultSubobject<UGyraStatsSet>(TEXT("StatsSet"));

	// AbilitySystemComponent needs to be updated at a high frequency.
	// AbilitySystemComponent需要频繁更新。
	SetNetUpdateFrequency(100.0f);
}

void AGyraMonster::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	check(AbilitySystemComponent);
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
}

UAbilitySystemComponent* AGyraMonster::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
