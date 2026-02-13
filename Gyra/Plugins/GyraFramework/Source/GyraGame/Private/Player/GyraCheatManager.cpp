// Copyright 2024 Huagang Liu. All Rights Reserved.

#include "GyraCheatManager.h"
#include "GameFramework/Pawn.h"
#include "GyraPlayerController.h"
#include "GyraDebugCameraController.h"
#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"
#include "Engine/Console.h"
#include "GameFramework/HUD.h"
#include "System/GyraAssetManager.h"
#include "System/GyraGameData.h"
#include "GyraGameplayTags.h"
#include "AbilitySystem/GyraAbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "GyraHealthComponent.h"
#include "GyraPawnExtensionComponent.h"
#include "System/GyraSystemStatics.h"
#include "GyraDeveloperSettings.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GyraCheatManager)

DEFINE_LOG_CATEGORY(LogGyraGameCheat);

namespace GyraCheat
{
	static const FName NAME_Fixed = FName(TEXT("Fixed"));
	
	static bool bEnableDebugCameraCycling = false;
	static FAutoConsoleVariableRef CVarEnableDebugCameraCycling(
		TEXT("GyraCheat.EnableDebugCameraCycling"),
		bEnableDebugCameraCycling,
		TEXT("If true then you can cycle the debug camera while running the game."),
		ECVF_Cheat);

	static bool bStartInGodMode = false;
	static FAutoConsoleVariableRef CVarStartInGodMode(
		TEXT("GyraCheat.StartInGodMode"),
		bStartInGodMode,
		TEXT("If true then the God cheat will be applied on begin play"),
		ECVF_Cheat);
};


UGyraCheatManager::UGyraCheatManager()
{
	DebugCameraControllerClass = AGyraDebugCameraController::StaticClass();
}

void UGyraCheatManager::InitCheatManager()
{
	Super::InitCheatManager();

#if WITH_EDITOR
	if (GIsEditor)
	{
		APlayerController* PC = GetOuterAPlayerController();
		for (const FGyraCheatToRun& CheatRow : GetDefault<UGyraDeveloperSettings>()->CheatsToRun)
		{
			if (CheatRow.Phase == ECheatExecutionTime::OnCheatManagerCreated)
			{
				PC->ConsoleCommand(CheatRow.Cheat, /*bWriteToLog=*/ true);
			}
		}
	}
#endif

	if (GyraCheat::bStartInGodMode)
	{
		God();	
	}
}

void UGyraCheatManager::CheatOutputText(const FString& TextToOutput)
{
#if USING_CHEAT_MANAGER
	// Output to the console.
	if (GEngine && GEngine->GameViewport && GEngine->GameViewport->ViewportConsole)
	{
		GEngine->GameViewport->ViewportConsole->OutputText(TextToOutput);
	}

	// Output to log.
	UE_LOG(LogGyraGameCheat, Display, TEXT("%s"), *TextToOutput);
#endif // USING_CHEAT_MANAGER
}

void UGyraCheatManager::Cheat(const FString& Msg)
{
	if (AGyraPlayerController* GyraPC = Cast<AGyraPlayerController>(GetOuterAPlayerController()))
	{
		GyraPC->ServerCheat(Msg.Left(128));
	}
}

void UGyraCheatManager::CheatAll(const FString& Msg)
{
	if (AGyraPlayerController* GyraPC = Cast<AGyraPlayerController>(GetOuterAPlayerController()))
	{
		GyraPC->ServerCheatAll(Msg.Left(128));
	}
}

void UGyraCheatManager::PlayNextGame()
{
	UGyraSystemStatics::PlayNextGame(this);
}

void UGyraCheatManager::EnableDebugCamera()
{
	Super::EnableDebugCamera();
}

void UGyraCheatManager::DisableDebugCamera()
{
	FVector DebugCameraLocation;
	FRotator DebugCameraRotation;

	ADebugCameraController* DebugCC = Cast<ADebugCameraController>(GetOuter());
	APlayerController* OriginalPC = nullptr;

	if (DebugCC)
	{
		OriginalPC = DebugCC->OriginalControllerRef;
		DebugCC->GetPlayerViewPoint(DebugCameraLocation, DebugCameraRotation);
	}

	Super::DisableDebugCamera();

	if (OriginalPC && OriginalPC->PlayerCameraManager && (OriginalPC->PlayerCameraManager->CameraStyle == GyraCheat::NAME_Fixed))
	{
		OriginalPC->SetInitialLocationAndRotation(DebugCameraLocation, DebugCameraRotation);

		OriginalPC->PlayerCameraManager->ViewTarget.POV.Location = DebugCameraLocation;
		OriginalPC->PlayerCameraManager->ViewTarget.POV.Rotation = DebugCameraRotation;
		OriginalPC->PlayerCameraManager->PendingViewTarget.POV.Location = DebugCameraLocation;
		OriginalPC->PlayerCameraManager->PendingViewTarget.POV.Rotation = DebugCameraRotation;
	}
}

