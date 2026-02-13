// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "GameplayEffectExecutionCalculation.h"

#include "GyraDamageExecution.generated.h"

class UObject;


/**
 * UGyraDamageExecution
 *
 *	Execution used by gameplay effects to apply damage to the health attributes.
 */
UCLASS()
class UGyraDamageExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:

	UGyraDamageExecution();

protected:

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
