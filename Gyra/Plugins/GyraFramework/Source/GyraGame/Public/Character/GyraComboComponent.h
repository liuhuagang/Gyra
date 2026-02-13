// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/GameFrameworkComponent.h"
#include "GyraComboComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class GYRAGAME_API UGyraComboComponent : public UGameFrameworkComponent
{
	GENERATED_BODY()


public:

	UGyraComboComponent(const FObjectInitializer& ObjectInitializer);

	// Returns the health component if one exists on the specified actor.
	UFUNCTION(BlueprintPure, Category = "Gyra|Fight|Combo")
	static UGyraComboComponent* FindComboComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UGyraComboComponent>() : nullptr); }

	UFUNCTION(BlueprintPure, Category = "Gyra|Fight|Combo")
	int32 GetComboIndex();

	UFUNCTION(BlueprintCallable, Category = "Gyra|Fight|Combo")
	void SetComboAnimMontages(TArray<UAnimMontage*> InMontages);

	UFUNCTION(BlueprintPure, Category = "Gyra|Fight|Combo")
	UAnimMontage* GetComboAnimMontage();

	UFUNCTION(BlueprintCallable, Category = "Gyra|Fight|Combo")
	UAnimMontage* GetLastPlayAnimMontage();

	UFUNCTION(BlueprintCallable, Category = "Gyra|Fight|Combo")
	void UpdateComboIndex();

	UFUNCTION(BlueprintCallable, Category = "Gyra|Fight|Combo")
	void ResetComboIndex();

	UFUNCTION(BlueprintCallable, Category = "Gyra|Fight|Combo")
	void SetPressed();

	UFUNCTION(BlueprintCallable, Category = "Gyra|Fight|Combo")
	void ResetPressed();

	UFUNCTION(BlueprintCallable, Category = "Gyra|Fight|Combo")
	bool IsStillPressed();

	UFUNCTION(BlueprintCallable, Category = "Gyra|Fight|Combo")
	void ComboMelee();

protected:

	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Gyra|Fight|Combo")
	TArray<TObjectPtr<UAnimMontage>> ComboMontages;

	UPROPERTY();
	TObjectPtr<UAnimMontage> LastPlayMontage = nullptr;

	int32 ComboIndex = 0;

	int32 ComboMaxIndex = 0;

	bool bShortPress = false;

};

