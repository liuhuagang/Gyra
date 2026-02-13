// Copyright 2024 Huagang Liu. All Rights Reserved.

#include "GyraPawnInitInfo.h"

#include "GyraAbilitySystemComponent.h"
#include "GyraGameplayTags_Attribute.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GyraPawnInitInfo)

//此处使用宏进行简化拼接
#define XG_PAWN_VALUE_TO_TAG(InPropertyName) \
StatsSets.Add(GyraGameplayTags::InPropertyName,InPropertyName);


void FGyraPawnInitInfo::InitAttributeFromInfo(UGyraAbilitySystemComponent* InASC) const
{
	if (!InASC)
	{
		check(false);
	}


	TMap<FGameplayTag, int32> StatsSets;

	XG_PAWN_VALUE_TO_TAG(Attribute_S_F_Level);
	XG_PAWN_VALUE_TO_TAG(Attribute_S_F_EXP);
	XG_PAWN_VALUE_TO_TAG(Attribute_S_F_RewardEXP);
	XG_PAWN_VALUE_TO_TAG(Attribute_S_F_MaxEXP);
	XG_PAWN_VALUE_TO_TAG(Attribute_S_F_Glod);
	XG_PAWN_VALUE_TO_TAG(Attribute_S_F_RewardGlod);
	XG_PAWN_VALUE_TO_TAG(Attribute_S_V_MaxHealth);
	XG_PAWN_VALUE_TO_TAG(Attribute_S_R_MaxHealth);
	XG_PAWN_VALUE_TO_TAG(Attribute_S_V_MaxMana);
	XG_PAWN_VALUE_TO_TAG(Attribute_S_R_MaxMana);
	XG_PAWN_VALUE_TO_TAG(Attribute_S_V_FightRecoverHealth);
	XG_PAWN_VALUE_TO_TAG(Attribute_S_V_OutOfFightRecoverHealth);
	XG_PAWN_VALUE_TO_TAG(Attribute_S_V_FightRecoverMana);
	XG_PAWN_VALUE_TO_TAG(Attribute_S_V_OutOfFightRecoverMana);


	XG_PAWN_VALUE_TO_TAG(Attribute_A_V_Base);
	XG_PAWN_VALUE_TO_TAG(Attribute_A_R_Base);
	XG_PAWN_VALUE_TO_TAG(Attribute_A_V_CriticalStrikePercentage);
	XG_PAWN_VALUE_TO_TAG(Attribute_A_R_CriticalStrike);
	XG_PAWN_VALUE_TO_TAG(Attribute_A_R_WeakStrike);
	XG_PAWN_VALUE_TO_TAG(Attribute_A_R_HitAgain);
	XG_PAWN_VALUE_TO_TAG(Attribute_A_V_Gemophagia);
	XG_PAWN_VALUE_TO_TAG(Attribute_A_R_Gemophagia);


	XG_PAWN_VALUE_TO_TAG(Attribute_D_V_Base);
	XG_PAWN_VALUE_TO_TAG(Attribute_D_R_Base);
	XG_PAWN_VALUE_TO_TAG(Attribute_D_V_Dodge);
	XG_PAWN_VALUE_TO_TAG(Attribute_D_V_Block);



	for (auto& TmpSet : StatsSets)
	{
		const UGyraStatsSet* Stats = InASC->GetSet<UGyraStatsSet>();

		float NewValue = TmpSet.Value;
		float OldValue = Stats->TagToValue(TmpSet.Key);

		InASC->SetNumericAttributeBase(UGyraStatsSet::TagToAttribute(TmpSet.Key),
			NewValue);

		Stats->BroadcastAttributeChanage(nullptr, nullptr, nullptr, TmpSet.Key,
			0.f, OldValue, NewValue);

	}




}
