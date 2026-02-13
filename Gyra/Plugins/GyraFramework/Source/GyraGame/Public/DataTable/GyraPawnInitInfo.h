// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GyraStatsSet.h"
#include "Engine/DataTable.h"

#include "GyraPawnInitInfo.generated.h"

/**
 * Gyra项目中所有角色的初始化信息.此时未叠加等级成长信息.
 */
USTRUCT(BlueprintType)
struct FGyraPawnInitInfo:public FTableRowBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gyra|Pawn|InitInfo")
	int32 Attribute_S_F_Level = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gyra|Pawn|InitInfo")
	int32 Attribute_S_F_EXP = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gyra|Pawn|InitInfo")
	int32 Attribute_S_F_RewardEXP = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gyra|Pawn|InitInfo")
	int32 Attribute_S_F_MaxEXP = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gyra|Pawn|InitInfo")
	int32 Attribute_S_F_Glod = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gyra|Pawn|InitInfo")
	int32 Attribute_S_F_RewardGlod = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gyra|Pawn|InitInfo")
	int32 Attribute_S_V_MaxHealth = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gyra|Pawn|InitInfo")
	int32 Attribute_S_R_MaxHealth = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gyra|Pawn|InitInfo")
	int32 Attribute_S_V_MaxMana = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gyra|Pawn|InitInfo")
	int32 Attribute_S_R_MaxMana = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gyra|Pawn|InitInfo")
	int32 Attribute_S_V_FightRecoverHealth = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gyra|Pawn|InitInfo")
	int32 Attribute_S_V_OutOfFightRecoverHealth = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gyra|Pawn|InitInfo")
	int32 Attribute_S_V_FightRecoverMana = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gyra|Pawn|InitInfo")
	int32 Attribute_S_V_OutOfFightRecoverMana = 0;




	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gyra|Pawn|InitInfo")
	int32 Attribute_A_V_Base = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gyra|Pawn|InitInfo")
	int32 Attribute_A_R_Base = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gyra|Pawn|InitInfo")
	int32 Attribute_A_V_CriticalStrikePercentage = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gyra|Pawn|InitInfo")
	int32 Attribute_A_R_CriticalStrike = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gyra|Pawn|InitInfo")
	int32 Attribute_A_R_WeakStrike = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gyra|Pawn|InitInfo")
	int32 Attribute_A_R_HitAgain = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gyra|Pawn|InitInfo")
	int32 Attribute_A_V_Gemophagia = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gyra|Pawn|InitInfo")
	int32 Attribute_A_R_Gemophagia = 0;



	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gyra|Pawn|InitInfo")
	int32 Attribute_D_V_Base = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gyra|Pawn|InitInfo")
	int32 Attribute_D_R_Base = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gyra|Pawn|InitInfo")
	int32 Attribute_D_V_Dodge = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gyra|Pawn|InitInfo")
	int32 Attribute_D_V_Block = 0;

	void InitAttributeFromInfo(UGyraAbilitySystemComponent* InASC) const;

};


