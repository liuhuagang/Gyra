// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "GameFramework/Info.h"
#include "GameplayTagStack.h"

#include "GyraTeamInfoBase.generated.h"

namespace EEndPlayReason { enum Type : int; }

class UGyraTeamCreationComponent;
class UGyraTeamSubsystem;
class UObject;
struct FFrame;

UCLASS(Abstract)
class AGyraTeamInfoBase : public AInfo
{
	GENERATED_BODY()

public:
	AGyraTeamInfoBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	int32 GetTeamId() const { return TeamId; }

	//~AActor interface
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~End of AActor interface

protected:
	virtual void RegisterWithTeamSubsystem(UGyraTeamSubsystem* Subsystem);
	void TryRegisterWithTeamSubsystem();

private:
	void SetTeamId(int32 NewTeamId);

	UFUNCTION()
	void OnRep_TeamId();

public:
	friend UGyraTeamCreationComponent;

	UPROPERTY(Replicated)
	FGameplayTagStackContainer TeamTags;

private:
	UPROPERTY(ReplicatedUsing = OnRep_TeamId)
	int32 TeamId;
};
