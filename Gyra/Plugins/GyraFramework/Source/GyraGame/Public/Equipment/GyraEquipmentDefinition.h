// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "Templates/SubclassOf.h"

#include "GyraEquipmentDefinition.generated.h"

class AActor;
class UGyraAbilitySet;
class UGyraEquipmentInstance;

USTRUCT()
struct FGyraEquipmentActorToSpawn
{
	GENERATED_BODY()

	FGyraEquipmentActorToSpawn()
	{}

	UPROPERTY(EditAnywhere, Category=Equipment)
	TSubclassOf<AActor> ActorToSpawn;

	UPROPERTY(EditAnywhere, Category=Equipment)
	FName AttachSocket;

	UPROPERTY(EditAnywhere, Category=Equipment)
	FTransform AttachTransform;
};


/**
 * UGyraEquipmentDefinition
 *
 * Definition of a piece of equipment that can be applied to a pawn
 */
UCLASS(Blueprintable, Const, Abstract, BlueprintType)
class UGyraEquipmentDefinition : public UObject
{
	GENERATED_BODY()

public:
	UGyraEquipmentDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// Class to spawn
	UPROPERTY(EditDefaultsOnly, Category=Equipment)
	TSubclassOf<UGyraEquipmentInstance> InstanceType;

	// Gameplay ability sets to grant when this is equipped
	UPROPERTY(EditDefaultsOnly, Category=Equipment)
	TArray<TObjectPtr<const UGyraAbilitySet>> AbilitySetsToGrant;

	// Actors to spawn on the pawn when this is equipped
	UPROPERTY(EditDefaultsOnly, Category=Equipment)
	TArray<FGyraEquipmentActorToSpawn> ActorsToSpawn;
};
