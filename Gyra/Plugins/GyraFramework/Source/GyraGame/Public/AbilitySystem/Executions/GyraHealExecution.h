// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "GameplayEffectExecutionCalculation.h"

#include "GyraHealExecution.generated.h"

class UObject;


/**
 * UGyraHealExecution
 *
 *	Execution used by gameplay effects to apply healing to the health attributes.
 */
UCLASS()
class UGyraHealExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:

	UGyraHealExecution();

protected:

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
