// Copyright 2024 Huagang Liu. All Rights Reserved.

#include "GyraUICameraManagerComponent.h"

#include "GameFramework/HUD.h"
#include "GameFramework/PlayerController.h"

#include "GyraPlayerCameraManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GyraUICameraManagerComponent)

class AActor;
class FDebugDisplayInfo;

UGyraUICameraManagerComponent* UGyraUICameraManagerComponent::GetComponent(APlayerController* PC)
{
	if (PC != nullptr)
	{
		if (AGyraPlayerCameraManager* PCCamera = Cast<AGyraPlayerCameraManager>(PC->PlayerCameraManager))
		{
			return PCCamera->GetUICameraComponent();
		}
	}

	return nullptr;
}

UGyraUICameraManagerComponent::UGyraUICameraManagerComponent()
{
	bWantsInitializeComponent = true;

	if (!HasAnyFlags(RF_ClassDefaultObject))
	{
		// Register "showdebug" hook.
		if (!IsRunningDedicatedServer())
		{
			AHUD::OnShowDebugInfo.AddUObject(this, &ThisClass::OnShowDebugInfo);
		}
	}
}

void UGyraUICameraManagerComponent::InitializeComponent()
{
	Super::InitializeComponent();
}

void UGyraUICameraManagerComponent::SetViewTarget(AActor* InViewTarget, FViewTargetTransitionParams TransitionParams)
{
	TGuardValue<bool> UpdatingViewTargetGuard(bUpdatingViewTarget, true);

	ViewTarget = InViewTarget;
	CastChecked<AGyraPlayerCameraManager>(GetOwner())->SetViewTarget(ViewTarget, TransitionParams);
}

bool UGyraUICameraManagerComponent::NeedsToUpdateViewTarget() const
{
	return false;
}

void UGyraUICameraManagerComponent::UpdateViewTarget(struct FTViewTarget& OutVT, float DeltaTime)
{
}

void UGyraUICameraManagerComponent::OnShowDebugInfo(AHUD* HUD, UCanvas* Canvas, const FDebugDisplayInfo& DisplayInfo, float& YL, float& YPos)
{
}

