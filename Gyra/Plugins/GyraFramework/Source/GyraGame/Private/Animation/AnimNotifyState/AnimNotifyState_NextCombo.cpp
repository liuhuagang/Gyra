// Copyright 2024 Huagang Liu. All Rights Reserved.

#include "AnimNotifyState_NextCombo.h"

#include "GyraComboComponent.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(AnimNotifyState_NextCombo)


UAnimNotifyState_NextCombo::UAnimNotifyState_NextCombo()
{


}

void UAnimNotifyState_NextCombo::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (MeshComp->GetOuter() && MeshComp->GetOuter()->GetWorld() && !MeshComp->GetOuter()->GetWorld()->IsNetMode(ENetMode::NM_DedicatedServer))
	{
		if (AActor* Character = Cast<AActor>(MeshComp->GetOuter()))
		{
			if (UGyraComboComponent* ComboComponent = UGyraComboComponent::FindComboComponent(Character))
			{
				ComboComponent->ResetPressed();
			}
		}
	}

}

void UAnimNotifyState_NextCombo::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

}

void UAnimNotifyState_NextCombo::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (MeshComp->GetOuter() && MeshComp->GetOuter()->GetWorld() && !MeshComp->GetOuter()->GetWorld()->IsNetMode(ENetMode::NM_DedicatedServer))
	{
		if (AActor* Character = Cast<AActor>(MeshComp->GetOuter()))
		{
			if (UGyraComboComponent* ComboComponent = UGyraComboComponent::FindComboComponent(Character))
			{
				if (ComboComponent->IsStillPressed())
				{

					ComboComponent->ComboMelee();

				}
			}
		}
	}

}
