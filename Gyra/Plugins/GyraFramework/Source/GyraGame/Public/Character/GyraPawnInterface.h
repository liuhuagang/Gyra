// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GyraPawnInterface.generated.h"


UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UGyraPawnInterface : public UInterface
{
	GENERATED_BODY()
};

class GYRAGAME_API IGyraPawnInterface
{
	GENERATED_BODY()

public:
	
	/**
	 * 获取角色的唯一主键名称
	 * 
	 * @return 
	 */
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Gyra|Pawn")
	virtual FName GetGyraPawnInitInfoName();

};
