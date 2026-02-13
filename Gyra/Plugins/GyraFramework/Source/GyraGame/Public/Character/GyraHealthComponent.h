// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "Components/GameFrameworkComponent.h"

#include "GyraHealthComponent.generated.h"

class UGyraHealthComponent;

class UGyraAbilitySystemComponent;
class UGyraHealthSet;
class UObject;
struct FFrame;
struct FGameplayEffectSpec;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGyraHealth_DeathEvent, AActor*, OwningActor);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FGyraHealth_AttributeChanged, UGyraHealthComponent*, HealthComponent,
                                              float, OldValue, float, NewValue, AActor*, Instigator);

/**
 * EGyraDeathState
 *
 *	Defines current state of death.
 *	定义了死亡的状态
 */
UENUM(BlueprintType)
enum class EGyraDeathState : uint8
{
	NotDead = 0,
	DeathStarted,
	DeathFinished
};


/**
 * UGyraHealthComponent
 *
 *	An actor component used to handle anything related to health.
 *	一个用于处理与健康相关的一切事务的演员组件。
 */
UCLASS(Blueprintable, Meta=(BlueprintSpawnableComponent))
class GYRAGAME_API UGyraHealthComponent : public UGameFrameworkComponent
{
	GENERATED_BODY()

public:
	UGyraHealthComponent(const FObjectInitializer& ObjectInitializer);

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	// Returns the health component if one exists on the specified actor.
	// 若指定的角色对象上存在健康组件，则返回该组件。
	UFUNCTION(BlueprintPure, Category = "Gyra|Health")
	static UGyraHealthComponent* FindHealthComponent(const AActor* Actor);


	// Initialize the component using an ability system component.
	// 使用能力系统组件来初始化该组件。
	UFUNCTION(BlueprintCallable, Category = "Gyra|Health")
	void InitializeWithAbilitySystem(UGyraAbilitySystemComponent* InASC);

	// Uninitialize the component, clearing any references to the ability system.
	// 释放组件资源，清除对能力系统的所有引用。
	UFUNCTION(BlueprintCallable, Category = "Gyra|Health")
	void UninitializeFromAbilitySystem();

	// Returns the current health value.
	// 返回当前的生命值。
	UFUNCTION(BlueprintCallable, Category = "Gyra|Health")
	float GetHealth() const;

	// Returns the current maximum health value.
	// 返回当前的最大生命值。
	UFUNCTION(BlueprintCallable, Category = "Gyra|Health")
	float GetMaxHealth() const;

	// Returns the current health in the range [0.0, 1.0].
	// 返回当前的生命值，范围在 [0.0, 1.0] 之间。
	UFUNCTION(BlueprintCallable, Category = "Gyra|Health")
	float GetHealthNormalized() const;

	// 获取当前死亡状态
	UFUNCTION(BlueprintCallable, Category = "Gyra|Health")
	EGyraDeathState GetDeathState() const ;

	// 是否已经死亡或者正在死亡
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Gyra|Health",
		Meta = (ExpandBoolAsExecs = "ReturnValue"))
	bool IsDeadOrDying() const ;

	// Begins the death sequence for the owner.
	// 开始主人的死亡过程。
	virtual void StartDeath();

	// Ends the death sequence for the owner.
	// 结束主人的死亡过程。
	virtual void FinishDeath();

	// Applies enough damage to kill the owner.
	// 对目标造成足够的伤害以使其死亡。
	virtual void DamageSelfDestruct(bool bFellOutOfWorld = false);

public:
	
	// Delegate fired when the health value has changed. This is called on the client but the instigator may not be valid
	// 当生命值发生变化时触发此委托。该委托在客户端被调用，但发起者可能无效。
	UPROPERTY(BlueprintAssignable)
	FGyraHealth_AttributeChanged OnHealthChanged;

	// Delegate fired when the max health value has changed. This is called on the client but the instigator may not be valid
	// 当最大生命值发生变化时触发此委托。该委托在客户端被调用，但发起者可能无效。
	UPROPERTY(BlueprintAssignable)
	FGyraHealth_AttributeChanged OnMaxHealthChanged;

	// Delegate fired when the death sequence has started.
	// 当死亡序列开始时触发此委托。
	UPROPERTY(BlueprintAssignable)
	FGyraHealth_DeathEvent OnDeathStarted;

	// Delegate fired when the death sequence has finished.
	// 当死亡序列结束时触发此委托。
	UPROPERTY(BlueprintAssignable)
	FGyraHealth_DeathEvent OnDeathFinished;

protected:
	virtual void OnUnregister() override;

	void ClearGameplayTags();

	virtual void HandleHealthChanged(AActor* DamageInstigator, AActor* DamageCauser,
	                                 const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue,
	                                 float NewValue);
	virtual void HandleMaxHealthChanged(AActor* DamageInstigator, AActor* DamageCauser,
	                                    const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude,
	                                    float OldValue, float NewValue);
	virtual void HandleOutOfHealth(AActor* DamageInstigator, AActor* DamageCauser,
	                               const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue,
	                               float NewValue);

	UFUNCTION()
	virtual void OnRep_DeathState(EGyraDeathState OldDeathState);

protected:
	
	// Ability system used by this component.
	// 该组件所使用的能力系统。
	UPROPERTY()
	TObjectPtr<UGyraAbilitySystemComponent> AbilitySystemComponent;

	// Health set used by this component.
	// 本组件所使用的健康状态设置。
	UPROPERTY()
	TObjectPtr<const UGyraHealthSet> HealthSet;

	// Replicated state used to handle dying.
	// 用于处理对象死亡情况的复制状态。
	UPROPERTY(ReplicatedUsing = OnRep_DeathState)
	EGyraDeathState DeathState;
};
