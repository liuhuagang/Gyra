// Copyright 2024 Huagang Liu. All Rights Reserved.

#include "GyraTeamInfoBase.h"

#include "Engine/World.h"
#include "Net/UnrealNetwork.h"

#include "GyraTeamSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GyraTeamInfoBase)

class FLifetimeProperty;

AGyraTeamInfoBase::AGyraTeamInfoBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, TeamId(INDEX_NONE)
{
	bReplicates = true;
	bAlwaysRelevant = true;
	NetPriority = 3.0f;
	SetReplicatingMovement(false);
}



void AGyraTeamInfoBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, TeamTags);
	DOREPLIFETIME_CONDITION(ThisClass, TeamId, COND_InitialOnly);
}

void AGyraTeamInfoBase::BeginPlay()
{
	Super::BeginPlay();

	TryRegisterWithTeamSubsystem();
}

void AGyraTeamInfoBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (TeamId != INDEX_NONE)
	{
		UGyraTeamSubsystem* TeamSubsystem = GetWorld()->GetSubsystem<UGyraTeamSubsystem>();
		if (TeamSubsystem)
		{
			// EndPlay can happen at weird times where the subsystem has already been destroyed
			TeamSubsystem->UnregisterTeamInfo(this);
		}
	}

	Super::EndPlay(EndPlayReason);
}

void AGyraTeamInfoBase::RegisterWithTeamSubsystem(UGyraTeamSubsystem* Subsystem)
{
	Subsystem->RegisterTeamInfo(this);
}

void AGyraTeamInfoBase::TryRegisterWithTeamSubsystem()
{
	if (TeamId != INDEX_NONE)
	{
		UGyraTeamSubsystem* TeamSubsystem = GetWorld()->GetSubsystem<UGyraTeamSubsystem>();
		if (ensure(TeamSubsystem))
		{
			RegisterWithTeamSubsystem(TeamSubsystem);
		}
	}
}

void AGyraTeamInfoBase::SetTeamId(int32 NewTeamId)
{
	check(HasAuthority());
	check(TeamId == INDEX_NONE);
	check(NewTeamId != INDEX_NONE);

	TeamId = NewTeamId;

	TryRegisterWithTeamSubsystem();
}

void AGyraTeamInfoBase::OnRep_TeamId()
{
	TryRegisterWithTeamSubsystem();
}


