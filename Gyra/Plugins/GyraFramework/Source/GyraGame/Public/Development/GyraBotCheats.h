// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "GameFramework/CheatManager.h"

#include "GyraBotCheats.generated.h"

class UGyraBotCreationComponent;
class UObject;
struct FFrame;

/** Cheats related to bots */
UCLASS(NotBlueprintable)
class UGyraBotCheats final : public UCheatManagerExtension
{
	GENERATED_BODY()

public:
	UGyraBotCheats();

	// Adds a bot player
	UFUNCTION(Exec, BlueprintAuthorityOnly)
	void AddPlayerBot();

	// Removes a random bot player
	UFUNCTION(Exec, BlueprintAuthorityOnly)
	void RemovePlayerBot();

private:
	UGyraBotCreationComponent* GetBotComponent() const;
};
