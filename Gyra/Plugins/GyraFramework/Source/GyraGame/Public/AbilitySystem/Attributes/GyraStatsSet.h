// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "AbilitySystemComponent.h"
#include "GyraAttributeSet.h"
#include "GameplayTagContainer.h"

#include "GyraStatsSet.generated.h"

class UObject;
struct FFrame;

DECLARE_MULTICAST_DELEGATE_SevenParams(FGyraAttributeEvent_Stats,
	AActor* /*EffectInstigator*/,
	AActor* /*EffectCauser*/,
	const FGameplayEffectSpec* /*EffectSpec*/,
	FGameplayTag /*GameplayTag_Attribute*/,
	float /*EffectMagnitude*/,
	float /*OldValue*/,
	float /*NewValue*/);


/**
 * UGyraStatsSet
 * 定义了人物的状态属性
 * 例如:
 * 体制:
 *		生命:最大生命值,战斗生命值恢复速度,脱战生命值恢复速度
 *		魔法值:最大魔法值,战斗魔法值恢复速度,脱战魔法值恢复速度
 *
 * 
 */
UCLASS(BlueprintType)
class UGyraStatsSet : public UGyraAttributeSet
{
	GENERATED_BODY()

public:

	UGyraStatsSet();


public:

	float TagToValue(const FGameplayTag& InAttributeTag) const;

	static	FGameplayAttribute TagToAttribute(const FGameplayTag& InAttributeTag) ;

	FGameplayTag AttributeToTag(const FGameplayAttribute& InAttribute) const;

	float AttributeToValue(const FGameplayAttribute& InAttribute) const;


	mutable FGyraAttributeEvent_Stats OnChanged_StatsAttribute;

	virtual void BroadcastAttributeChanage(AActor* EffectInstigator,
		AActor* EffectCauser,
		const FGameplayEffectSpec* EffectSpec,
		FGameplayTag GameplayTag_Attribute,
		float EffectMagnitude,
		float OldValue,
		float NewValue) const;

protected:

	virtual bool PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;


#pragma region Status

public:

	ATTRIBUTE_ACCESSORS(UGyraStatsSet, Attribute_S_F_Level);
	ATTRIBUTE_ACCESSORS(UGyraStatsSet, Attribute_S_F_EXP);
	ATTRIBUTE_ACCESSORS(UGyraStatsSet, Attribute_S_F_RewardEXP);
	ATTRIBUTE_ACCESSORS(UGyraStatsSet, Attribute_S_F_MaxEXP);

	ATTRIBUTE_ACCESSORS(UGyraStatsSet, Attribute_S_F_Glod);
	ATTRIBUTE_ACCESSORS(UGyraStatsSet, Attribute_S_F_RewardGlod);


	ATTRIBUTE_ACCESSORS(UGyraStatsSet, Attribute_S_V_MaxHealth);
	ATTRIBUTE_ACCESSORS(UGyraStatsSet, Attribute_S_R_MaxHealth);
	ATTRIBUTE_ACCESSORS(UGyraStatsSet, Attribute_S_V_MaxMana);
	ATTRIBUTE_ACCESSORS(UGyraStatsSet, Attribute_S_R_MaxMana);

	ATTRIBUTE_ACCESSORS(UGyraStatsSet, Attribute_S_V_FightRecoverHealth);
	ATTRIBUTE_ACCESSORS(UGyraStatsSet, Attribute_S_V_OutOfFightRecoverHealth);
	ATTRIBUTE_ACCESSORS(UGyraStatsSet, Attribute_S_V_FightRecoverMana);
	ATTRIBUTE_ACCESSORS(UGyraStatsSet, Attribute_S_V_OutOfFightRecoverMana);

protected:

