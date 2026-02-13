// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "Camera/GyraCameraAssistInterface.h"
#include "CommonPlayerController.h"
#include "GyraTeamAgentInterface.h"

#include "GyraPlayerController.generated.h"

struct FGenericTeamId;

class AGyraHUD;
class AGyraPlayerState;
class APawn;
class APlayerState;
class FPrimitiveComponentId;
class IInputInterface;
class UGyraAbilitySystemComponent;
class UGyraSettingsShared;
class UObject;
class UPlayer;
struct FFrame;

/**
 * AGyraPlayerController
 *
 *	The base player controller class used by this project.
 */
UCLASS(Config = Game, Meta = (ShortTooltip = "The base player controller class used by this project."))
class GYRAGAME_API AGyraPlayerController : public ACommonPlayerController, public IGyraCameraAssistInterface, public IGyraTeamAgentInterface
{
	GENERATED_BODY()

public:

	AGyraPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category = "Gyra|PlayerController")
	AGyraPlayerState* GetGyraPlayerState() const;

	UFUNCTION(BlueprintCallable, Category = "Gyra|PlayerController")
	UGyraAbilitySystemComponent* GetGyraAbilitySystemComponent() const;

	UFUNCTION(BlueprintCallable, Category = "Gyra|PlayerController")
	AGyraHUD* GetGyraHUD() const;

	// Run a cheat command on the server.
	UFUNCTION(Reliable, Server, WithValidation)
	void ServerCheat(const FString& Msg);

	// Run a cheat command on the server for all players.
	UFUNCTION(Reliable, Server, WithValidation)
	void ServerCheatAll(const FString& Msg);

	//~AActor interface
	virtual void PreInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	//~End of AActor interface

	//~AController interface
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
	virtual void InitPlayerState() override;
	virtual void CleanupPlayerState() override;
	virtual void OnRep_PlayerState() override;
	//~End of AController interface

	//~APlayerController interface
	virtual void ReceivedPlayer() override;
	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetPlayer(UPlayer* InPlayer) override;
	virtual void AddCheats(bool bForce) override;
	virtual void UpdateForceFeedback(IInputInterface* InputInterface, const int32 ControllerId) override;
	virtual void UpdateHiddenComponents(const FVector& ViewLocation, TSet<FPrimitiveComponentId>& OutHiddenComponents) override;
	virtual void PreProcessInput(const float DeltaTime, const bool bGamePaused) override;
	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;
	//~End of APlayerController interface

	//~IGyraCameraAssistInterface interface
	virtual void OnCameraPenetratingTarget() override;
	//~End of IGyraCameraAssistInterface interface

	//~IGyraTeamAgentInterface interface
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const override;
	virtual FOnGyraTeamIndexChangedDelegate* GetOnTeamIndexChangedDelegate() override;
	//~End of IGyraTeamAgentInterface interface

	UFUNCTION(BlueprintCallable, Category = "Gyra|Character")
	void SetIsAutoRunning(const bool bEnabled);

	UFUNCTION(BlueprintCallable, Category = "Gyra|Character")
	bool GetIsAutoRunning() const;

private:
	UPROPERTY()
	FOnGyraTeamIndexChangedDelegate OnTeamChangedDelegate;

	UPROPERTY()
	TObjectPtr<APlayerState> LastSeenPlayerState;

private:
	UFUNCTION()
	void OnPlayerStateChangedTeam(UObject* TeamAgent, int32 OldTeam, int32 NewTeam);

protected:
	// Called when the player state is set or cleared
	virtual void OnPlayerStateChanged();

private:
	void BroadcastOnPlayerStateChanged();

protected:

	//~APlayerController interface

	//~End of APlayerController interface

	void OnSettingsChanged(UGyraSettingsShared* Settings);

	void OnStartAutoRun();
	void OnEndAutoRun();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnStartAutoRun"))
	void K2_OnStartAutoRun();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnEndAutoRun"))
	void K2_OnEndAutoRun();

	bool bHideViewTargetPawnNextFrame = false;
};


