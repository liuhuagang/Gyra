// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "Animation/AnimInstance.h"
#include "GameplayEffectTypes.h"
#include "GyraAnimInstance.generated.h"

class UAbilitySystemComponent;


/**
 * UGyraAnimInstance
 *
 * The base game animation instance class used by this project.
 * 本项目使用的基本游戏动画实例类。
 */
UCLASS(Config = Game)
class UGyraAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:

	UGyraAnimInstance(const FObjectInitializer& ObjectInitializer);

	virtual void InitializeWithAbilitySystem(UAbilitySystemComponent* ASC);

protected:

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif // WITH_EDITOR

	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:

	// Gameplay tags that can be mapped to blueprint variables. The variables will automatically update as the tags are added or removed.
	// 可以映射到蓝图变量的玩法标签。随着标签的添加或删除，变量将自动更新。
	// These should be used instead of manually querying for the gameplay tags.
	// 应该使用这些方法而不是手动查询游戏玩法标签。
	UPROPERTY(EditDefaultsOnly, Category = "GameplayTags")
	FGameplayTagBlueprintPropertyMap GameplayTagPropertyMap;

	UPROPERTY(BlueprintReadOnly, Category = "Character State Data")
	float GroundDistance = -1.0f;
};
