// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "Character/GyraCharacter.h"

#include "GyraCharacterWithAbilities.generated.h"

class UAbilitySystemComponent;
class UGyraAbilitySystemComponent;
class UObject;

class UGyraPawnData;
// AGyraCharacter typically gets the ability system component from the possessing player state
// This represents a character with a self-contained ability system component.
// GyraCharacter 是一个典型的通过ASC组件从受控制的玩家状态类获取.而AGyraCharacterWithAbilities是一个典型自己包含ASC组件的类
// 在本项目中,我们不使用这个类,而是重新写一个角色类,原先的类所包含了大量不需要的东西,对于怪物而言.
UCLASS(Blueprintable)
class GYRAGAME_API AGyraCharacterWithAbilities : public AGyraCharacter
{
	GENERATED_BODY()

public:
	AGyraCharacterWithAbilities(const FObjectInitializer& ObjectInitializer);

	virtual void PostInitializeComponents() override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

private:

	// The ability system component sub-object used by player characters.
	UPROPERTY(VisibleAnywhere, Category = "Gyra|PlayerState")
	TObjectPtr<UGyraAbilitySystemComponent> AbilitySystemComponent;
	
	// Health attribute set used by this actor.
	UPROPERTY()
	TObjectPtr<const class UGyraHealthSet> HealthSet;
	// Combat attribute set used by this actor.
	UPROPERTY()
	TObjectPtr<const class UGyraCombatSet> CombatSet;
};
