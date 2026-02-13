// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "Abilities/GameplayAbility.h"

#include "GyraGameplayAbility.generated.h"


class UObject;
class AActor;
class AController;
class APlayerController;
class FText;
class UAnimMontage;

class IGyraAbilitySourceInterface;
class AGyraCharacter;
class AGyraCharacterBase;
class AGyraPlayerController;


class UGyraAbilityCost;
class UGyraAbilitySystemComponent;
class UGyraCameraMode;
class UGyraHeroComponent;
class AGyraMonster;


struct FFrame;
struct FGameplayAbilityActorInfo;
struct FGameplayEffectSpec;
struct FGameplayEventData;
struct FGameplayAbilityActivationInfo;
struct FGameplayAbilitySpec;
struct FGameplayAbilitySpecHandle;

/**
 * EGyraAbilityActivationPolicy
 *
 * Defines how an ability is meant to activate.
 * 定义如何激活一个能力。
 */
UENUM(BlueprintType)
enum class EGyraAbilityActivationPolicy : uint8
{
	// Try to activate the ability when the input is triggered.
	// 当输入被触发时，试着激活这个能力。
	OnInputTriggered,

	// Continually try to activate the ability while the input is active.
	// 当输入是活跃的时候，不断地尝试激活这种能力。
	WhileInputActive,

	// Try to activate the ability when an avatar is assigned.
	// 当一个角色被分配时，试着激活这个能力。
	OnSpawn
};


/** Failure reason that can be used to play an animation montage when a failure occurs */
/** 故障原因，可用于播放动画蒙太奇时，发生故障 */
USTRUCT(BlueprintType)
struct FGyraAbilityMontageFailureMessage
{
	GENERATED_BODY()

public:
	// Player controller that failed to activate the ability, if the AbilitySystemComponent was player owned
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<APlayerController> PlayerController = nullptr;

	// Avatar actor that failed to activate the ability
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<AActor> AvatarActor = nullptr;

	// All the reasons why this ability has failed
	UPROPERTY(BlueprintReadWrite)
	FGameplayTagContainer FailureTags;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UAnimMontage> FailureMontage = nullptr;
};


/**
 * UGyraGameplayAbility
 *
 *	The base gameplay ability class used by this project.
 */
UCLASS(Abstract, HideCategories = Input,
	Meta = (ShortTooltip = "The base gameplay ability class used by this project."))
class GYRAGAME_API UGyraGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	friend class UGyraAbilitySystemComponent;

public:
	UGyraGameplayAbility(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category = "Gyra|Ability")
	UGyraAbilitySystemComponent* GetGyraAbilitySystemComponentFromActorInfo() const;

	UFUNCTION(BlueprintCallable, Category = "Gyra|Ability")
	AGyraPlayerController* GetGyraPlayerControllerFromActorInfo() const;

	UFUNCTION(BlueprintCallable, Category = "Gyra|Ability")
	AController* GetControllerFromActorInfo() const;

	UFUNCTION(BlueprintCallable, Category = "Gyra|Ability")
	AGyraCharacterBase* GetGyraCharacterBaseFromActorInfo() const;

	UFUNCTION(BlueprintCallable, Category = "Gyra|Ability")
	AGyraCharacter* GetGyraCharacterFromActorInfo() const;

	UFUNCTION(BlueprintCallable, Category = "Gyra|Ability")
	AGyraMonster* GetGyraMonsterFromActorInfo() const;

	UFUNCTION(BlueprintCallable, Category = "Gyra|Ability")
	UGyraHeroComponent* GetHeroComponentFromActorInfo() const;

	EGyraAbilityActivationPolicy GetActivationPolicy() const { return ActivationPolicy; }


	void TryActivateAbilityOnSpawn(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) const;
	

	// Sets the ability's camera mode.
	UFUNCTION(BlueprintCallable, Category = "Gyra|Ability")
	void SetCameraMode(TSubclassOf<UGyraCameraMode> CameraMode);

	// Clears the ability's camera mode.  Automatically called if needed when the ability ends.
	UFUNCTION(BlueprintCallable, Category = "Gyra|Ability")
	void ClearCameraMode();

	void OnAbilityFailedToActivate(const FGameplayTagContainer& FailedReason) const
	{
		NativeOnAbilityFailedToActivate(FailedReason);
		ScriptOnAbilityFailedToActivate(FailedReason);
	}

