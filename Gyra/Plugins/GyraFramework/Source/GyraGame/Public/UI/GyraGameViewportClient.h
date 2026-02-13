// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "CommonGameViewportClient.h"

#include "GyraGameViewportClient.generated.h"

class UGameInstance;
class UObject;

UCLASS(BlueprintType)
class UGyraGameViewportClient : public UCommonGameViewportClient
{
	GENERATED_BODY()

public:
	
	UGyraGameViewportClient();

	virtual void Init(struct FWorldContext& WorldContext, UGameInstance* OwningGameInstance, bool bCreateNewAudioDevice = true) override;
	
};
