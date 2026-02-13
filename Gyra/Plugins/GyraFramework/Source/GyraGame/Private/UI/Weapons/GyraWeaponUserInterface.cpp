// Copyright 2024 Huagang Liu. All Rights Reserved.

#include "GyraWeaponUserInterface.h"

#include "GyraEquipmentManagerComponent.h"

#include "GameFramework/Pawn.h"

#include "GyraWeaponInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GyraWeaponUserInterface)

struct FGeometry;

UGyraWeaponUserInterface::UGyraWeaponUserInterface(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UGyraWeaponUserInterface::NativeConstruct()
{
	Super::NativeConstruct();
}

void UGyraWeaponUserInterface::NativeDestruct()
{
	Super::NativeDestruct();
}

void UGyraWeaponUserInterface::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (APawn* Pawn = GetOwningPlayerPawn())
	{
		if (UGyraEquipmentManagerComponent* EquipmentManager = Pawn->FindComponentByClass<UGyraEquipmentManagerComponent>())
		{
			if (UGyraWeaponInstance* NewInstance = EquipmentManager->GetFirstInstanceOfType<UGyraWeaponInstance>())
			{
				if (NewInstance != CurrentInstance && NewInstance->GetInstigator() != nullptr)
				{
					UGyraWeaponInstance* OldWeapon = CurrentInstance;
					CurrentInstance = NewInstance;
					RebuildWidgetFromWeapon();
					OnWeaponChanged(OldWeapon, CurrentInstance);
				}
			}
		}
	}
}

void UGyraWeaponUserInterface::RebuildWidgetFromWeapon()
{
	
}

