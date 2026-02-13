// Copyright 2024 Huagang Liu. All Rights Reserved.

#include "GyraMonsterController.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Engine/World.h"
#include "GameFramework/PlayerState.h"
#include "LogGyraGame.h"
#include "Perception/AIPerceptionComponent.h"
#include "Net/UnrealNetwork.h"
#include "GyraPawnExtensionComponent.h"
#include "GyraExperienceManagerComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GyraMonsterController)

class UObject;

AGyraMonsterController::AGyraMonsterController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bWantsPlayerState = false;
	bStopAILogicOnUnposses = true;
}



void AGyraMonsterController::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	if (HasAuthority())
	{
		const FGenericTeamId OldTeamID = MyTeamID;
		MyTeamID = NewTeamID;
		ConditionalBroadcastTeamChanged(this, OldTeamID, MyTeamID);
	}
	else
	{
		UE_LOG(LogGyraTeams, Error, TEXT("You can't set the team ID on a character (%s) except on the authority"), *GetPathNameSafe(this));
	}
}

FGenericTeamId AGyraMonsterController::GetGenericTeamId() const
{
	if (IGyraTeamAgentInterface* PSWithTeamInterface = Cast<IGyraTeamAgentInterface>(PlayerState))
	{
		return PSWithTeamInterface->GetGenericTeamId();
	}
	return FGenericTeamId::NoTeam;
}

FOnGyraTeamIndexChangedDelegate* AGyraMonsterController::GetOnTeamIndexChangedDelegate()
{
	return &OnTeamChangedDelegate;
}

ETeamAttitude::Type AGyraMonsterController::GetTeamAttitudeTowards(const AActor& Other) const
{
	if (const APawn* OtherPawn = Cast<APawn>(&Other)) {

		if (const IGyraTeamAgentInterface* TeamAgent = Cast<IGyraTeamAgentInterface>(OtherPawn->GetController()))
		{
			FGenericTeamId OtherTeamID = TeamAgent->GetGenericTeamId();

			//Checking Other pawn ID to define Attitude
			if (OtherTeamID.GetId() != GetGenericTeamId().GetId())
			{
				return ETeamAttitude::Hostile;
			}
			else
			{
				return ETeamAttitude::Friendly;
			}
		}
	}

	return ETeamAttitude::Neutral;
}

void AGyraMonsterController::UpdateTeamAttitude(UAIPerceptionComponent* AIPerception)
{
	if (AIPerception)
	{
		AIPerception->RequestStimuliListenerUpdate();
	}
}

void AGyraMonsterController::OnPossess(APawn* InPawn)
{

	Super::OnPossess(InPawn);
	//暂时设置为队伍1
	SetGenericTeamId(1);



	UWorld* World = GetWorld();
	if (World && World->IsGameWorld() && World->GetNetMode() != NM_Client)
	{
		AGameStateBase* GameState = GetWorld()->GetGameState();
		check(GameState);
		UGyraExperienceManagerComponent* ExperienceComponent = GameState->FindComponentByClass<UGyraExperienceManagerComponent>();
		check(ExperienceComponent);
		ExperienceComponent->CallOrRegister_OnExperienceLoaded(FOnGyraExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded));
	}

	
	
	
}

void AGyraMonsterController::OnUnPossess()
{
	// Make sure the pawn that is being unpossessed doesn't remain our ASC's avatar actor
	if (APawn* PawnBeingUnpossessed = GetPawn())
	{
		if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(PlayerState))
		{
			if (ASC->GetAvatarActor() == PawnBeingUnpossessed)
			{
				ASC->SetAvatarActor(nullptr);
			}
		}
	}

	Super::OnUnPossess();
}

void AGyraMonsterController::OnExperienceLoaded(const UGyraExperienceDefinition* CurrentExperience)
{
	SetMonsetData(GetPawn());
}

void AGyraMonsterController::SetMonsetData(APawn* InPawn)
{
	if (UGyraPawnExtensionComponent* PawnExtComp = UGyraPawnExtensionComponent::FindPawnExtensionComponent(InPawn))
	{
		PawnExtComp->SetPawnData(MonsterData);
	}
}

void AGyraMonsterController::OnRep_MonsterData()
{

}

void AGyraMonsterController::OnRep_MyTeamID(FGenericTeamId OldTeamID)
{
	ConditionalBroadcastTeamChanged(this, OldTeamID, MyTeamID);
}

void AGyraMonsterController::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams SharedParams;
	SharedParams.bIsPushBased = true;


	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, MonsterData, SharedParams);

	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, MyTeamID, SharedParams);


}
