// Copyright 2024 Huagang Liu. All Rights Reserved.

#include "GyraTeamStatics.h"

#include "Engine/Engine.h"
#include "Engine/World.h"

#include "LogGyraGame.h"
#include "GyraTeamDisplayAsset.h"
#include "GyraTeamSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GyraTeamStatics)

class UTexture;

//////////////////////////////////////////////////////////////////////

void UGyraTeamStatics::FindTeamFromObject(const UObject* Agent,
bool& bIsPartOfTeam, 
int32& TeamId, 
UGyraTeamDisplayAsset*& DisplayAsset,
bool bLogIfNotSet)
{
	bIsPartOfTeam = false;
	TeamId = INDEX_NONE;
	DisplayAsset = nullptr;

	if (UWorld* World = GEngine->GetWorldFromContextObject(Agent, EGetWorldErrorMode::LogAndReturnNull))
	{
		if (UGyraTeamSubsystem* TeamSubsystem = World->GetSubsystem<UGyraTeamSubsystem>())
		{
			TeamId = TeamSubsystem->FindTeamFromObject(Agent);
			if (TeamId != INDEX_NONE)
			{
				bIsPartOfTeam = true;

				DisplayAsset = TeamSubsystem->GetTeamDisplayAsset(TeamId, INDEX_NONE);

				if ((DisplayAsset == nullptr) && bLogIfNotSet)
				{
					UE_LOG(LogGyraTeams, Log, TEXT("FindTeamFromObject(%s) called too early (found team %d but no display asset set yet"), *GetPathNameSafe(Agent), TeamId);
				}
			}
		}
		else
		{
			UE_LOG(LogGyraTeams, Error, TEXT("FindTeamFromObject(%s) failed: Team subsystem does not exist yet"), *GetPathNameSafe(Agent));
		}
	}
}

UGyraTeamDisplayAsset* UGyraTeamStatics::GetTeamDisplayAsset(const UObject* WorldContextObject, int32 TeamId)
{
	UGyraTeamDisplayAsset* Result = nullptr;
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		if (UGyraTeamSubsystem* TeamSubsystem = World->GetSubsystem<UGyraTeamSubsystem>())
		{
			return TeamSubsystem->GetTeamDisplayAsset(TeamId, INDEX_NONE);
		}
	}
	return Result;
}

float UGyraTeamStatics::GetTeamScalarWithFallback(UGyraTeamDisplayAsset* DisplayAsset, FName ParameterName, float DefaultValue)
{
	if (DisplayAsset)
	{
		if (float* pValue = DisplayAsset->ScalarParameters.Find(ParameterName))
		{
			return *pValue;
		}
	}
	return DefaultValue;
}

FLinearColor UGyraTeamStatics::GetTeamColorWithFallback(UGyraTeamDisplayAsset* DisplayAsset, FName ParameterName, FLinearColor DefaultValue)
{
	if (DisplayAsset)
	{
		if (FLinearColor* pColor = DisplayAsset->ColorParameters.Find(ParameterName))
		{
			return *pColor;
		}
	}
	return DefaultValue;
}

UTexture* UGyraTeamStatics::GetTeamTextureWithFallback(UGyraTeamDisplayAsset* DisplayAsset, FName ParameterName, UTexture* DefaultValue)
{
	if (DisplayAsset)
	{
		if (TObjectPtr<UTexture>* pTexture = DisplayAsset->TextureParameters.Find(ParameterName))
		{
			return *pTexture;
		}
	}
	return DefaultValue;
}

