// Copyright 2024 Huagang Liu. All Rights Reserved.

#include "GyraCharacter.h"

#include "Components/SkeletalMeshComponent.h"
#include "TimerManager.h"


#include "GyraCameraComponent.h"
#include "GyraComboComponent.h"
#include "GyraCharacterMovementComponent.h"


#include "Player/GyraPlayerState.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(GyraCharacter)

class AActor;
class FLifetimeProperty;
class IRepChangedPropertyTracker;
class UInputComponent;


AGyraCharacter::AGyraCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Avoid ticking characters if possible.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;


	SetNetCullDistanceSquared(900000000.0f);

	CameraComponent = CreateDefaultSubobject<UGyraCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetRelativeLocation(FVector(-300.0f, 0.0f, 75.0f));

	ComboComponent = CreateDefaultSubobject<UGyraComboComponent>(TEXT("ComboComponent"));
}
