// Copyright 2025 Huagang Liu. All Rights Reserved.

#include "LogGyraGame.h"

#include "GameFramework/Actor.h"

DEFINE_LOG_CATEGORY(LogGyraGame);
DEFINE_LOG_CATEGORY(LogGyraExperience);
DEFINE_LOG_CATEGORY(LogGyraAbilitySystem);
DEFINE_LOG_CATEGORY(LogGyraTeams);

FString GetClientServerContextString(UObject* ContextObject)
{
	ENetRole Role = ROLE_None;

	if (AActor* Actor = Cast<AActor>(ContextObject))
	{
		Role = Actor->GetLocalRole();
	}
	else if (UActorComponent* Component = Cast<UActorComponent>(ContextObject))
	{
		Role = Component->GetOwnerRole();
	}

	if (Role != ROLE_None)
	{
		return (Role == ROLE_Authority) ? TEXT("Server") : TEXT("Client");
	}
	else
	{
#if WITH_EDITOR
		if (GIsEditor)
		{
			extern ENGINE_API FString GPlayInEditorContextString;
			return GPlayInEditorContextString;
		}
#endif
	}

	return TEXT("[]");
}
