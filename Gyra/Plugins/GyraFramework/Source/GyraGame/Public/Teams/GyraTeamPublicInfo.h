// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "GyraTeamInfoBase.h"

#include "GyraTeamPublicInfo.generated.h"

class UGyraTeamCreationComponent;
class UGyraTeamDisplayAsset;
class UObject;
struct FFrame;

UCLASS()
class AGyraTeamPublicInfo : public AGyraTeamInfoBase
{
	GENERATED_BODY()

	friend UGyraTeamCreationComponent;

public:
	AGyraTeamPublicInfo(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UGyraTeamDisplayAsset* GetTeamDisplayAsset() const { return TeamDisplayAsset; }

private:
	UFUNCTION()
	void OnRep_TeamDisplayAsset();

	void SetTeamDisplayAsset(TObjectPtr<UGyraTeamDisplayAsset> NewDisplayAsset);

private:
	UPROPERTY(ReplicatedUsing = OnRep_TeamDisplayAsset)
	TObjectPtr<UGyraTeamDisplayAsset> TeamDisplayAsset;
};
