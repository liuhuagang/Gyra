// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "GameplayEffectExecutionCalculation.h"

#include "GyraGoldExecution.generated.h"

class UObject;


/**
 * UGyraGoldExecution
 *
 * 死亡时奖励金币
 * 
 */
UCLASS()
class UGyraGoldExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:

	UGyraGoldExecution();

protected:

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
