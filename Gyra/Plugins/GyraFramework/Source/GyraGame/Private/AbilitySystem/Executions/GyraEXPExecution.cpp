// Copyright 2024 Huagang Liu. All Rights Reserved.

#include "GyraEXPExecution.h"

#include "GyraHealthSet.h"
#include "GyraCombatSet.h"
#include "GyraStatsSet.h"

#include "GyraGameplayTags.h"
#include "AbilitySystem/GyraGameplayEffectContext.h"
#include "AbilitySystem/GyraAbilitySourceInterface.h"
#include "Engine/World.h"
#include "GyraTeamSubsystem.h"

#include "PhysicalMaterialWithTags.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GyraEXPExecution)

struct FEXPStatics
{
	//这里可以添加金币获取率

	FGameplayEffectAttributeCaptureDefinition ExpDef;



	FEXPStatics()
	{
		//捕获自身金币掉落
		ExpDef = FGameplayEffectAttributeCaptureDefinition(UGyraStatsSet::GetAttribute_S_F_RewardEXPAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);

	}

};

static FEXPStatics& EXPStatics()
{
	static FEXPStatics Statics;
	return Statics;
}


UGyraEXPExecution::UGyraEXPExecution()
{
	RelevantAttributesToCapture.Add(EXPStatics().ExpDef);

}

void UGyraEXPExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
#if WITH_SERVER_CODE

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	FGyraGameplayEffectContext* TypedContext = FGyraGameplayEffectContext::ExtractEffectContext(Spec.GetContext());
	check(TypedContext);

	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	float BaseRewardEXP = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(EXPStatics().ExpDef, EvaluateParameters, BaseRewardEXP);

	if (BaseRewardEXP > 0.0f)
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UGyraStatsSet::GetAttribute_S_F_EXPAttribute(), EGameplayModOp::Additive, BaseRewardEXP));
	}

#endif // #if WITH_SERVER_CODE


}

