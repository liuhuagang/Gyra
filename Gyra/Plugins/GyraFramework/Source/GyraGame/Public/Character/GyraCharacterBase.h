// Copyright 2025 Huagang Liu. All Rights Reserved.

#pragma once

#include "ModularCharacter.h"
#include "AbilitySystemInterface.h"
#include "GameplayCueInterface.h"
#include "GameplayTagAssetInterface.h"

#include "GyraPawnInterface.h"
#include "GyraTeamAgentInterface.h"
#include "GyraCharacterBase.generated.h"

class AActor;
class AController;

class FLifetimeProperty;
class IRepChangedPropertyTracker;
class UAbilitySystemComponent;
class UInputComponent;

class AGyraCharacterBase;
class AGyraPlayerController;
class AGyraPlayerState;

class UGyraAbilitySystemComponent;
class UGyraCameraComponent;
class UGyraHealthComponent;
class UGyraStatsComponent;
class UGyraPawnExtensionComponent;
class UGyraComboComponent;

class UObject;
struct FFrame;
struct FGameplayTag;
struct FGameplayTagContainer;

UENUM(BlueprintType)
enum class EGyraCharacterHurtDirection : uint8
{
	None UMETA(DisplayName = "Default"),
	Front UMETA(DisplayName = "Front"),
	Back UMETA(DisplayName = "Back"),
	Left UMETA(DisplayName = "Left"),
	Right UMETA(DisplayName = "Right"),
	Max UMETA(Hidden)
};

/**
 * FGyraReplicatedAcceleration: Compressed representation of acceleration
 */
USTRUCT()
struct FGyraReplicatedAcceleration
{
	GENERATED_BODY()

	UPROPERTY()
	uint8 AccelXYRadians = 0; // Direction of XY accel component, quantized to represent [0, 2*pi]

	UPROPERTY()
	uint8 AccelXYMagnitude = 0; //Accel rate of XY component, quantized to represent [0, MaxAcceleration]

	UPROPERTY()
	int8 AccelZ = 0; // Raw Z accel rate component, quantized to represent [-MaxAcceleration, MaxAcceleration]
};

/**
 * Minimal class that supports extension by project
 * 最小化依赖用于项目的人物基类
 */
UCLASS(Blueprintable)
class GYRAGAME_API AGyraCharacterBase : public AModularCharacter,
                                        public IGyraPawnInterface,
                                        public IAbilitySystemInterface,
                                        public IGameplayCueInterface,
                                        public IGameplayTagAssetInterface,
                                        public IGyraTeamAgentInterface
{
	GENERATED_BODY()

public:
	
	// 构造函数 初始化成员 变量
	AGyraCharacterBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// 获取玩家控制器,如果类型不正确会崩溃,以便检测代码访问的合理性
	UFUNCTION(BlueprintCallable, Category = "Gyra|Character")
	AGyraPlayerController* GetGyraPlayerController() const;

	// 获取玩家状态类,如果类型不正确会崩溃,以便检测代码访问的合理性
	UFUNCTION(BlueprintCallable, Category = "Gyra|Character")
	AGyraPlayerState* GetGyraPlayerState() const;

	// 获取ASC组件
	UFUNCTION(BlueprintCallable, Category = "Gyra|Character")
	UGyraAbilitySystemComponent* GetGyraAbilitySystemComponent() const;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	// Tag的转发处理
	//~IGameplayTagAssetInterface interface
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;
	virtual bool HasMatchingGameplayTag(FGameplayTag TagToCheck) const override;
	virtual bool HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
	virtual bool HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
	//~End of IGameplayTagAssetInterface interface
	
	//~AActor interface
	virtual void PreInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Reset() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker) override;
	//~End of AActor interface

	//~APawn interface
	virtual void NotifyControllerChanged() override;
	//~End of APawn interface

	//~IGyraTeamAgentInterface interface
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const override;
	virtual FOnGyraTeamIndexChangedDelegate* GetOnTeamIndexChangedDelegate() override;
	//~End of IGyraTeamAgentInterface interface

	//~IGyraPawnInterface interface
	virtual FName GetGyraPawnInitInfoName() override;
	//~End of IGyraPawnInterface interface

	void ToggleCrouch();

protected:
	virtual void OnAbilitySystemInitialized();
	virtual void OnAbilitySystemUninitialized();

	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;

	virtual void OnRep_Controller() override;
	virtual void OnRep_PlayerState() override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	void InitializeGameplayTags();

	virtual void FellOutOfWorld(const class UDamageType& dmgType) override;

	// Begins the death sequence for the character (disables collision, disables movement, etc...)
	UFUNCTION()
	virtual void OnDeathStarted(AActor* OwningActor);

	// Ends the death sequence for the character (detaches controller, destroys pawn, etc...)
	UFUNCTION()
	virtual void OnDeathFinished(AActor* OwningActor);

	void DisableMovementAndCollision();
	void DestroyDueToDeath();
	void UninitAndDestroy();

	// Called when the death sequence for the character has completed
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnDeathFinished"))
	void K2_OnDeathFinished();

	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) override;
	void SetMovementModeTag(EMovementMode MovementMode, uint8 CustomMovementMode, bool bTagEnabled);

	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

	virtual bool CanJumpInternal_Implementation() const;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gyra|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UGyraPawnExtensionComponent> PawnExtComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gyra|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UGyraStatsComponent> StatsComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gyra|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UGyraHealthComponent> HealthComponent;


protected:
	UPROPERTY()
	FOnGyraTeamIndexChangedDelegate OnTeamChangedDelegate;

	// Called to determine what happens to the team ID when possession ends
	// 用于确定当控球权结束时团队标识将如何处理的函数
	virtual FGenericTeamId DetermineNewTeamAfterPossessionEnds(FGenericTeamId OldTeamID) const;

	UFUNCTION()
	void OnRep_MyTeamID(FGenericTeamId OldTeamID);


	UFUNCTION()
	void OnRep_ReplicatedAcceleration();

	UFUNCTION()
	void OnControllerChangedTeam(UObject* TeamAgent, int32 OldTeam, int32 NewTeam);

private:
	
	UPROPERTY(ReplicatedUsing = OnRep_MyTeamID)
	FGenericTeamId MyTeamID;
	
	UPROPERTY(Transient, ReplicatedUsing = OnRep_ReplicatedAcceleration)
	FGyraReplicatedAcceleration ReplicatedAcceleration;


	
	
	
public:
	
	UFUNCTION(BlueprintPure, Category = "Gyra|Anim")
	UAnimMontage* GetDeathMontage();

	UFUNCTION(BlueprintPure, Category = "Gyra|Anim")
	UAnimMontage* GetHurtMontage(EGyraCharacterHurtDirection InHurtDirection);

	UAnimMontage* GetHurtMontage_InternalList(TArray<TObjectPtr<UAnimMontage>> InAnimList);

	UFUNCTION(BlueprintPure, Category = "Gyra|Anim")
	UAnimMontage* GetStunMontage();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gyra|Anim|Death", Meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<UAnimMontage>> DeathMontageList;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gyra|Anim|Hurt", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> DefaultHurtMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gyra|Anim|Hurt", Meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<UAnimMontage>> HurtFontMontageList;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gyra|Anim|Hurt", Meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<UAnimMontage>> HurtBackMontageList;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gyra|Anim|Hurt", Meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<UAnimMontage>> HurtLeftMontageList;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gyra|Anim|Hurt", Meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<UAnimMontage>> HurtRightMontageList;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gyra|Anim|Stun", Meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<UAnimMontage>> StunMontageList;
};
