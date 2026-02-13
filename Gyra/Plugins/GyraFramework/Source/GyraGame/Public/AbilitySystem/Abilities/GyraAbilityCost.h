// Copyright 2025 Huagang Liu. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpec.h"
#include "Abilities/GameplayAbility.h"

#include "GyraAbilityCost.generated.h"


class UGyraGameplayAbility;


/**
 * UGyraAbilityCost
 *
 * GyraGameplayAbility进行消耗定义的父类,如弹药或充能
 * 
 */
UCLASS(DefaultToInstanced, EditInlineNew, Abstract)
class GYRAGAME_API UGyraAbilityCost : public UObject
{
	GENERATED_BODY()

public:
	UGyraAbilityCost()
	{
	}


	/**
	 * 
	 * 看看我们能否负担得起这笔消耗
	 * 
	 * 失败原因标签可以添加到OptionalRelevantTags（如果非空），可以在其他地方查询，以确定如何提供用户反馈（例如，如果武器弹药不足，点击噪音）
	 * 
	 * Ability和ActorInfo保证在条目时为非空，但OptionalRelevantTags标签可以为空。
	 * 
	 * @return true 代表我们可以支付这笔消耗,否则不能支付这笔消耗
	 * 
	 */
	virtual bool CheckCost(const UGyraGameplayAbility* Ability,
	                       const FGameplayAbilitySpecHandle Handle,
	                       const FGameplayAbilityActorInfo* ActorInfo,
	                       FGameplayTagContainer* OptionalRelevantTags) const
	{
		return true;
	}

	/**
	 * 
	 * 将该技能的消耗作用于目标
	 *
	 * 注:
	 * -你的实现不需要检查ShouldOnlyApplyCostOnHit()，调用者会为你做。
	 * -保证输入时Ability和ActorInfo为非空
	 * 
	 */
	virtual void ApplyCost(const UGyraGameplayAbility* Ability,
	                       const FGameplayAbilitySpecHandle Handle,
	                       const FGameplayAbilityActorInfo* ActorInfo,
	                       const FGameplayAbilityActivationInfo ActivationInfo)
	{
		
	}

	/** 若为真，则此费用只应在此技能成功命中时使用 */
	bool ShouldOnlyApplyCostOnHit() const { return bOnlyApplyCostOnHit; }

protected:
	/** 若为真，则此费用只应在此技能成功命中时使用 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Costs)
	bool bOnlyApplyCostOnHit = false;
};
