// Copyright 2024 Huagang Liu. All Rights Reserved.

#include "GyraNumberPopComponent_NiagaraText.h"

#include "GyraNumberPopComponent.h"
#include "GyraDamagePopStyleNiagara.h"
#include "LogGyraGame.h"
#include "NiagaraComponent.h"
#include "NiagaraDataInterfaceArrayFunctionLibrary.h"

#include "GyraGameplayTags.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GyraNumberPopComponent_NiagaraText)

UGyraNumberPopComponent_NiagaraText::UGyraNumberPopComponent_NiagaraText(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{


}

void UGyraNumberPopComponent_NiagaraText::AddNumberPop(const FGyraNumberPopRequest& NewRequest)
{
	int32 LocalDamage = NewRequest.NumberToDisplay;
	int32 ColorIndex = 0;

	//Change Damage to negative to differentiate Critial vs Normal hit
	if (NewRequest.bIsCriticalDamage)
	{
		LocalDamage *= -1;
	}

	const FGameplayTagContainer& SourceTags = NewRequest.SourceTags;

	if (SourceTags.HasTagExact(GyraGameplayTags::DamageEffect_Physics_DeadAttack))
	{
		ColorIndex = 3;
	}
	else if (SourceTags.HasTagExact(GyraGameplayTags::DamageEffect_Physics_CriticalAttack))
	{
		ColorIndex = 2;
	}
	else if (SourceTags.HasTagExact(GyraGameplayTags::DamageEffect_Physics_WeakAttack))
	{
		ColorIndex = 1;
	}



	//Add a NiagaraComponent if we don't already have one
	if (!NiagaraComp)
	{
		NiagaraComp = NewObject<UNiagaraComponent>(GetOwner());
		if (Style != nullptr)
		{
			NiagaraComp->SetAsset(Style->TextNiagara);
			NiagaraComp->bAutoActivate = false;

		}
		NiagaraComp->SetupAttachment(nullptr);
		check(NiagaraComp);
		NiagaraComp->RegisterComponent();
	}


	NiagaraComp->Activate(false);
	NiagaraComp->SetWorldLocation(NewRequest.WorldLocation);

	UE_LOG(LogGyraGame, Log, TEXT("DamageHit location : %s"), *(NewRequest.WorldLocation.ToString()));
	//Add Damage information to the current Niagara list - Damage informations are packed inside a FVector4 where XYZ = Position, W = Damage
	TArray<FVector4> DamageList = UNiagaraDataInterfaceArrayFunctionLibrary::GetNiagaraArrayVector4(NiagaraComp, Style->NiagaraArrayName);
	DamageList.Add(FVector4(NewRequest.WorldLocation.X, NewRequest.WorldLocation.Y, NewRequest.WorldLocation.Z, LocalDamage));
	UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayVector4(NiagaraComp, Style->NiagaraArrayName, DamageList);

	TArray<int32> ColorIndexList = UNiagaraDataInterfaceArrayFunctionLibrary::GetNiagaraArrayInt32(NiagaraComp, TEXT("ColorIndex"));
	ColorIndexList.Add(ColorIndex);
	UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayInt32(NiagaraComp, TEXT("ColorIndex"), ColorIndexList);
}

