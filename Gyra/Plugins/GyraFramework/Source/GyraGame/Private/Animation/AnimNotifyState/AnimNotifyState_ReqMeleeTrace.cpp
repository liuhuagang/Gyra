// Copyright 2024 Huagang Liu. All Rights Reserved.

#include "AnimNotifyState_ReqMeleeTrace.h"

#include "GameFramework/Character.h"
#include "Animation/AnimSequenceBase.h"
#include "Components/SkeletalMeshComponent.h"

#include "GyraQuickBarComponent.h"
#include "GyraEquipmentInstance.h"
#include "MeleeTraceComponent.h"
#include "GyraMeleeWeaponInstance.h"
#include "GyraWeaponTraceInterface.h"


void UAnimNotifyState_ReqMeleeTrace::NotifyBegin(USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation,
	float TotalDuration,
	const FAnimNotifyEventReference& EventReference)
{

	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (MeshComp->GetOuter() && MeshComp->GetOuter()->GetWorld() && !MeshComp->GetOuter()->GetWorld()->IsNetMode(ENetMode::NM_Client))
	{
		if ( AActor* OwnerActor = MeshComp->GetOwner())
		{
			if (ACharacter* OwnerCharacter =  Cast<ACharacter>(OwnerActor))
			{
				if ( AController *  OwnerController= OwnerCharacter->GetController())
				{
					 UGyraQuickBarComponent* QuickBarComp=	Cast<UGyraQuickBarComponent>(OwnerController->FindComponentByClass(UGyraQuickBarComponent::StaticClass()));
					
					if (QuickBarComp)
					 {
						if (UGyraEquipmentInstance* EquipmentInstance = QuickBarComp->GetActiveEquipmentInstance())
						{
							TArray<AActor*>  SpawnedActors = EquipmentInstance->GetSpawnedActors();

							for (auto& Tmp : SpawnedActors)
							{
								if (Tmp->Implements<UGyraWeaponTraceInterface>())
								{
									IGyraWeaponTraceInterface::Execute_TraceStart(Tmp);
								}

							}

						}
					 }

				}
			

			}
		}
	}
}


void UAnimNotifyState_ReqMeleeTrace::NotifyEnd(USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)

{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (MeshComp->GetOuter() && MeshComp->GetOuter()->GetWorld() && !MeshComp->GetOuter()->GetWorld()->IsNetMode(ENetMode::NM_Client))
	{
		if (AActor* OwnerActor = MeshComp->GetOwner())
		{
			if (ACharacter* OwnerCharacter = Cast<ACharacter>(OwnerActor))
			{
				if (AController* OwnerController = OwnerCharacter->GetController())
				{
					UGyraQuickBarComponent* QuickBarComp = Cast<UGyraQuickBarComponent>(OwnerController->FindComponentByClass(UGyraQuickBarComponent::StaticClass()));
					if (QuickBarComp){

					if (UGyraEquipmentInstance* EquipmentInstance = QuickBarComp->GetActiveEquipmentInstance())
					{
						TArray<AActor*>  SpawnedActors = EquipmentInstance->GetSpawnedActors();

						for (auto& Tmp : SpawnedActors)
						{
							if (Tmp->Implements<UGyraWeaponTraceInterface>())
							{
								IGyraWeaponTraceInterface::Execute_TraceEnd(Tmp);
							}

						}

					}
					}
				}


			}
		}
	}


}

void UAnimNotifyState_ReqMeleeTrace::NotifyTick(USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation,
	float FrameDeltaTime,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
}
