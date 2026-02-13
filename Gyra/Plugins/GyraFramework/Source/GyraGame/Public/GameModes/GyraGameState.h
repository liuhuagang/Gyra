// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "AbilitySystemInterface.h"
#include "ModularGameState.h"

#include "GyraGameState.generated.h"

struct FGyraVerbMessage;

class APlayerState;
class UAbilitySystemComponent;
class UGyraAbilitySystemComponent;
class UGyraExperienceManagerComponent;
class UObject;
struct FFrame;

/**
 * AGyraGameState
 *
 *	The base game state class used by this project.
 *	游戏使用的基础GameStateBase
 */
UCLASS(Config = Game)
class GYRAGAME_API AGyraGameState : public AModularGameStateBase, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:

	AGyraGameState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~AActor interface
	virtual void PreInitializeComponents() override;
	virtual void PostInitializeComponents() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaSeconds) override;
	//~End of AActor interface

	//~AGameStateBase interface
	virtual void AddPlayerState(APlayerState* PlayerState) override;
	virtual void RemovePlayerState(APlayerState* PlayerState) override;
	virtual void SeamlessTravelTransitionCheckpoint(bool bToTransitionMap) override;
	//~End of AGameStateBase interface

	//~IAbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	//~End of IAbilitySystemInterface

	// Gets the ability system component used for game wide things
	UFUNCTION(BlueprintCallable, Category = "Gyra|GameState")
	UGyraAbilitySystemComponent* GetGyraAbilitySystemComponent() const { return AbilitySystemComponent; }

	// Send a message that all clients will (probably) get
	// (use only for client notifications like eliminations, server join messages, etc... that can handle being lost)
	UFUNCTION(NetMulticast, Unreliable, BlueprintCallable, Category = "Gyra|GameState")
	void MulticastMessageToClients(const FGyraVerbMessage Message);

	// Send a message that all clients will be guaranteed to get
	// (use only for client notifications that cannot handle being lost)
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category = "Gyra|GameState")
	void MulticastReliableMessageToClients(const FGyraVerbMessage Message);

	// Gets the server's FPS, replicated to clients
	float GetServerFPS() const;

private:
	// Handles loading and managing the current gameplay experience
	UPROPERTY()
	TObjectPtr<UGyraExperienceManagerComponent> ExperienceManagerComponent;

	// The ability system component subobject for game-wide things (primarily gameplay cues)
	UPROPERTY(VisibleAnywhere, Category = "Gyra|GameState")
	TObjectPtr<UGyraAbilitySystemComponent> AbilitySystemComponent;

protected:
	
	UPROPERTY(Replicated)
	float ServerFPS;


};