bool UGyraCheatManager::InDebugCamera() const
{
	return (Cast<ADebugCameraController>(GetOuter()) ? true : false);
}

void UGyraCheatManager::EnableFixedCamera()
{
	const ADebugCameraController* DebugCC = Cast<ADebugCameraController>(GetOuter());
	APlayerController* PC = (DebugCC ? ToRawPtr(DebugCC->OriginalControllerRef) : GetOuterAPlayerController());

	if (PC && PC->PlayerCameraManager)
	{
		PC->SetCameraMode(GyraCheat::NAME_Fixed);
	}
}

void UGyraCheatManager::DisableFixedCamera()
{
	const ADebugCameraController* DebugCC = Cast<ADebugCameraController>(GetOuter());
	APlayerController* PC = (DebugCC ? ToRawPtr(DebugCC->OriginalControllerRef) : GetOuterAPlayerController());

	if (PC && PC->PlayerCameraManager)
	{
		PC->SetCameraMode(NAME_Default);
	}
}

bool UGyraCheatManager::InFixedCamera() const
{
	const ADebugCameraController* DebugCC = Cast<ADebugCameraController>(GetOuter());
	const APlayerController* PC = (DebugCC ? ToRawPtr(DebugCC->OriginalControllerRef) : GetOuterAPlayerController());

	if (PC && PC->PlayerCameraManager)
	{
		return (PC->PlayerCameraManager->CameraStyle == GyraCheat::NAME_Fixed);
	}

	return false;
}

void UGyraCheatManager::ToggleFixedCamera()
{
	if (InFixedCamera())
	{
		DisableFixedCamera();
	}
	else
	{
		EnableFixedCamera();
	}
}

void UGyraCheatManager::CycleDebugCameras()
{
	if (!GyraCheat::bEnableDebugCameraCycling)
	{
		return;
	}
	
	if (InDebugCamera())
	{
		EnableFixedCamera();
		DisableDebugCamera();
	}
	else if (InFixedCamera())
	{
		DisableFixedCamera();
		DisableDebugCamera();
	}
	else
	{
		EnableDebugCamera();
		DisableFixedCamera();
	}
}

void UGyraCheatManager::CycleAbilitySystemDebug()
{
	APlayerController* PC = Cast<APlayerController>(GetOuterAPlayerController());

	if (PC && PC->MyHUD)
	{
		if (!PC->MyHUD->bShowDebugInfo || !PC->MyHUD->DebugDisplay.Contains(TEXT("AbilitySystem")))
		{
			PC->MyHUD->ShowDebug(TEXT("AbilitySystem"));
		}

		PC->ConsoleCommand(TEXT("AbilitySystem.Debug.NextCategory"));
	}
}

void UGyraCheatManager::CancelActivatedAbilities()
{
	if (UGyraAbilitySystemComponent* GyraASC = GetPlayerAbilitySystemComponent())
	{
		const bool bReplicateCancelAbility = true;
		GyraASC->CancelInputActivatedAbilities(bReplicateCancelAbility);
	}
}

void UGyraCheatManager::AddTagToSelf(FString TagName)
{
	FGameplayTag Tag = GyraGameplayTags::FindTagByString(TagName, true);
	if (Tag.IsValid())
	{
		if (UGyraAbilitySystemComponent* GyraASC = GetPlayerAbilitySystemComponent())
		{
			GyraASC->AddDynamicTagGameplayEffect(Tag);
		}
	}
	else
	{
		UE_LOG(LogGyraGameCheat, Display, TEXT("AddTagToSelf: Could not find any tag matching [%s]."), *TagName);
	}
}

void UGyraCheatManager::RemoveTagFromSelf(FString TagName)
{
	FGameplayTag Tag = GyraGameplayTags::FindTagByString(TagName, true);
	if (Tag.IsValid())
	{
		if (UGyraAbilitySystemComponent* GyraASC = GetPlayerAbilitySystemComponent())
		{
			GyraASC->RemoveDynamicTagGameplayEffect(Tag);
		}
	}
	else
	{
		UE_LOG(LogGyraGameCheat, Display, TEXT("RemoveTagFromSelf: Could not find any tag matching [%s]."), *TagName);
	}
}

void UGyraCheatManager::DamageSelf(float DamageAmount)
{
	if (UGyraAbilitySystemComponent* GyraASC = GetPlayerAbilitySystemComponent())
	{
		ApplySetByCallerDamage(GyraASC, DamageAmount);
	}
}

void UGyraCheatManager::DamageTarget(float DamageAmount)
{
	if (AGyraPlayerController* GyraPC = Cast<AGyraPlayerController>(GetOuterAPlayerController()))
	{
		if (GyraPC->GetNetMode() == NM_Client)
		{
			// Automatically send cheat to server for convenience.
			GyraPC->ServerCheat(FString::Printf(TEXT("DamageTarget %.2f"), DamageAmount));
			return;
		}

		FHitResult TargetHitResult;
		AActor* TargetActor = GetTarget(GyraPC, TargetHitResult);

		if (UGyraAbilitySystemComponent* GyraTargetASC = Cast<UGyraAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor)))
		{
			ApplySetByCallerDamage(GyraTargetASC, DamageAmount);
		}
	}
}

