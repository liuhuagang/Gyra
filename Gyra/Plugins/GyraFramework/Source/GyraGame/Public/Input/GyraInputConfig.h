// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"

#include "GyraInputConfig.generated.h"



class UInputAction;
class UObject;
struct FFrame;


/**
 * FGyraInputAction
 *
 * Struct used to map a input action to a gameplay input tag.
 * 结构体，用于将输入动作映射到游戏玩法输入标签。
 */
USTRUCT(BlueprintType)
struct FGyraInputAction
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<const UInputAction> InputAction = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (Categories = "InputTag"))
	FGameplayTag InputTag;
};



/**
 * UGyraInputConfig
 *
 * Non-mutable data asset that contains input configuration properties.
 * 包含输入配置属性的不可变数据资产。
 */
UCLASS(BlueprintType, Const)
class UGyraInputConfig : public UDataAsset
{
	GENERATED_BODY()

public:

	UGyraInputConfig(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = "Gyra|Pawn")
	const UInputAction* FindNativeInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound = true) const;

	UFUNCTION(BlueprintCallable, Category = "Gyra|Pawn")
	const UInputAction* FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound = true) const;

public:
	// List of input actions used by the owner.  These input actions are mapped to a gameplay tag and must be manually bound.
	// 所有者使用的输入动作列表。这些输入动作映射到游戏玩法标签，必须手动绑定。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (TitleProperty = "InputAction"))
	TArray<FGyraInputAction> NativeInputActions;

	// List of input actions used by the owner.  These input actions are mapped to a gameplay tag and are automatically bound to abilities with matching input tags.
	// 所有者使用的输入动作列表。这些输入动作被映射到游戏玩法标签，并自动绑定到具有匹配输入标签的能力。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (TitleProperty = "InputAction"))
	TArray<FGyraInputAction> AbilityInputActions;
};
