// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "ActiveGameplayEffectHandle.h"
#include "Engine/DataAsset.h"
#include "AttributeSet.h"
#include "GameplayTagContainer.h"

#include "GameplayAbilitySpecHandle.h"
#include "GyraAbilitySet.generated.h"

class UAttributeSet;
class UGameplayEffect;
class UGyraAbilitySystemComponent;
class UGyraGameplayAbility;
class UObject;

/**
 * FGyraAbilitySet_GameplayAbility
 *
 * Data used by the ability set to grant gameplay abilities.
 * 能力集用来授予游戏玩法能力的数据。
 */
USTRUCT(BlueprintType)
struct FGyraAbilitySet_GameplayAbility
{
	GENERATED_BODY()

public:

	// Gameplay ability to grant.
	// 游戏能力授予。
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGyraGameplayAbility> Ability = nullptr;

	// Level of ability to grant.
	// 授予的能力等级。
	UPROPERTY(EditDefaultsOnly)
	int32 AbilityLevel = 1;

	// Tag used to process input for the ability.
	// 标签用于处理输入的能力。
	UPROPERTY(EditDefaultsOnly, Meta = (Categories = "InputTag"))
	FGameplayTag InputTag;
};


/**
 * FGyraAbilitySet_GameplayEffect
 *
 * Data used by the ability set to grant gameplay effects.
 * 能力集用于授予游戏效果的数据。
 */
USTRUCT(BlueprintType)
struct FGyraAbilitySet_GameplayEffect
{
	GENERATED_BODY()

public:

	// Gameplay effect to grant.
	// 游戏效果授予。
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> GameplayEffect = nullptr;

	// Level of gameplay effect to grant.
	// 级别的游戏效果授予。
	UPROPERTY(EditDefaultsOnly)
	float EffectLevel = 1.0f;
};



/**
 * FGyraAbilitySet_AttributeSet
 *
 * Data used by the ability set to grant attribute sets.
 * 能力集用于授予属性集的数据。
 */
USTRUCT(BlueprintType)
struct FGyraAbilitySet_AttributeSet
{
	GENERATED_BODY()

public:
	// Gameplay effect to grant.
	// 游戏效果授予。
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAttributeSet> AttributeSet;

};

/**
 * FGyraAbilitySet_GrantedHandles
 *
 * Data used to store handles to what has been granted by the ability set.
 * 用于存储由能力集授予的句柄的数据。
 */
USTRUCT(BlueprintType)
struct FGyraAbilitySet_GrantedHandles
{
	GENERATED_BODY()

public:

	void AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle);
	void AddGameplayEffectHandle(const FActiveGameplayEffectHandle& Handle);
	void AddAttributeSet(UAttributeSet* Set);

	void TakeFromAbilitySystem(UGyraAbilitySystemComponent* GyraASC);

protected:

	// Handles to the granted abilities.
	UPROPERTY()
	TArray<FGameplayAbilitySpecHandle> AbilitySpecHandles;

	// Handles to the granted gameplay effects.
	UPROPERTY()
	TArray<FActiveGameplayEffectHandle> GameplayEffectHandles;

	// Pointers to the granted attribute sets
	UPROPERTY()
	TArray<TObjectPtr<UAttributeSet>> GrantedAttributeSets;
};

/**
 * UGyraAbilitySet
 *
 * Non-mutable data asset used to grant gameplay abilities and gameplay effects.
 * 用于授予游戏能力和游戏效果的不可变数据资产。
 */
UCLASS(BlueprintType, Const)
class UGyraAbilitySet : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UGyraAbilitySet(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// Grants the ability set to the specified ability system component.
	// The returned handles can be used later to take away anything that was granted.
	// 将能力集授予指定的能力系统组件。
	// 稍后可以使用返回的句柄来移除已授予的任何内容。
	void GiveToAbilitySystem(UGyraAbilitySystemComponent* GyraASC, const TMap<FGameplayTag, float>& InAttributeMap,
	FGyraAbilitySet_GrantedHandles* OutGrantedHandles, UObject* SourceObject = nullptr) const;

protected:

	// Gameplay abilities to grant when this ability set is granted.
	// 当这个能力集被授予时授予的游戏能力。
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Abilities", meta = (TitleProperty = Ability))
	TArray<FGyraAbilitySet_GameplayAbility> GrantedGameplayAbilities;

	// Gameplay effects to grant when this ability set is granted.
	// 当此技能集被授予时授予的游戏效果。
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects", meta = (TitleProperty = GameplayEffect))
	TArray<FGyraAbilitySet_GameplayEffect> GrantedGameplayEffects;

	// Attribute sets to grant when this ability set is granted.
	// 当此能力集被授予时，属性集被授予。
	UPROPERTY(EditDefaultsOnly, Category = "Attribute Sets", meta = (TitleProperty = AttributeSet))
	TArray<FGyraAbilitySet_AttributeSet> GrantedAttributes;
};
