// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameplayMessageProcessor.h"

#include "AssistProcessor.generated.h"

class APlayerState;
class UObject;
struct FGameplayTag;
struct FGyraVerbMessage;
template <typename T> struct TObjectPtr;

// Tracks the damage done to a player by other players
USTRUCT()
struct FPlayerAssistDamageTracking
{
	GENERATED_BODY()

	// Map of damager to damage dealt
	UPROPERTY(Transient)
	TMap<TObjectPtr<APlayerState>, float> AccumulatedDamageByPlayer;
};

// Tracks assists (dealing damage to another player without finishing them)
UCLASS()
class UAssistProcessor : public UGameplayMessageProcessor
{
	GENERATED_BODY()

public:
	virtual void StartListening() override;

private:
	void OnDamageMessage(FGameplayTag Channel, const FGyraVerbMessage& Payload);
	void OnEliminationMessage(FGameplayTag Channel, const FGyraVerbMessage& Payload);

private:
	// Map of player to damage dealt to them
	UPROPERTY(Transient)
	TMap<TObjectPtr<APlayerState>, FPlayerAssistDamageTracking> DamageHistory;
};
