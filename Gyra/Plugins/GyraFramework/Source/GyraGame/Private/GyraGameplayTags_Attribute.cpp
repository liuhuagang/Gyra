// Copyright 2024 Huagang Liu. All Rights Reserved.

#include "GyraGameplayTags_Attribute.h"

#include "Engine/EngineTypes.h"
#include "GameplayTagsManager.h"
#include "LogGyraGame.h"


namespace GyraGameplayTags
{
#pragma region Status


	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_S_F_Level, "Attribute.S.F.Level", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SetByCaller_Attribute_S_F_Level, "SetByCaller.Attribute.S.F.Level", "");


	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_S_F_EXP, "Attribute.S.F.EXP", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SetByCaller_Attribute_S_F_EXP, "SetByCaller.Attribute.S.F.EXP", "");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_S_F_RewardEXP, "Attribute.S.F.RewardEXP", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SetByCaller_Attribute_S_F_RewardEXP, "SetByCaller.Attribute.S.F.RewardEXP", "");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_S_F_MaxEXP, "Attribute.S.F.MaxEXP", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SetByCaller_Attribute_S_F_MaxEXP, "SetByCaller.Attribute.S.F.MaxEXP", "");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_S_F_Glod, "Attribute.S.F.Glod", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SetByCaller_Attribute_S_F_Glod, "SetByCaller.Attribute.S.F.Glod", "");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_S_F_RewardGlod, "Attribute.S.F.RewardGlod", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SetByCaller_Attribute_S_F_RewardGlod, "SetByCaller.Attribute.S.F.RewardGlod", "");


	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_S_V_MaxHealth, "Attribute.S.V.MaxHealth", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SetByCaller_Attribute_S_V_MaxHealth, "SetByCaller.Attribute.S.V.MaxHealth", "");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_S_R_MaxHealth, "Attribute.S.R.MaxHealth", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SetByCaller_Attribute_S_R_MaxHealth, "SetByCaller.Attribute.S.R.MaxHealth", "");


	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_S_V_MaxMana, "Attribute.S.V.MaxMana", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SetByCaller_Attribute_S_V_MaxMana, "SetByCaller.Attribute.S.V.MaxMana", "");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_S_R_MaxMana, "Attribute.S.R.MaxMana", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SetByCaller_Attribute_S_R_MaxMana, "SetByCaller.Attribute.S.R.MaxMana", "");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_S_V_FightRecoverHealth, "Attribute.S.V.FightRecoverHealth", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SetByCaller_Attribute_S_V_FightRecoverHealth, "SetByCaller.Attribute.S.V.FightRecoverHealth", "");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_S_V_OutOfFightRecoverHealth, "Attribute.S.V.OutOfFightRecoverHealth", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SetByCaller_Attribute_S_V_OutOfFightRecoverHealth, "SetByCaller.Attribute.S.V.OutOfFightRecoverHealth", "");


	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_S_V_FightRecoverMana, "Attribute.S.V.FightRecoverMana", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SetByCaller_Attribute_S_V_FightRecoverMana, "SetByCaller.Attribute.S.V.FightRecoverMana", "");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_S_V_OutOfFightRecoverMana, "Attribute.S.V.OutOfFightRecoverMana", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SetByCaller_Attribute_S_V_OutOfFightRecoverMana, "SetByCaller.Attribute.S.V.OutOfFightRecoverMana", "");


#pragma endregion Status


#pragma region Attack

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_A_V_Base, "Attribute.A.V.Base", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SetByCaller_Attribute_A_V_Base, "SetByCaller.Attribute.A.V.Base", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SetByCaller_Fixed_Attribute_A_V_Base, "SetByCaller.Fixed.Attribute.A.V.Base", "");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_A_R_Base, "Attribute.A.R.Base", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SetByCaller_Attribute_A_R_Base, "SetByCaller.Attribute.A.R.Base", "");


	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_A_V_CriticalStrikePercentage, "Attribute.A.V.CriticalStrikePercentage", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SetByCaller_Attribute_A_V_CriticalStrikePercentage, "SetByCaller.Attribute.A.V.CriticalStrikePercentage", "");


	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_A_R_CriticalStrike, "Attribute.A.R.CriticalStrike", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SetByCaller_Attribute_A_R_CriticalStrike, "SetByCaller.Attribute.A.R.CriticalStrike", "");


	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_A_R_WeakStrike, "Attribute.A.R.WeakStrike", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SetByCaller_Attribute_A_R_WeakStrike, "SetByCaller.Attribute.A.R.WeakStrike", "");


	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_A_R_HitAgain, "Attribute.A.R.HitAgain", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SetByCaller_Attribute_A_R_HitAgain, "SetByCaller.Attribute.A.R.HitAgain", "");


	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_A_V_Gemophagia, "Attribute.A.V.Gemophagia", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SetByCaller_Attribute_A_V_Gemophagia, "SetByCaller.Attribute.A.V.Gemophagia", "");


	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_A_R_Gemophagia, "Attribute.A.R.Gemophagia", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SetByCaller_Attribute_A_R_Gemophagia, "SetByCaller.Attribute.A.R.Gemophagia", "");





#pragma endregion Attack


#pragma region Defense

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_D_V_Base, "Attribute.D.V.Base", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SetByCaller_Attribute_D_V_Base, "SetByCaller.Attribute.D.V.Base", "");


	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_D_R_Base, "Attribute.D.R.Base", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SetByCaller_Attribute_D_R_Base, "SetByCaller.Attribute.D.R.Base", "");


	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_D_V_Dodge, "Attribute.D.V.Dodge", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SetByCaller_Attribute_D_V_Dodge, "SetByCaller.Attribute.D.V.Dodge", "");


	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_D_V_Block, "Attribute.D.V.Block", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SetByCaller_Attribute_D_V_Block, "SetByCaller.Attribute.D.V.Block", "");



#pragma endregion Defense


}


