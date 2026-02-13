// Copyright 2024 Huagang Liu. All Rights Reserved.

#include "GyraDebugCameraController.h"
#include "GyraCheatManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GyraDebugCameraController)


AGyraDebugCameraController::AGyraDebugCameraController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Use the same cheat class as GyraPlayerController to allow toggling the debug camera through cheats.
	CheatClass = UGyraCheatManager::StaticClass();
}

void AGyraDebugCameraController::AddCheats(bool bForce)
{
	// Mirrors GyraPlayerController's AddCheats() to avoid the player becoming stuck in the debug camera.
#if USING_CHEAT_MANAGER
	Super::AddCheats(true);
#else //#if USING_CHEAT_MANAGER
	Super::AddCheats(bForce);
#endif // #else //#if USING_CHEAT_MANAGER
}