	UFUNCTION()
	void OnRep_Attribute_S_F_Level(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_Attribute_S_F_EXP(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_Attribute_S_F_RewardEXP(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_Attribute_S_F_MaxEXP(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_Attribute_S_F_Glod(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_Attribute_S_F_RewardGlod(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_Attribute_S_V_MaxHealth(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_Attribute_S_R_MaxHealth(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_Attribute_S_V_MaxMana(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_Attribute_S_R_MaxMana(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_Attribute_S_V_FightRecoverHealth(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_Attribute_S_V_OutOfFightRecoverHealth(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_Attribute_S_V_FightRecoverMana(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_Attribute_S_V_OutOfFightRecoverMana(const FGameplayAttributeData& OldValue);

private:

	//当前等级
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Attribute_S_V_MaxHealth, Category = "Gyra|Stats", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Attribute_S_F_Level;

	//当前经验值
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Attribute_S_V_MaxHealth, Category = "Gyra|Stats", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Attribute_S_F_EXP;

	//死亡奖励的经验值
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Attribute_S_V_MaxHealth, Category = "Gyra|Stats", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Attribute_S_F_RewardEXP;

	//下一级升级的经验值
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Attribute_S_V_MaxHealth, Category = "Gyra|Stats", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Attribute_S_F_MaxEXP;

	//拥有的金币数量
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Attribute_S_V_MaxHealth, Category = "Gyra|Stats", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Attribute_S_F_Glod;

	//奖励的金币数量
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Attribute_S_V_MaxHealth, Category = "Gyra|Stats", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Attribute_S_F_RewardGlod;

	//最大生命值 值加成
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Attribute_S_V_MaxHealth, Category = "Gyra|Stats", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Attribute_S_V_MaxHealth;

	//最大生命值 百分比加成
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Attribute_S_R_MaxHealth, Category = "Gyra|Stats", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Attribute_S_R_MaxHealth;

	//最大魔法值 值加成
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Attribute_S_R_MaxHealth, Category = "Gyra|Stats", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Attribute_S_V_MaxMana;

	//最大魔法值 百分比加成
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Attribute_S_R_MaxMana, Category = "Gyra|Stats", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Attribute_S_R_MaxMana;

	//战斗生命值恢复 值加成
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Attribute_S_V_FightRecoverHealth, Category = "Gyra|Stats", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Attribute_S_V_FightRecoverHealth;

	//脱战生命值恢复 值加成
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Attribute_S_V_OutOfFightRecoverHealth, Category = "Gyra|Stats", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Attribute_S_V_OutOfFightRecoverHealth;

	//战斗魔法值恢复 值加成
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Attribute_S_V_FightRecoverMana, Category = "Gyra|Stats", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Attribute_S_V_FightRecoverMana;

	//脱战魔法值恢复 值加成
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Attribute_S_V_OutOfFightRecoverMana, Category = "Gyra|Stats", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Attribute_S_V_OutOfFightRecoverMana;
#pragma endregion Status

#pragma region Attack

public:

	ATTRIBUTE_ACCESSORS(UGyraStatsSet, Attribute_A_V_Base);
	ATTRIBUTE_ACCESSORS(UGyraStatsSet, Attribute_A_R_Base);
	ATTRIBUTE_ACCESSORS(UGyraStatsSet, Attribute_A_V_CriticalStrikePercentage);
	ATTRIBUTE_ACCESSORS(UGyraStatsSet, Attribute_A_R_CriticalStrike);

	ATTRIBUTE_ACCESSORS(UGyraStatsSet, Attribute_A_R_WeakStrike);
	ATTRIBUTE_ACCESSORS(UGyraStatsSet, Attribute_A_R_HitAgain);
	ATTRIBUTE_ACCESSORS(UGyraStatsSet, Attribute_A_V_Gemophagia);
	ATTRIBUTE_ACCESSORS(UGyraStatsSet, Attribute_A_R_Gemophagia);


protected:

	UFUNCTION()
	void OnRep_Attribute_A_V_Base(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_Attribute_A_R_Base(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_Attribute_A_V_CriticalStrikePercentage(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_Attribute_A_R_CriticalStrike(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_Attribute_A_R_WeakStrike(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_Attribute_A_R_HitAgain(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_Attribute_A_V_Gemophagia(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_Attribute_A_R_Gemophagia(const FGameplayAttributeData& OldValue);

private:

	//基础攻击 值加成
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Attribute_A_V_Base, Category = "Gyra|Stats", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Attribute_A_V_Base;

	//伤害倍率加成 在所有伤害计算完成之后进行 乘法
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Attribute_A_V_Base, Category = "Gyra|Stats", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Attribute_A_R_Base;

	//暴击率 值加成
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Attribute_A_V_CriticalStrikePercentage, Category = "Gyra|Stats", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Attribute_A_V_CriticalStrikePercentage;

	//暴击伤害 倍率加成
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Attribute_A_R_CriticalStrike, Category = "Gyra|Stats", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Attribute_A_R_CriticalStrike;

	//弱点伤害 倍率加成 我方倍率加成
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Attribute_A_R_WeakStrike, Category = "Gyra|Stats", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Attribute_A_R_WeakStrike;

	//闪避后 再次命中率
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Attribute_A_R_HitAgain, Category = "Gyra|Stats", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Attribute_A_R_HitAgain;

	//吸血值 值加成 固定每次命中恢复
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Attribute_A_V_Gemophagia, Category = "Gyra|Stats", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Attribute_A_V_Gemophagia;

	//吸血值 百分比加成 按照实际造成伤害进行恢复
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Attribute_A_R_Gemophagia, Category = "Gyra|Stats", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Attribute_A_R_Gemophagia;

#pragma endregion Attack

#pragma region Defense
public:

	ATTRIBUTE_ACCESSORS(UGyraStatsSet, Attribute_D_V_Base);
	ATTRIBUTE_ACCESSORS(UGyraStatsSet, Attribute_D_R_Base);
	ATTRIBUTE_ACCESSORS(UGyraStatsSet, Attribute_D_V_Dodge);
	ATTRIBUTE_ACCESSORS(UGyraStatsSet, Attribute_D_V_Block);

protected:

	UFUNCTION()
	void OnRep_Attribute_D_V_Base(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_Attribute_D_R_Base(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_Attribute_D_V_Dodge(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_Attribute_D_V_Block(const FGameplayAttributeData& OldValue);

private:

	//基础物理防御 值加成 在所有伤害增幅计算完成后扣减
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Attribute_D_V_Base, Category = "Gyra|Stats", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Attribute_D_V_Base;

	//伤害减少倍率倍率 在防御值扣减后 乘入
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Attribute_D_R_Base, Category = "Gyra|Stats", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Attribute_D_R_Base;

	//闪避率 值加成
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Attribute_D_V_Dodge, Category = "Gyra|Stats", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Attribute_D_V_Dodge;


	//格挡值 值加成 在所有伤害计算完成最后进行格挡
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Attribute_D_V_Block, Category = "Gyra|Stats", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Attribute_D_V_Block;

#pragma endregion Defense

};