void UGyraCheatManager::ApplySetByCallerDamage(UGyraAbilitySystemComponent* GyraASC, float DamageAmount)
{
	check(GyraASC);

	TSubclassOf<UGameplayEffect> DamageGE = UGyraAssetManager::GetSubclass(UGyraGameData::Get().DamageGameplayEffect_SetByCaller);
	FGameplayEffectSpecHandle SpecHandle = GyraASC->MakeOutgoingSpec(DamageGE, 1.0f, GyraASC->MakeEffectContext());

	if (SpecHandle.IsValid())
	{
		SpecHandle.Data->SetSetByCallerMagnitude(GyraGameplayTags::SetByCaller_Damage, DamageAmount);
		GyraASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
}

void UGyraCheatManager::HealSelf(float HealAmount)
{
	if (UGyraAbilitySystemComponent* GyraASC = GetPlayerAbilitySystemComponent())
	{
		ApplySetByCallerHeal(GyraASC, HealAmount);
	}
}

void UGyraCheatManager::HealTarget(float HealAmount)
{
	if (AGyraPlayerController* GyraPC = Cast<AGyraPlayerController>(GetOuterAPlayerController()))
	{
		FHitResult TargetHitResult;
		AActor* TargetActor = GetTarget(GyraPC, TargetHitResult);

		if (UGyraAbilitySystemComponent* GyraTargetASC = Cast<UGyraAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor)))
		{
			ApplySetByCallerHeal(GyraTargetASC, HealAmount);
		}
	}
}

void UGyraCheatManager::ApplySetByCallerHeal(UGyraAbilitySystemComponent* GyraASC, float HealAmount)
{
	check(GyraASC);

	TSubclassOf<UGameplayEffect> HealGE = UGyraAssetManager::GetSubclass(UGyraGameData::Get().HealGameplayEffect_SetByCaller);
	FGameplayEffectSpecHandle SpecHandle = GyraASC->MakeOutgoingSpec(HealGE, 1.0f, GyraASC->MakeEffectContext());

	if (SpecHandle.IsValid())
	{
		SpecHandle.Data->SetSetByCallerMagnitude(GyraGameplayTags::SetByCaller_Heal, HealAmount);
		GyraASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
}

UGyraAbilitySystemComponent* UGyraCheatManager::GetPlayerAbilitySystemComponent() const
{
	if (AGyraPlayerController* GyraPC = Cast<AGyraPlayerController>(GetOuterAPlayerController()))
	{
		return GyraPC->GetGyraAbilitySystemComponent();
	}
	return nullptr;
}

void UGyraCheatManager::DamageSelfDestruct()
{
	if (AGyraPlayerController* GyraPC = Cast<AGyraPlayerController>(GetOuterAPlayerController()))
	{
 		if (const UGyraPawnExtensionComponent* PawnExtComp = UGyraPawnExtensionComponent::FindPawnExtensionComponent(GyraPC->GetPawn()))
		{
			if (PawnExtComp->HasReachedInitState(GyraGameplayTags::InitState_GameplayReady))
			{
				if (UGyraHealthComponent* HealthComponent = UGyraHealthComponent::FindHealthComponent(GyraPC->GetPawn()))
				{
					HealthComponent->DamageSelfDestruct();
				}
			}
		}
	}
}

void UGyraCheatManager::God()
{
	if (AGyraPlayerController* GyraPC = Cast<AGyraPlayerController>(GetOuterAPlayerController()))
	{
		if (GyraPC->GetNetMode() == NM_Client)
		{
			// Automatically send cheat to server for convenience.
			GyraPC->ServerCheat(FString::Printf(TEXT("God")));
			return;
		}

		if (UGyraAbilitySystemComponent* GyraASC = GyraPC->GetGyraAbilitySystemComponent())
		{
			const FGameplayTag Tag = GyraGameplayTags::Cheat_GodMode;
			const bool bHasTag = GyraASC->HasMatchingGameplayTag(Tag);

			if (bHasTag)
			{
				GyraASC->RemoveDynamicTagGameplayEffect(Tag);
			}
			else
			{
				GyraASC->AddDynamicTagGameplayEffect(Tag);
			}
		}
	}
}

void UGyraCheatManager::UnlimitedHealth(int32 Enabled)
{
	if (UGyraAbilitySystemComponent* GyraASC = GetPlayerAbilitySystemComponent())
	{
		const FGameplayTag Tag = GyraGameplayTags::Cheat_UnlimitedHealth;
		const bool bHasTag = GyraASC->HasMatchingGameplayTag(Tag);

		if ((Enabled == -1) || ((Enabled > 0) && !bHasTag) || ((Enabled == 0) && bHasTag))
		{
			if (bHasTag)
			{
				GyraASC->RemoveDynamicTagGameplayEffect(Tag);
			}
			else
			{
				GyraASC->AddDynamicTagGameplayEffect(Tag);
			}
		}
	}
}

