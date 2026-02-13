// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "GameFramework/CheatManager.h"

#include "GyraCosmeticCheats.generated.h"

class UGyraControllerComponent_CharacterParts;
class UObject;
struct FFrame;

/** Cheats related to bots */
UCLASS(NotBlueprintable)
class UGyraCosmeticCheats final : public UCheatManagerExtension
{
	GENERATED_BODY()

public:
	UGyraCosmeticCheats();

	// Adds a character part
	UFUNCTION(Exec, BlueprintAuthorityOnly)
	void AddCharacterPart(const FString& AssetName, bool bSuppressNaturalParts = true);

	// Replaces previous cheat parts with a new one
	UFUNCTION(Exec, BlueprintAuthorityOnly)
	void ReplaceCharacterPart(const FString& AssetName, bool bSuppressNaturalParts = true);

	// Clears any existing cheats
	UFUNCTION(Exec, BlueprintAuthorityOnly)
	void ClearCharacterPartOverrides();

private:
	UGyraControllerComponent_CharacterParts* GetCosmeticComponent() const;
};
