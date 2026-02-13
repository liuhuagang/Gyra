// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "GyraNumberPopComponent.h"

#include "GyraNumberPopComponent_NiagaraText.generated.h"

class UGyraDamagePopStyleNiagara;
class UNiagaraComponent;
class UObject;

UCLASS(Blueprintable)
class UGyraNumberPopComponent_NiagaraText : public UGyraNumberPopComponent
{
	GENERATED_BODY()

public:

	UGyraNumberPopComponent_NiagaraText(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~UGyraNumberPopComponent interface
	virtual void AddNumberPop(const FGyraNumberPopRequest& NewRequest) override;
	//~End of UGyraNumberPopComponent interface

protected:
	
	TArray<int32> DamageNumberArray;

	/** Style patterns to attempt to apply to the incoming number pops */
	UPROPERTY(EditDefaultsOnly, Category = "Number Pop|Style")
	TObjectPtr<UGyraDamagePopStyleNiagara> Style;

	//Niagara Component used to display the damage
	UPROPERTY(EditDefaultsOnly, Category = "Number Pop|Style")
	TObjectPtr<UNiagaraComponent> NiagaraComp;
};
