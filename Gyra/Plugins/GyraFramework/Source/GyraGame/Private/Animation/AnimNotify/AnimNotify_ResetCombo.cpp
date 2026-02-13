// Copyright 2024 Huagang Liu. All Rights Reserved.

#include "AnimNotify_ResetCombo.h"

#include "GyraComboComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AnimNotify_ResetCombo)

UAnimNotify_ResetCombo::UAnimNotify_ResetCombo()
{

}

FString UAnimNotify_ResetCombo::GetNotifyName_Implementation() const
{
	return Super::GetNotifyName_Implementation();
}

void UAnimNotify_ResetCombo::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (AActor* Character = Cast<AActor>(MeshComp->GetOuter()))
	{
		if (UGyraComboComponent* ComboComponent =UGyraComboComponent::FindComboComponent(Character))
		{
			ComboComponent->ResetComboIndex();
		}
	}
}
