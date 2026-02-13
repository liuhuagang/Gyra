// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "GyraTeamStatics.generated.h"

class UGyraTeamDisplayAsset;
class UObject;
class UTexture;
struct FFrame;

/** A subsystem for easy access to team information for team-based actors (e.g., pawns or player states) */
/** 一个子系统，可以方便地访问基于团队的参与者（例如，棋子或玩家状态）的团队信息 */
UCLASS()
class UGyraTeamStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	// Returns the team this object belongs to, or INDEX_NONE if it is not part of a team
	// 返回该对象所属的球队，如果不是球队的一部分，则返回INDEX_NONE
	UFUNCTION(BlueprintCallable, Category = Teams, meta = (Keywords = "GetTeamFromObject", DefaultToSelf = "Agent", AdvancedDisplay = "bLogIfNotSet"))
	static void FindTeamFromObject(const UObject* Agent, bool& bIsPartOfTeam, int32& TeamId, UGyraTeamDisplayAsset*& DisplayAsset, bool bLogIfNotSet = false);

	UFUNCTION(BlueprintCallable, Category = Teams, meta = (WorldContext = "WorldContextObject"))
	static UGyraTeamDisplayAsset* GetTeamDisplayAsset(const UObject* WorldContextObject, int32 TeamId);

	UFUNCTION(BlueprintCallable, Category = Teams)
	static float GetTeamScalarWithFallback(UGyraTeamDisplayAsset* DisplayAsset, FName ParameterName, float DefaultValue);

	UFUNCTION(BlueprintCallable, Category = Teams)
	static FLinearColor GetTeamColorWithFallback(UGyraTeamDisplayAsset* DisplayAsset, FName ParameterName, FLinearColor DefaultValue);

	UFUNCTION(BlueprintCallable, Category = Teams)
	static UTexture* GetTeamTextureWithFallback(UGyraTeamDisplayAsset* DisplayAsset, FName ParameterName, UTexture* DefaultValue);
};
