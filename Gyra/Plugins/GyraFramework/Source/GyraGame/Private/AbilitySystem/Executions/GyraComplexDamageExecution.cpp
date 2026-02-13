// Copyright 2024 Huagang Liu. All Rights Reserved.

#include "GyraComplexDamageExecution.h"

#include "GyraHealthSet.h"
#include "GyraCombatSet.h"
#include "GyraStatsSet.h"

#include "GyraGameplayTags.h"
#include "AbilitySystem/GyraGameplayEffectContext.h"
#include "AbilitySystem/GyraAbilitySourceInterface.h"
#include "Engine/World.h"
#include "GyraTeamSubsystem.h"

#include "PhysicalMaterialWithTags.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GyraComplexDamageExecution)

struct FComplexDamageStatics
{
	FGameplayEffectAttributeCaptureDefinition BaseDamageDef;

	FGameplayEffectAttributeCaptureDefinition Owner_A_V_Base_Def;
	FGameplayEffectAttributeCaptureDefinition Owner_A_R_Base_Def;

	FGameplayEffectAttributeCaptureDefinition Owner_A_V_CriticalStrikePercentage_Def;
	FGameplayEffectAttributeCaptureDefinition Owner_A_R_CriticalStrikeAttribute_Def;

	FGameplayEffectAttributeCaptureDefinition Owner_A_R_WeakStrike_Def;
	FGameplayEffectAttributeCaptureDefinition Owner_A_R_HitAgain_Def;
	FGameplayEffectAttributeCaptureDefinition Owner_A_V_Gemophagia_Def;
	FGameplayEffectAttributeCaptureDefinition Owner_A_R_Gemophagia_Def;

	FGameplayEffectAttributeCaptureDefinition Target_D_V_Base_Def;
	FGameplayEffectAttributeCaptureDefinition Target_D_R_Base_Def;
	FGameplayEffectAttributeCaptureDefinition Target_D_V_Dodge_Def;
	FGameplayEffectAttributeCaptureDefinition Target_D_V_Block_Def;

