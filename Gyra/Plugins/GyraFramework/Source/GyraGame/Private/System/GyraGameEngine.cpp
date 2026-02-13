// Copyright 2024 Huagang Liu. All Rights Reserved.

#include "GyraGameEngine.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GyraGameEngine)

class IEngineLoop;


UGyraGameEngine::UGyraGameEngine(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UGyraGameEngine::Init(IEngineLoop* InEngineLoop)
{
	Super::Init(InEngineLoop);
}


