// Copyright 2024 Huagang Liu. All Rights Reserved.

#include "GyraPlayerState.h"

#include "GyraCombatSet.h"
#include "GyraHealthSet.h"

#include "GyraMagicSet.h"

#include "GyraStatsSet.h"


#include "AbilitySystem/GyraAbilitySet.h"
#include "AbilitySystem/GyraAbilitySystemComponent.h"
#include "Character/GyraPawnData.h"
#include "Character/GyraPawnExtensionComponent.h"
#include "Components/GameFrameworkComponentManager.h"
#include "Engine/World.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "GyraExperienceManagerComponent.h"
//@TODO: Would like to isolate this a bit better to get the pawn data in here without this having to know about other stuff
#include "GyraGameMode.h"
#include "LogGyraGame.h"
#include "GyraPlayerController.h"
#include "GyraVerbMessage.h"
#include "Net/UnrealNetwork.h"
#include "GyraArchiveSubsystem.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(GyraPlayerState)

class AController;
class APlayerState;
class FLifetimeProperty;

const FName AGyraPlayerState::NAME_GyraAbilityReady("GyraAbilitiesReady");

void AGyraPlayerState::BeginPlay()
{
	Super::BeginPlay();

	//这里需要确认人物装扮已经生成,且必须先从客户端上传,再到服务端分发.
	//XGTODO:需要优化
	if (GetWorld()->GetNetMode()!=ENetMode::NM_DedicatedServer)
	{
		if (GetWorld()->GetFirstPlayerController() == GetPlayerController())
		{
			UGyraArchiveSubsystem* ArchiveSubsystem = UGyraArchiveSubsystem::GetArchiveSubsystem();

			ClientCallServerUpdateMutableInfo(ArchiveSubsystem->GetMutableInfo());
		}
	}
}

void AGyraPlayerState::ClientCallServerUpdateMutableInfo_Implementation(FGyraMutableInfo InMutableInfo)
{
	MutableInfo = InMutableInfo;

	OnRep_MutableInfo();
}

FGyraMutableInfo AGyraPlayerState::GetMutableInfo()
{
	return MutableInfo;
}

AGyraPlayerState::AGyraPlayerState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, MyPlayerConnectionType(EGyraPlayerConnectionType::Player)
{
	AbilitySystemComponent = ObjectInitializer.CreateDefaultSubobject<UGyraAbilitySystemComponent>(this, TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	// These attribute sets will be detected by AbilitySystemComponent::InitializeComponent. Keeping a reference so that the sets don't get garbage collected before that.
	HealthSet = CreateDefaultSubobject<UGyraHealthSet>(TEXT("HealthSet"));
	CombatSet = CreateDefaultSubobject<UGyraCombatSet>(TEXT("CombatSet"));

	MagicSet = CreateDefaultSubobject<UGyraMagicSet>(TEXT("MagicSet"));

	StatsSet = CreateDefaultSubobject<UGyraStatsSet>(TEXT("StatsSet"));

	// AbilitySystemComponent needs to be updated at a high frequency.
	SetNetUpdateFrequency(100.0f);

	MyTeamID = FGenericTeamId::NoTeam;
	MySquadID = INDEX_NONE;
}

void AGyraPlayerState::PreInitializeComponents()
{
	Super::PreInitializeComponents();
}

void AGyraPlayerState::Reset()
{
	Super::Reset();
}

void AGyraPlayerState::ClientInitialize(AController* C)
{
	Super::ClientInitialize(C);

	if (UGyraPawnExtensionComponent* PawnExtComp = UGyraPawnExtensionComponent::FindPawnExtensionComponent(GetPawn()))
	{
		PawnExtComp->CheckDefaultInitialization();
	}
}

void AGyraPlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);

	//@TODO: Copy stats
}

void AGyraPlayerState::OnDeactivated()
{
	bool bDestroyDeactivatedPlayerState = false;

	switch (GetPlayerConnectionType())
	{
	case EGyraPlayerConnectionType::Player:
	case EGyraPlayerConnectionType::InactivePlayer:
		//@TODO: Ask the experience if we should destroy disconnecting players immediately or leave them around
		// (e.g., for long running servers where they might build up if lots of players cycle through)
		bDestroyDeactivatedPlayerState = true;
		break;
	default:
		bDestroyDeactivatedPlayerState = true;
		break;
	}

	SetPlayerConnectionType(EGyraPlayerConnectionType::InactivePlayer);

	if (bDestroyDeactivatedPlayerState)
	{
		Destroy();
	}
}

void AGyraPlayerState::OnReactivated()
{
	if (GetPlayerConnectionType() == EGyraPlayerConnectionType::InactivePlayer)
	{
		SetPlayerConnectionType(EGyraPlayerConnectionType::Player);
	}
}

void AGyraPlayerState::OnExperienceLoaded(const UGyraExperienceDefinition* /*CurrentExperience*/)
{
	if (AGyraGameMode* GyraGameMode = GetWorld()->GetAuthGameMode<AGyraGameMode>())
	{
		if (const UGyraPawnData* NewPawnData = GyraGameMode->GetPawnDataForController(GetOwningController()))
		{
			SetPawnData(NewPawnData);
		}
		else
		{
			UE_LOG(LogGyraGame, Error, TEXT("AGyraPlayerState::OnExperienceLoaded(): Unable to find PawnData to initialize player state [%s]!"), *GetNameSafe(this));
		}
	}
}

void AGyraPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams SharedParams;
	SharedParams.bIsPushBased = true;

	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, PawnData, SharedParams);
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, MutableInfo, SharedParams);
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, MyPlayerConnectionType, SharedParams)
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, MyTeamID, SharedParams);
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, MySquadID, SharedParams);

	SharedParams.Condition = ELifetimeCondition::COND_SkipOwner;
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, ReplicatedViewRotation, SharedParams);

	DOREPLIFETIME(ThisClass, StatTags);
}

FRotator AGyraPlayerState::GetReplicatedViewRotation() const
{
	// Could replace this with custom replication
	return ReplicatedViewRotation;
}

void AGyraPlayerState::SetReplicatedViewRotation(const FRotator& NewRotation)
{
	if (NewRotation != ReplicatedViewRotation)
	{
		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, ReplicatedViewRotation, this);
		ReplicatedViewRotation = NewRotation;
	}
}

AGyraPlayerController* AGyraPlayerState::GetGyraPlayerController() const
{
	return Cast<AGyraPlayerController>(GetOwner());
}

UAbilitySystemComponent* AGyraPlayerState::GetAbilitySystemComponent() const
{
	return GetGyraAbilitySystemComponent();
}

void AGyraPlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	check(AbilitySystemComponent);
	AbilitySystemComponent->InitAbilityActorInfo(this, GetPawn());

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

void AGyraPlayerState::SetPawnData(const UGyraPawnData* InPawnData)
{
	check(InPawnData);

	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	if (PawnData)
	{
		UE_LOG(LogGyraGame, Error, TEXT("Trying to set PawnData [%s] on player state [%s] that already has valid PawnData [%s]."), *GetNameSafe(InPawnData), *GetNameSafe(this), *GetNameSafe(PawnData));
		return;
	}

	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, PawnData, this);
	PawnData = InPawnData;

	for (const UGyraAbilitySet* AbilitySet : PawnData->AbilitySets)
	{
		if (AbilitySet)
		{
			AbilitySet->GiveToAbilitySystem(AbilitySystemComponent, {},nullptr);
		}
	}

	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(this, NAME_GyraAbilityReady);

	ForceNetUpdate();
}

void AGyraPlayerState::OnRep_PawnData()
{
}

void AGyraPlayerState::SetPlayerConnectionType(EGyraPlayerConnectionType NewType)
{
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, MyPlayerConnectionType, this);
	MyPlayerConnectionType = NewType;
}

void AGyraPlayerState::SetSquadID(int32 NewSquadId)
{
	if (HasAuthority())
	{
		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, MySquadID, this);

		MySquadID = NewSquadId;
	}
}

void AGyraPlayerState::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	if (HasAuthority())
	{
		const FGenericTeamId OldTeamID = MyTeamID;

		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, MyTeamID, this);
		MyTeamID = NewTeamID;
		ConditionalBroadcastTeamChanged(this, OldTeamID, NewTeamID);
	}
	else
	{
		UE_LOG(LogGyraTeams, Error, TEXT("Cannot set team for %s on non-authority"), *GetPathName(this));
	}
}

FGenericTeamId AGyraPlayerState::GetGenericTeamId() const
{
	return MyTeamID;
}

FOnGyraTeamIndexChangedDelegate* AGyraPlayerState::GetOnTeamIndexChangedDelegate()
{
	return &OnTeamChangedDelegate;
}

void AGyraPlayerState::OnRep_MyTeamID(FGenericTeamId OldTeamID)
{
	ConditionalBroadcastTeamChanged(this, OldTeamID, MyTeamID);
}

void AGyraPlayerState::OnRep_MySquadID()
{
	//@TODO: Let the squad subsystem know (once that exists)
}

void AGyraPlayerState::OnRep_MutableInfo()
{
	MutableDelegate.Broadcast();


}

void AGyraPlayerState::AddStatTagStack(FGameplayTag Tag, int32 StackCount)
{
	StatTags.AddStack(Tag, StackCount);
}

void AGyraPlayerState::RemoveStatTagStack(FGameplayTag Tag, int32 StackCount)
{
	StatTags.RemoveStack(Tag, StackCount);
}

int32 AGyraPlayerState::GetStatTagStackCount(FGameplayTag Tag) const
{
	return StatTags.GetStackCount(Tag);
}

bool AGyraPlayerState::HasStatTag(FGameplayTag Tag) const
{
	return StatTags.ContainsTag(Tag);
}

void AGyraPlayerState::ClientBroadcastMessage_Implementation(const FGyraVerbMessage Message)
{
	// This check is needed to prevent running the action when in standalone mode
	if (GetNetMode() == NM_Client)
	{
		UGameplayMessageSubsystem::Get(this).BroadcastMessage(Message.Verb, Message);
	}
}