	FComplexDamageStatics()
	{
		//捕获基础伤害 是GE自身附带 目前没有使用它 纯靠属性运算
		BaseDamageDef = FGameplayEffectAttributeCaptureDefinition(UGyraCombatSet::GetBaseDamageAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);

		//捕获自身基础攻击力
		Owner_A_V_Base_Def = FGameplayEffectAttributeCaptureDefinition(UGyraStatsSet::GetAttribute_A_V_BaseAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
		//捕获自身伤害倍率加成
		Owner_A_R_Base_Def = FGameplayEffectAttributeCaptureDefinition(UGyraStatsSet::GetAttribute_A_R_BaseAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
		//捕获自身暴击几率
		Owner_A_V_CriticalStrikePercentage_Def = FGameplayEffectAttributeCaptureDefinition(UGyraStatsSet::GetAttribute_A_V_CriticalStrikePercentageAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
		//捕获自己暴击伤害
		Owner_A_R_CriticalStrikeAttribute_Def = FGameplayEffectAttributeCaptureDefinition(UGyraStatsSet::GetAttribute_A_R_CriticalStrikeAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);

		//捕获自己弱点攻击加成
		Owner_A_R_WeakStrike_Def = FGameplayEffectAttributeCaptureDefinition(UGyraStatsSet::GetAttribute_A_R_WeakStrikeAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
		//捕获自己再次概率
		Owner_A_R_HitAgain_Def = FGameplayEffectAttributeCaptureDefinition(UGyraStatsSet::GetAttribute_A_R_HitAgainAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
		//捕获自己固定击中回血
		Owner_A_V_Gemophagia_Def = FGameplayEffectAttributeCaptureDefinition(UGyraStatsSet::GetAttribute_A_V_GemophagiaAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
		//捕获自己击中百分比回血
		Owner_A_R_Gemophagia_Def = FGameplayEffectAttributeCaptureDefinition(UGyraStatsSet::GetAttribute_A_R_GemophagiaAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);




		//捕获目标基础防御力
		Target_D_V_Base_Def = FGameplayEffectAttributeCaptureDefinition(UGyraStatsSet::GetAttribute_D_V_BaseAttribute(), EGameplayEffectAttributeCaptureSource::Target, false);
		//捕获目标减伤百分比
		Target_D_R_Base_Def = FGameplayEffectAttributeCaptureDefinition(UGyraStatsSet::GetAttribute_D_R_BaseAttribute(), EGameplayEffectAttributeCaptureSource::Target, false);
		//捕获目标闪避几率
		Target_D_V_Dodge_Def = FGameplayEffectAttributeCaptureDefinition(UGyraStatsSet::GetAttribute_D_V_DodgeAttribute(), EGameplayEffectAttributeCaptureSource::Target, false);
		//捕获不表的搁置值
		Target_D_V_Block_Def = FGameplayEffectAttributeCaptureDefinition(UGyraStatsSet::GetAttribute_D_V_BlockAttribute(), EGameplayEffectAttributeCaptureSource::Target, false);
	}

};

static FComplexDamageStatics& ComplexDamageStatics()
{
	static FComplexDamageStatics Statics;
	return Statics;
}


UGyraComplexDamageExecution::UGyraComplexDamageExecution()
{
	RelevantAttributesToCapture.Add(ComplexDamageStatics().BaseDamageDef);

	RelevantAttributesToCapture.Add(ComplexDamageStatics().Owner_A_V_Base_Def);
	RelevantAttributesToCapture.Add(ComplexDamageStatics().Owner_A_R_Base_Def);
	RelevantAttributesToCapture.Add(ComplexDamageStatics().Owner_A_V_CriticalStrikePercentage_Def);
	RelevantAttributesToCapture.Add(ComplexDamageStatics().Owner_A_R_CriticalStrikeAttribute_Def);

	RelevantAttributesToCapture.Add(ComplexDamageStatics().Owner_A_R_WeakStrike_Def);
	RelevantAttributesToCapture.Add(ComplexDamageStatics().Owner_A_R_HitAgain_Def);
	RelevantAttributesToCapture.Add(ComplexDamageStatics().Owner_A_V_Gemophagia_Def);
	RelevantAttributesToCapture.Add(ComplexDamageStatics().Owner_A_R_Gemophagia_Def);


	RelevantAttributesToCapture.Add(ComplexDamageStatics().Target_D_V_Base_Def);
	RelevantAttributesToCapture.Add(ComplexDamageStatics().Target_D_R_Base_Def);
	RelevantAttributesToCapture.Add(ComplexDamageStatics().Target_D_V_Dodge_Def);
	RelevantAttributesToCapture.Add(ComplexDamageStatics().Target_D_V_Block_Def);

}

void UGyraComplexDamageExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
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

	//获取到本次基础伤害
	float BaseDamage = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(ComplexDamageStatics().BaseDamageDef,
	EvaluateParameters, BaseDamage);

	//获取自身的伤害值
	float Owner_A_V_Base = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(ComplexDamageStatics().Owner_A_V_Base_Def, 
	EvaluateParameters, Owner_A_V_Base);
	Owner_A_V_Base = FMath::Max<float>(Owner_A_V_Base, 0.0f);

	//获取自身的伤害加成系数	
	float Owner_A_R_Base = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(ComplexDamageStatics().Owner_A_R_Base_Def, 
	EvaluateParameters, Owner_A_R_Base);
	Owner_A_R_Base = FMath::Max<float>(Owner_A_R_Base, 0.0f);

	//获取自身暴击几率
	float Owner_A_V_CriticalStrikePercentage = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(ComplexDamageStatics().Owner_A_V_CriticalStrikePercentage_Def,
		EvaluateParameters, Owner_A_V_CriticalStrikePercentage);
	Owner_A_V_CriticalStrikePercentage = FMath::Max<float>(Owner_A_V_CriticalStrikePercentage, 0.0f);

	//获取自身暴击伤害
	float Owner_A_R_CriticalStrikeAttribute = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(ComplexDamageStatics().Owner_A_R_CriticalStrikeAttribute_Def,
		EvaluateParameters, Owner_A_R_CriticalStrikeAttribute);
	Owner_A_R_CriticalStrikeAttribute = FMath::Max<float>(Owner_A_R_CriticalStrikeAttribute, 0.0f);

	//获取自身弱点伤害 倍率加成
	float Owner_A_R_WeakStrike = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(ComplexDamageStatics().Owner_A_R_WeakStrike_Def,
		EvaluateParameters, Owner_A_R_WeakStrike);
	Owner_A_R_WeakStrike = FMath::Max<float>(Owner_A_R_WeakStrike, 0.0f);

	//获取自身再次命中几率
	float Owner_A_R_HitAgain = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(ComplexDamageStatics().Owner_A_R_HitAgain_Def,
		EvaluateParameters, Owner_A_R_HitAgain);
	Owner_A_R_HitAgain = FMath::Max<float>(Owner_A_R_HitAgain, 0.0f);

	//获取自身固定击中回血
	float Owner_A_V_Gemophagia = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(ComplexDamageStatics().Owner_A_V_Gemophagia_Def,
		EvaluateParameters, Owner_A_V_Gemophagia);
	Owner_A_V_Gemophagia = FMath::Max<float>(Owner_A_V_Gemophagia, 0.0f);

	//获取自身击中百分比回血
	float Owner_A_R_Gemophagia = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(ComplexDamageStatics().Owner_A_R_Gemophagia_Def,
		EvaluateParameters, Owner_A_R_Gemophagia);
	Owner_A_R_Gemophagia = FMath::Max<float>(Owner_A_R_Gemophagia, 0.0f);

	//获取目标防御力
	float Target_D_V_Base = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(ComplexDamageStatics().Target_D_V_Base_Def,
	EvaluateParameters, Target_D_V_Base);
	Target_D_V_Base = FMath::Max<float>(Target_D_V_Base, 0.0f);

	//获取目标减伤
	float Target_D_R_Base = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(ComplexDamageStatics().Target_D_R_Base_Def,
		EvaluateParameters, Target_D_R_Base);
	Target_D_R_Base = FMath::Max<float>(Target_D_R_Base, 0.0f);

	//获取目标闪避值
	float Target_D_V_Dodge = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(ComplexDamageStatics().Target_D_V_Dodge_Def,
		EvaluateParameters, Target_D_V_Dodge);
	Target_D_V_Dodge = FMath::Max<float>(Target_D_V_Dodge, 0.0f);

	//获取目标格挡值
	float Target_D_V_Block = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(ComplexDamageStatics().Target_D_V_Block_Def,
		EvaluateParameters, Target_D_V_Block);
	Target_D_V_Block = FMath::Max<float>(Target_D_V_Block, 0.0f);

	//1.计算闪避
	int32 DodgeCheck = FMath::RandRange(0, 100);
	bool bDodge = DodgeCheck < Target_D_V_Dodge ? true : false;

	if (bDodge)
	{
		//2.计算再次命中
		int32 HitAgainCheck = FMath::RandRange(0, 100);
		bool bHitAgain = HitAgainCheck < Owner_A_R_HitAgain ? true : false;

		if (bHitAgain)
		{
			//触发闪避后再次命中
		}
		else
		{
			//触发闪避后未再次命中
			return;
		}
	}
	//3. 未触发闪避 或者触发闪避后再次命中
	//计算是否暴击

	int32 RandCheck = FMath::RandRange(0, 100);
	bool bCritical = RandCheck < Owner_A_V_CriticalStrikePercentage ? true : false;

	//计算基础物理伤害+自身物理伤害-对方的格挡值
	BaseDamage = BaseDamage+ Owner_A_V_Base-Target_D_V_Block;

	//叠加暴击伤害
	if (bCritical)
	{
		BaseDamage += BaseDamage * Owner_A_R_CriticalStrikeAttribute/100;
	}

	const AActor* EffectCauser = TypedContext->GetEffectCauser();
	const FHitResult* HitActorResult = TypedContext->GetHitResult();

	AActor* HitActor = nullptr;
	FVector ImpactLocation = FVector::ZeroVector;
	FVector ImpactNormal = FVector::ZeroVector;
	FVector StartTrace = FVector::ZeroVector;
	FVector EndTrace = FVector::ZeroVector;

	// Calculation of hit actor, surface, zone, and distance all rely on whether the calculation has a hit result or not.
	// Effects just being added directly w/o having been targeted will always come in without a hit result, which must default
	// to some fallback information.
	if (HitActorResult)
	{
		const FHitResult& CurHitResult = *HitActorResult;
		HitActor = CurHitResult.HitObjectHandle.FetchActor();
		if (HitActor)
		{
			ImpactLocation = CurHitResult.ImpactPoint;
			ImpactNormal = CurHitResult.ImpactNormal;
			StartTrace = CurHitResult.TraceStart;
			EndTrace = CurHitResult.TraceEnd;
		}
	}

	// Handle case of no hit result or hit result not actually returning an actor
	UAbilitySystemComponent* TargetAbilitySystemComponent = ExecutionParams.GetTargetAbilitySystemComponent();
	if (!HitActor)
	{
		HitActor = TargetAbilitySystemComponent ? TargetAbilitySystemComponent->GetAvatarActor_Direct() : nullptr;
		if (HitActor)
		{
			ImpactLocation = HitActor->GetActorLocation();
		}
	}

	// Apply rules for team damage/self damage/etc...
	// 获取团队伤害规则
	float DamageInteractionAllowedMultiplier = 0.0f;
	if (HitActor)
	{
		UGyraTeamSubsystem* TeamSubsystem = HitActor->GetWorld()->GetSubsystem<UGyraTeamSubsystem>();
		if (ensure(TeamSubsystem))
		{
			DamageInteractionAllowedMultiplier = TeamSubsystem->CanCauseDamage(EffectCauser, HitActor) ? 1.0 : 0.0;
		}
	}

	// Determine distance
	double Distance = WORLD_MAX;

	if (TypedContext->HasOrigin())
	{
		Distance = FVector::Dist(TypedContext->GetOrigin(), ImpactLocation);
	}
	else if (EffectCauser)
	{
		Distance = FVector::Dist(EffectCauser->GetActorLocation(), ImpactLocation);
	}
	else
	{
		ensureMsgf(false, TEXT("Damage Calculation cannot deduce a source location for damage coming from %s; Falling back to WORLD_MAX dist!"), *GetPathNameSafe(Spec.Def));
	}
	bool bWeak =false;
	// Apply ability source modifiers
	float PhysicalMaterialAttenuation = 1.0f;
	float DistanceAttenuation = 1.0f;
	if (const IGyraAbilitySourceInterface* AbilitySource = TypedContext->GetAbilitySource())
	{
		if (const UPhysicalMaterial* PhysMat = TypedContext->GetPhysicalMaterial())
		{
			//计算弱点伤害增益系数
			PhysicalMaterialAttenuation = AbilitySource->GetPhysicalMaterialAttenuation(PhysMat, SourceTags, TargetTags);
			
			if (const UPhysicalMaterialWithTags* PhysMatWithTags =Cast<UPhysicalMaterialWithTags>(PhysMat))
			{
				bWeak= PhysMatWithTags->Tags.HasTagExact(FGameplayTag::RequestGameplayTag(FName("Gameplay.Zone.WeakSpot"),true));
				PhysicalMaterialAttenuation+= Owner_A_R_WeakStrike / 100;
			}
		
		}

		DistanceAttenuation = AbilitySource->GetDistanceAttenuation(Distance, SourceTags, TargetTags);
	}
	DistanceAttenuation = FMath::Max(DistanceAttenuation, 0.0f);

	// Clamping is done when damage is converted to -health
	// 当伤害转换为-生命值时，夹住
	// 叠加距离衰减 弱点伤害 团队规则 伤害加成
	BaseDamage = BaseDamage * DistanceAttenuation * PhysicalMaterialAttenuation * DamageInteractionAllowedMultiplier*(100+Owner_A_R_Base)/100;
	// 减少目标防御值 和伤害衰减
	BaseDamage = (BaseDamage-Target_D_V_Base)*(100- Target_D_R_Base)/100;

	const float DamageDone = FMath::Max(BaseDamage, 0.0f);

	FGameplayEffectSpec* MutableSpec = ExecutionParams.GetOwningSpecForPreExecuteMod();
	
	if (bCritical&&bWeak)
	{
		MutableSpec->AddDynamicAssetTag(GyraGameplayTags::DamageEffect_Physics_DeadAttack);

	}
	else if(bCritical&&!bWeak)
	{
		MutableSpec->AddDynamicAssetTag(GyraGameplayTags::DamageEffect_Physics_CriticalAttack);
	}
	else if (!bCritical && bWeak)
	{
		MutableSpec->AddDynamicAssetTag(GyraGameplayTags::DamageEffect_Physics_WeakAttack);
	}
	else
	{
		MutableSpec->AddDynamicAssetTag(GyraGameplayTags::DamageEffect_Physics_NormalAttack);
	}




	if (DamageDone > 0.0f)
	{
		// Apply a damage modifier, this gets turned into - health on the target
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UGyraHealthSet::GetDamageAttribute(), EGameplayModOp::Additive, DamageDone));
	}
#endif // #if WITH_SERVER_CODE
}

