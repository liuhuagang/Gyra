// Copyright 2024 Huagang Liu. All Rights Reserved.

#include "GyraTeamPublicInfo.h"

#include "Net/UnrealNetwork.h"
#include "GyraTeamInfoBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GyraTeamPublicInfo)

class FLifetimeProperty;

AGyraTeamPublicInfo::AGyraTeamPublicInfo(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void AGyraTeamPublicInfo::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ThisClass, TeamDisplayAsset, COND_InitialOnly);
}

void AGyraTeamPublicInfo::SetTeamDisplayAsset(TObjectPtr<UGyraTeamDisplayAsset> NewDisplayAsset)
{
	check(HasAuthority());
	check(TeamDisplayAsset == nullptr);

	TeamDisplayAsset = NewDisplayAsset;

	TryRegisterWithTeamSubsystem();
}

void AGyraTeamPublicInfo::OnRep_TeamDisplayAsset()
{
	TryRegisterWithTeamSubsystem();
}

