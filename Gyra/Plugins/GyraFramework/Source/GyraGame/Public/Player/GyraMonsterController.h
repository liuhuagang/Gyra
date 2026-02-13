// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "ModularAIController.h"
#include "GyraTeamAgentInterface.h"

#include "GyraMonsterController.generated.h"

namespace ETeamAttitude { enum Type : int; }
struct FGenericTeamId;

class APlayerState;
class UAIPerceptionComponent;
class UObject;
struct FFrame;
class UGyraMonsterData;
/**
 * AGyraMonsterController
 *
 *	这个是僵尸的AI控制器.暂时用于所有的小兵AI控制器
 *
 */
UCLASS(Blueprintable)
class AGyraMonsterController : public AModularAIController, public IGyraTeamAgentInterface
{
	GENERATED_BODY()

public:
	AGyraMonsterController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~IGyraTeamAgentInterface interface
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const override;
	virtual FOnGyraTeamIndexChangedDelegate* GetOnTeamIndexChangedDelegate() override;
	ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;
	//~End of IGyraTeamAgentInterface interface

	//Update Team Attitude for the AI
	UFUNCTION(BlueprintCallable, Category = "Gyra AI Player Controller")
	void UpdateTeamAttitude(UAIPerceptionComponent* AIPerception);

	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;



protected:


private:

	void OnExperienceLoaded(const class UGyraExperienceDefinition* CurrentExperience);


	void SetMonsetData(APawn* InPawn);


	UFUNCTION()
	void OnRep_MonsterData();

protected:

	UPROPERTY(EditDefaultsOnly,ReplicatedUsing = OnRep_MonsterData)
	TObjectPtr<const class UGyraPawnData> MonsterData;

private:

	UPROPERTY()
	FOnGyraTeamIndexChangedDelegate OnTeamChangedDelegate;

	UPROPERTY(ReplicatedUsing = OnRep_MyTeamID)
	FGenericTeamId MyTeamID;

private:

	UFUNCTION()
	void OnRep_MyTeamID(FGenericTeamId OldTeamID);
};
