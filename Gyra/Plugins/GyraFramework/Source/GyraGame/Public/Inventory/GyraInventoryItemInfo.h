// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GyraWorldCollectable.h"
#include "GyraInventoryItemInfo.generated.h"


/**
 *
 * 物品分类的枚举
 *
 */
UENUM(BlueprintType)
enum class EGyraInventoryItemType : uint8
{
	None  UMETA(DisplayName = "None"),
	Equipment  UMETA(DisplayName = "Equipment"),
	Ammo UMETA(DisplayName = "Ammo"),
	Potion UMETA(DisplayName = "Potion"),
	Material UMETA(DisplayName = "Material"),
	Misc UMETA(DisplayName = "Misc"),
	Max UMETA(Hidden)
};

/**
 *
 * 装备分类的枚举
 *
 */
UENUM( BlueprintType)
enum class EGyraEquipmentItemType : uint8
{
	None  = 0 UMETA(DisplayName = "None"),
	Weapon  = 1 UMETA(DisplayName = "Weapon"),
	Head   = 2 UMETA(DisplayName = "Head"),
	Shoulders  =3 UMETA(DisplayName = "Shoulders"),
	Hand  =4 UMETA(DisplayName = "Hand"),
	Upper = 5 UMETA(DisplayName = "Upper"),
	Lower = 6 UMETA(DisplayName = "Lower"),
	Foot  =7 UMETA(DisplayName = "Foot"),
	Max   UMETA(Hidden)
};



/**
 *
 * 物品分类的枚举
 *
 */
UENUM(BlueprintType)
enum class EGyraInventoryItemQuality :uint8
{
	Ordinary UMETA(DisplayName = "Ordinary"),//白色 1 固定词缀1
	Good UMETA(DisplayName = "Good"),//绿色 3		 固定词缀1
	Excellent UMETA(DisplayName = "Excellent"),//  深蓝   5	 固定词缀3
	Perfect UMETA(DisplayName = "Perfect"),//      黄色   7   固定词缀3
	Epic UMETA(DisplayName = "Epic"),//紫色 9       带有套装属性		固定词缀5
	Legend UMETA(DisplayName = "Legend"),//橙色 11  带有额外技能词缀	固定词缀7
	God UMETA(DisplayName = "God"),//红色 13 满属性 带有额外技能词缀	固定词缀9
	Max UMETA(Hidden)
};
