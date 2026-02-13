// Copyright Epic Games, Inc. All Rights Reserved.

#include "GyraPlayerInput.h"


#include "Performance/LatencyMarkerModule.h"

#include "GyraSettingsLocal.h"

UGyraPlayerInput::UGyraPlayerInput()
	: Super()
{
}

UGyraPlayerInput::~UGyraPlayerInput()
{
}

bool UGyraPlayerInput::InputKey(const FInputKeyEventArgs& Params)
{
	const bool bResult = Super::InputKey(Params);


	return bResult;
}


