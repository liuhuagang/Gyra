// Copyright 2024 Huagang Liu. All Rights Reserved.

#include "GyraTeamCreationComponent.h"

#include "Engine/World.h"

#include "GyraExperienceManagerComponent.h"
#include "GyraTeamPublicInfo.h"
#include "GyraTeamPrivateInfo.h"
#include "GyraPlayerState.h"
#include "GyraGameMode.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

#include UE_INLINE_GENERATED_CPP_BY_NAME(GyraTeamCreationComponent)

UGyraTeamCreationComponent::UGyraTeamCreationComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PublicTeamInfoClass = AGyraTeamPublicInfo::StaticClass();
	PrivateTeamInfoClass = AGyraTeamPrivateInfo::StaticClass();
}

#if WITH_EDITOR
EDataValidationResult UGyraTeamCreationComponent::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	//@TODO: TEAMS: Validate that all display assets have the same properties set!
	//@TODO: TEAMS: 验证所有显示资源都有相同的属性设置！

	return Result;
}
#endif

void UGyraTeamCreationComponent::BeginPlay()
{
	Super::BeginPlay();

	// Listen for the experience load to complete
	AGameStateBase* GameState = GetGameStateChecked<AGameStateBase>();
	UGyraExperienceManagerComponent* ExperienceComponent = GameState->FindComponentByClass<UGyraExperienceManagerComponent>();
	check(ExperienceComponent);
	ExperienceComponent->CallOrRegister_OnExperienceLoaded_HighPriority(FOnGyraExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded));
}

void UGyraTeamCreationComponent::OnExperienceLoaded(const UGyraExperienceDefinition* Experience)
{
#if WITH_SERVER_CODE
	if (HasAuthority())
	{
		ServerCreateTeams();
		ServerAssignPlayersToTeams();
	}
#endif
}


#if WITH_SERVER_CODE

void UGyraTeamCreationComponent::ServerCreateTeams()
{
	for (const auto& KVP : TeamsToCreate)
	{
		const int32 TeamId = KVP.Key;
		ServerCreateTeam(TeamId, KVP.Value);
	}
}

void UGyraTeamCreationComponent::ServerAssignPlayersToTeams()
{
	// Assign players that already exist to teams
	AGameStateBase* GameState = GetGameStateChecked<AGameStateBase>();
	for (APlayerState* PS : GameState->PlayerArray)
	{
		if (AGyraPlayerState* GyraPS = Cast<AGyraPlayerState>(PS))
		{
			ServerChooseTeamForPlayer(GyraPS);
		}
	}

	// Listen for new players logging in
	AGyraGameMode* GameMode = Cast<AGyraGameMode>(GameState->AuthorityGameMode);
	check(GameMode);

	GameMode->OnGameModePlayerInitialized.AddUObject(this, &ThisClass::OnPlayerInitialized);
}

void UGyraTeamCreationComponent::ServerChooseTeamForPlayer(AGyraPlayerState* PS)
{
	if (PS->IsOnlyASpectator())
	{
		PS->SetGenericTeamId(FGenericTeamId::NoTeam);
	}
	else
	{	//这里可以强制所有玩家为队伍2包括机器人,怪物为队伍1,在蓝图开启
		if (bForceAllPlayerInTeamTwo)
		{
			const FGenericTeamId SamePlayerTeamID = 2;

			PS->SetGenericTeamId(SamePlayerTeamID);
		}
		else
		{
			const FGenericTeamId TeamID = IntegerToGenericTeamId(GetLeastPopulatedTeamID());

			PS->SetGenericTeamId(TeamID);
		}
	
	}
}

void UGyraTeamCreationComponent::OnPlayerInitialized(AGameModeBase* GameMode, AController* NewPlayer)
{
	check(NewPlayer);
	check(NewPlayer->PlayerState);
	if (AGyraPlayerState* GyraPS = Cast<AGyraPlayerState>(NewPlayer->PlayerState))
	{
		ServerChooseTeamForPlayer(GyraPS);
	}
}

void UGyraTeamCreationComponent::ServerCreateTeam(int32 TeamId, UGyraTeamDisplayAsset* DisplayAsset)
{
	check(HasAuthority());

	//@TODO: ensure the team doesn't already exist

	UWorld* World = GetWorld();
	check(World);

	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AGyraTeamPublicInfo* NewTeamPublicInfo = World->SpawnActor<AGyraTeamPublicInfo>(PublicTeamInfoClass, SpawnInfo);
	checkf(NewTeamPublicInfo != nullptr, TEXT("Failed to create public team actor from class %s"), *GetPathNameSafe(*PublicTeamInfoClass));
	NewTeamPublicInfo->SetTeamId(TeamId);
	NewTeamPublicInfo->SetTeamDisplayAsset(DisplayAsset);

	AGyraTeamPrivateInfo* NewTeamPrivateInfo = World->SpawnActor<AGyraTeamPrivateInfo>(PrivateTeamInfoClass, SpawnInfo);
	checkf(NewTeamPrivateInfo != nullptr, TEXT("Failed to create private team actor from class %s"), *GetPathNameSafe(*PrivateTeamInfoClass));
	NewTeamPrivateInfo->SetTeamId(TeamId);
}

int32 UGyraTeamCreationComponent::GetLeastPopulatedTeamID() const
{
	const int32 NumTeams = TeamsToCreate.Num();
	if (NumTeams > 0)
	{
		TMap<int32, uint32> TeamMemberCounts;
		TeamMemberCounts.Reserve(NumTeams);

		for (const auto& KVP : TeamsToCreate)
		{
			const int32 TeamId = KVP.Key;
			TeamMemberCounts.Add(TeamId, 0);
		}

		AGameStateBase* GameState = GetGameStateChecked<AGameStateBase>();
		for (APlayerState* PS : GameState->PlayerArray)
		{
			if (AGyraPlayerState* GyraPS = Cast<AGyraPlayerState>(PS))
			{
				const int32 PlayerTeamID = GyraPS->GetTeamId();

				if ((PlayerTeamID != INDEX_NONE) && !GyraPS->IsInactive())	// do not count unassigned or disconnected players
				{
					check(TeamMemberCounts.Contains(PlayerTeamID))
						TeamMemberCounts[PlayerTeamID] += 1;
				}
			}
		}

		// sort by lowest team population, then by team ID
		int32 BestTeamId = INDEX_NONE;
		uint32 BestPlayerCount = TNumericLimits<uint32>::Max();
		for (const auto& KVP : TeamMemberCounts)
		{
			const int32 TestTeamId = KVP.Key;
			const uint32 TestTeamPlayerCount = KVP.Value;

			if (TestTeamPlayerCount < BestPlayerCount)
			{
				BestTeamId = TestTeamId;
				BestPlayerCount = TestTeamPlayerCount;
			}
			else if (TestTeamPlayerCount == BestPlayerCount)
			{
				if ((TestTeamId < BestTeamId) || (BestTeamId == INDEX_NONE))
				{
					BestTeamId = TestTeamId;
					BestPlayerCount = TestTeamPlayerCount;
				}
			}
		}

		return BestTeamId;
	}

	return INDEX_NONE;
}
#endif	// WITH_SERVER_CODE

