// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "GameplayEffectExecutionCalculation.h"

#include "GyraEXPExecution.generated.h"

class UObject;


/**
 * UGyraEXPExecution
 *
 * 死亡时经验奖励
 *
 */
UCLASS()
class UGyraEXPExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:

	UGyraEXPExecution();

protected:

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
