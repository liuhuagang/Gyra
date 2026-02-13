// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once
#include "Components/GameStateComponent.h"

#include "GyraTeamCreationComponent.generated.h"

class UGyraExperienceDefinition;
class AGyraTeamPublicInfo;
class AGyraTeamPrivateInfo;
class AGyraPlayerState;
class AGameModeBase;
class APlayerController;
class UGyraTeamDisplayAsset;



UCLASS(Blueprintable)
class UGyraTeamCreationComponent : public UGameStateComponent
{
	GENERATED_BODY()

public:
	UGyraTeamCreationComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~UObject interface
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif
	//~End of UObject interface

	//~UActorComponent interface
	virtual void BeginPlay() override;
	//~End of UActorComponent interface

private:
	void OnExperienceLoaded(const UGyraExperienceDefinition* Experience);

protected:

	UPROPERTY(EditDefaultsOnly, Category = Teams)
	bool bForceAllPlayerInTeamTwo =false;

	// List of teams to create (id to display asset mapping, the display asset can be left unset if desired)
	//要创建的团队列表（id用于显示资产映射，如果需要，可以不设置显示资产）
	UPROPERTY(EditDefaultsOnly, Category = Teams)
	TMap<uint8, TObjectPtr<UGyraTeamDisplayAsset>> TeamsToCreate;

	UPROPERTY(EditDefaultsOnly, Category = Teams)
	TSubclassOf<AGyraTeamPublicInfo> PublicTeamInfoClass;

	UPROPERTY(EditDefaultsOnly, Category = Teams)
	TSubclassOf<AGyraTeamPrivateInfo> PrivateTeamInfoClass;

#if WITH_SERVER_CODE
protected:
	virtual void ServerCreateTeams();
	virtual void ServerAssignPlayersToTeams();

	/** Sets the team ID for the given player state. Spectator-only player states will be stripped of any team association. */
	virtual void ServerChooseTeamForPlayer(AGyraPlayerState* PS);

private:
	void OnPlayerInitialized(AGameModeBase* GameMode, AController* NewPlayer);
	void ServerCreateTeam(int32 TeamId, UGyraTeamDisplayAsset* DisplayAsset);

	/** returns the Team ID with the fewest active players, or INDEX_NONE if there are no valid teams */
	int32 GetLeastPopulatedTeamID() const;
#endif
};
