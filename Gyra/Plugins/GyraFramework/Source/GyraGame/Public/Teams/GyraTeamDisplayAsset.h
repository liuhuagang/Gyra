// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "Engine/DataAsset.h"
#include "GyraTeamDisplayAsset.generated.h"

struct FPropertyChangedEvent;

class UMaterialInstanceDynamic;
class UMeshComponent;
class UNiagaraComponent;
class AActor;
class UTexture;

// Represents the display information for team definitions (e.g., colors, display names, textures, etc...)
// 表示团队定义的显示信息（例如，颜色，显示名称，纹理等）
UCLASS(BlueprintType)
class UGyraTeamDisplayAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<FName, float> ScalarParameters;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<FName, FLinearColor> ColorParameters;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<FName, TObjectPtr<UTexture>> TextureParameters;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText TeamShortName;

public:
	UFUNCTION(BlueprintCallable, Category = Teams)
	void ApplyToMaterial(UMaterialInstanceDynamic* Material);

	UFUNCTION(BlueprintCallable, Category = Teams)
	void ApplyToMeshComponent(UMeshComponent* MeshComponent);

	UFUNCTION(BlueprintCallable, Category = Teams)
	void ApplyToNiagaraComponent(UNiagaraComponent* NiagaraComponent);

	UFUNCTION(BlueprintCallable, Category = Teams, meta = (DefaultToSelf = "TargetActor"))
	void ApplyToActor(AActor* TargetActor, bool bIncludeChildActors = true);

public:

	//~UObject interface
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	//~End of UObject interface
};