protected:
	// Called when the ability fails to activate
	virtual void NativeOnAbilityFailedToActivate(const FGameplayTagContainer& FailedReason) const;

	// Called when the ability fails to activate
	UFUNCTION(BlueprintImplementableEvent)
	void ScriptOnAbilityFailedToActivate(const FGameplayTagContainer& FailedReason) const;

	//~UGameplayAbility interface
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                                const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags,
	                                FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual void SetCanBeCanceled(bool bCanBeCanceled) override;
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                             const FGameplayAbilityActivationInfo ActivationInfo,
	                             const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                        const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility,
	                        bool bWasCancelled) override;
	virtual bool CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                       OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                       const FGameplayAbilityActivationInfo ActivationInfo) const override;
	virtual FGameplayEffectContextHandle MakeEffectContext(const FGameplayAbilitySpecHandle Handle,
	                                                       const FGameplayAbilityActorInfo* ActorInfo) const override;
	virtual void ApplyAbilityTagsToGameplayEffectSpec(FGameplayEffectSpec& Spec,
	                                                  FGameplayAbilitySpec* AbilitySpec) const override;
	virtual bool DoesAbilitySatisfyTagRequirements(const UAbilitySystemComponent& AbilitySystemComponent,
	                                               const FGameplayTagContainer* SourceTags = nullptr,
	                                               const FGameplayTagContainer* TargetTags = nullptr,
	                                               OUT FGameplayTagContainer* OptionalRelevantTags = nullptr)
	const override;
	//~End of UGameplayAbility interface

	virtual void OnPawnAvatarSet();

	virtual void GetAbilitySource(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                              float& OutSourceLevel, const IGyraAbilitySourceInterface*& OutAbilitySource,
	                              AActor*& OutEffectCauser) const;

	/** Called when this ability is granted to the ability system component. */
	UFUNCTION(BlueprintImplementableEvent, Category = Ability, DisplayName = "OnAbilityAdded")
	void K2_OnAbilityAdded();

	/** Called when this ability is removed from the ability system component. */
	UFUNCTION(BlueprintImplementableEvent, Category = Ability, DisplayName = "OnAbilityRemoved")
	void K2_OnAbilityRemoved();

	/** Called when the ability system is initialized with a pawn avatar. */
	UFUNCTION(BlueprintImplementableEvent, Category = Ability, DisplayName = "OnPawnAvatarSet")
	void K2_OnPawnAvatarSet();

protected:
	// Defines how this ability is meant to activate.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gyra|Ability Activation")
	EGyraAbilityActivationPolicy ActivationPolicy;


	// Additional costs that must be paid to activate this ability
	UPROPERTY(EditDefaultsOnly, Instanced, Category = Costs)
	TArray<TObjectPtr<UGyraAbilityCost>> AdditionalCosts;

	// Map of failure tags to simple error messages
	UPROPERTY(EditDefaultsOnly, Category = "Advanced")
	TMap<FGameplayTag, FText> FailureTagToUserFacingMessages;

	// Map of failure tags to anim montages that should be played with them 
	UPROPERTY(EditDefaultsOnly, Category = "Advanced")
	TMap<FGameplayTag, TObjectPtr<UAnimMontage>> FailureTagToAnimMontage;

	// If true, extra information should be logged when this ability is canceled. This is temporary, used for tracking a bug.
	UPROPERTY(EditDefaultsOnly, Category = "Advanced")
	bool bLogCancelation;

	// Current camera mode set by the ability.
	TSubclassOf<UGyraCameraMode> ActiveCameraMode;
};
