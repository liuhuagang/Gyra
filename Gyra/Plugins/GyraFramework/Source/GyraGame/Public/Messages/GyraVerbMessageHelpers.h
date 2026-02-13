// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "GyraVerbMessageHelpers.generated.h"

struct FGameplayCueParameters;
struct FGyraVerbMessage;

class APlayerController;
class APlayerState;
class UObject;
struct FFrame;


UCLASS()
class GYRAGAME_API UGyraVerbMessageHelpers : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Gyra")
	static APlayerState* GetPlayerStateFromObject(UObject* Object);

	UFUNCTION(BlueprintCallable, Category = "Gyra")
	static APlayerController* GetPlayerControllerFromObject(UObject* Object);

	UFUNCTION(BlueprintCallable, Category = "Gyra")
	static FGameplayCueParameters VerbMessageToCueParameters(const FGyraVerbMessage& Message);

	UFUNCTION(BlueprintCallable, Category = "Gyra")
	static FGyraVerbMessage CueParametersToVerbMessage(const FGameplayCueParameters& Params);
};
