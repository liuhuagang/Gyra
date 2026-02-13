// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "NativeGameplayTags.h"

#define XG_DECLARE_DECLARE_GAMEPLAY_TAG_EXTERN(InAttributeTagName)\
GYRAGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InAttributeTagName);\
GYRAGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_##InAttributeTagName);\
GYRAGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Fixed_##InAttributeTagName);

//用于映射属性的标签 属性过多 必须使用Tag
namespace GyraGameplayTags
{

#pragma region Status

	//等级
	XG_DECLARE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_S_F_Level);
	
	//当前的经验值
	XG_DECLARE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_S_F_EXP);

	//击杀奖励经验值
	XG_DECLARE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_S_F_RewardEXP);

	//下一等级经验值
	XG_DECLARE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_S_F_MaxEXP);

	//拥有的金币数量
	XG_DECLARE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_S_F_Glod);

	//奖励的金币数量
	XG_DECLARE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_S_F_RewardGlod);

	//最大生命值 值加成
	XG_DECLARE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_S_V_MaxHealth);
	//最大生命值 百分比加成
	XG_DECLARE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_S_R_MaxHealth);

	//最大魔法值 值加成
	XG_DECLARE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_S_V_MaxMana);
	//最大魔法值 百分比加成
	XG_DECLARE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_S_R_MaxMana);

	//战斗时生命恢复
	XG_DECLARE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_S_V_FightRecoverHealth);
	//脱战时生命恢复
	XG_DECLARE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_S_V_OutOfFightRecoverHealth);
	//战斗时魔法值恢复
	XG_DECLARE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_S_V_FightRecoverMana);
	//脱站时魔法恢复
	XG_DECLARE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_S_V_OutOfFightRecoverMana);

#pragma endregion Status


#pragma region Attack


	//基础攻击 值加成
	XG_DECLARE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_A_V_Base);

	//伤害倍率加成 在所有伤害计算完成之后进行 乘法
	XG_DECLARE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_A_R_Base);

	//暴击率 值加成
	XG_DECLARE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_A_V_CriticalStrikePercentage);

	//暴击伤害 倍率加成
	XG_DECLARE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_A_R_CriticalStrike);

	//弱点伤害 倍率加成 我方倍率加成
	XG_DECLARE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_A_R_WeakStrike);

	//闪避后追加命中率 值加成 按百分比计算再次命中率 不再考虑二次回避
	XG_DECLARE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_A_R_HitAgain);

	//吸血值 值加成 固定每次命中恢复
	XG_DECLARE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_A_V_Gemophagia);

	//吸血值 百分比加成 按照实际造成伤害进行恢复
	XG_DECLARE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_A_R_Gemophagia);

#pragma endregion Attack


#pragma region Defense

	//基础物理防御 值加成 在所有伤害增幅计算完成后扣减
	XG_DECLARE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_D_V_Base);

	//伤害减少倍率倍率 在防御值扣减后 乘入
	XG_DECLARE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_D_R_Base);
	
	//闪避率 值加成
	XG_DECLARE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_D_V_Dodge);

	//格挡值 值加成 在所有伤害计算完成最后进行格挡
	XG_DECLARE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_D_V_Block);


#pragma endregion Defense





};

