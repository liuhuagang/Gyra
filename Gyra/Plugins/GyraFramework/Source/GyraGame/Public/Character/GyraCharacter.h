// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once


#include "GyraCharacterBase.h"

#include "GyraCharacter.generated.h"

class AActor;
class AController;

class FLifetimeProperty;
class IRepChangedPropertyTracker;
class UAbilitySystemComponent;
class UInputComponent;

class AGyraCharacterBase;
class AGyraPlayerController;
class AGyraPlayerState;

class UGyraAbilitySystemComponent;
class UGyraCameraComponent;
class UGyraHealthComponent;
class UGyraStatsComponent;
class UGyraPawnExtensionComponent;
class UGyraComboComponent;

class UObject;
struct FFrame;
struct FGameplayTag;
struct FGameplayTagContainer;





/**
 * AGyraCharacter
 *
 *	The base character pawn class used by this project.
 *	Responsible for sending events to pawn components.
 *	New behavior should be added via pawn components when possible.
 * 
 *	本项目的基础角色类.
 * 用于给各个棋子组件发送事件
 * 新的行为应该通过棋子组件进行添加,如果可能的话
 * 
 */
UCLASS(Config = Game, Meta = (ShortTooltip = "The base character pawn class used by this project."))
class GYRAGAME_API AGyraCharacter : public AGyraCharacterBase
{

	GENERATED_BODY()

public:

	AGyraCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gyra|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UGyraCameraComponent> CameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gyra|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UGyraComboComponent> ComboComponent;

};
