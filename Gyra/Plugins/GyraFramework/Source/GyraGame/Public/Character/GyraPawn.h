// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "ModularPawn.h"
#include "GyraTeamAgentInterface.h"

#include "GyraPawn.generated.h"

class AController;
class UObject;
struct FFrame;

/**
 * AGyraPawn
 */
UCLASS()
class GYRAGAME_API AGyraPawn : public AModularPawn, public IGyraTeamAgentInterface
{
	GENERATED_BODY()

public:

	AGyraPawn(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~AActor interface
	virtual void PreInitializeComponents() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~End of AActor interface

	//~APawn interface
	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;
	//~End of APawn interface

	//~IGyraTeamAgentInterface interface
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const override;
	virtual FOnGyraTeamIndexChangedDelegate* GetOnTeamIndexChangedDelegate() override;
	//~End of IGyraTeamAgentInterface interface

protected:
	// Called to determine what happens to the team ID when possession ends
	// 当控球结束时，确定球队ID会发生什么
	virtual FGenericTeamId DetermineNewTeamAfterPossessionEnds(FGenericTeamId OldTeamID) const
	{
		// This could be changed to return, e.g., OldTeamID if you want to keep it assigned afterwards, or return an ID for some neutral faction, or etc...
		// 这可以改变为返回，例如，OldTeamID，如果你想保持它的分配，或者返回一个中立阵营的ID，等等…
		return FGenericTeamId::NoTeam;
	}

private:
	UFUNCTION()
	void OnControllerChangedTeam(UObject* TeamAgent, int32 OldTeam, int32 NewTeam);

private:
	UPROPERTY(ReplicatedUsing = OnRep_MyTeamID)
	FGenericTeamId MyTeamID;

	UPROPERTY()
	FOnGyraTeamIndexChangedDelegate OnTeamChangedDelegate;

private:
	UFUNCTION()
	void OnRep_MyTeamID(FGenericTeamId OldTeamID);
};
