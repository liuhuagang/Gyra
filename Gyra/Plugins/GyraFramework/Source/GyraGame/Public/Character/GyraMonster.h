// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "GyraCharacterBase.h"
#include "GyraMonster.generated.h"

class AActor;
class AController;
class AGyraPlayerController;
class AGyraPlayerState;
class FLifetimeProperty;
class IRepChangedPropertyTracker;
class UAbilitySystemComponent;
class UInputComponent;
class UGyraAbilitySystemComponent;
class UGyraCameraComponent;
class UGyraStatsComponent;
class UGyraHealthComponent;
class UGyraPawnExtensionComponent;
class UObject;
struct FFrame;
struct FGameplayTag;
struct FGameplayTagContainer;
class UGyraExperienceDefinition;

/**
 * AGyraMonster
 * 本项目的基础怪物角色类.
 *
 */
UCLASS(Config = Game, Meta = (ShortTooltip = "The base Monster pawn class used by this project."))
class GYRAGAME_API AGyraMonster : public AGyraCharacterBase
{
	GENERATED_BODY()

public:

	AGyraMonster(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void PostInitializeComponents() override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
private:
	// The ability system component sub-object used by player characters.
	UPROPERTY(VisibleAnywhere, Category = "Gyra|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UGyraAbilitySystemComponent> AbilitySystemComponent;

	// Health attribute set used by this actor.
	UPROPERTY()
	TObjectPtr<const class UGyraHealthSet> HealthSet;
	// Combat attribute set used by this actor.
	UPROPERTY()
	TObjectPtr<const class UGyraCombatSet> CombatSet;

	UPROPERTY()
	TObjectPtr<const class UGyraMagicSet> MagicSet;

	UPROPERTY()
	TObjectPtr<const class UGyraStatsSet> StatsSet;
};
