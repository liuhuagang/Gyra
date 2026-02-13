// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Components/PawnComponent.h"
#include "Components/GameFrameworkInitStateInterface.h"

#include "GyraHealthSet.h"
#include "GyraMagicSet.h"
#include "GyraPawnInitInfo.h"
#include "GyraStatsSet.h"

#include "GameplayTagContainer.h"
#include "GyraStatsComponent.generated.h"

class UGyraStatsComponent;
class UGyraAbilitySystemComponent;

class UHealthSet;

class UGyraMagicSet;
class UGyraStatsSet;
struct FGyraPawnInitInfo;


class UObject;
struct FFrame;
struct FGameplayEffectSpec;
struct FDataRegistryAcquireResult;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FGyraStats_StatsAttributeChanged,
                                              UGyraStatsComponent*, StatsComponent,
                                              FGameplayTag, GameplayTag_Attirbute,
                                              float, OldValue,
                                              float, NewValue, AActor*, Instigator);

/**
 * UGyraStatsComponent
 * 
 * 这个组件主要获取人物所拥有的各类信息,可以为后续战斗伤害计算提供原始数据
 *  
 */
UCLASS(Blueprintable, Meta=(BlueprintSpawnableComponent))
class GYRAGAME_API UGyraStatsComponent : public UPawnComponent, public IGameFrameworkInitStateInterface
{
	GENERATED_BODY()

public:
	UGyraStatsComponent(const FObjectInitializer& ObjectInitializer);

	// Returns the health component if one exists on the specified actor.
	// 获取该对象身上的人物信息计算组件
	UFUNCTION(BlueprintPure, Category = "Gyra|Stats")
	static UGyraStatsComponent* FindStatsComponent(const AActor* Actor);

	// Initialize the component using an ability system component.
	// 使用ASC初始化组件
	UFUNCTION(BlueprintCallable, Category = "Gyra|Stats")
	void InitializeWithAbilitySystem(UGyraAbilitySystemComponent* InASC);

	// Uninitialize the component, clearing any references to the ability system.
	// 反注册该组件,清理跟ASC相关信息
	UFUNCTION(BlueprintCallable, Category = "Gyra|Stats")
	void UninitializeFromAbilitySystem();

	// 暴露给蓝图,获取状态相关属性值
	UFUNCTION(BlueprintCallable, Category = "Gyra|Stats")
	float GetStats(FGameplayTag InGameplayTag_Attribute) const;

public:
	/** The name of this component-implemented feature */
	/** 此组件实现的功能的名称 */
	static const FName NAME_ActorFeatureName;

	//~ Begin IGameFrameworkInitStateInterface interface
	virtual FName GetFeatureName() const override;
	virtual bool CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState,
	                                FGameplayTag DesiredState) const override;
	virtual void HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState,
	                                   FGameplayTag DesiredState) override;
	virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) override;
	virtual void CheckDefaultInitialization() override;
	//~ End IGameFrameworkInitStateInterface interface


public:
	// 暴露给蓝图 以便接收属性值的变化
	UPROPERTY(BlueprintAssignable)
	FGyraStats_StatsAttributeChanged OnChanged_StatsAttribute;

protected:
	
	virtual void HandleChanged_StatsAttribute(AActor* DamageInstigator, AActor* DamageCauser,
	                                          const FGameplayEffectSpec* DamageEffectSpec,
	                                          FGameplayTag GameplayTag_Attribute, float DamageMagnitude, float OldValue,
	                                          float NewValue);

	void OnRegistryLoadCompleted(const FDataRegistryAcquireResult& AccoladeHandle);

protected:
	
	virtual void OnRegister() override;
	virtual void OnUnregister() override;

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
	// Ability system used by this component.
	// ASC组件
	UPROPERTY()
	TObjectPtr<UGyraAbilitySystemComponent> AbilitySystemComponent;


	// Health set used by this component.
	UPROPERTY()
	TObjectPtr<const UGyraHealthSet> HealthSet;

	// Magic set used by this component.
	UPROPERTY()
	TObjectPtr<const UGyraMagicSet> MagicSet;

	// Stats set used by this component.
	UPROPERTY()
	TObjectPtr<const UGyraStatsSet> StatsSet;

	// 是否已经申请过读取数据表
	bool bApplyForInit = false;
	// 数据表是否已经异步读取完成
	bool bPawnAttributeInit = false;

	// 缓存的初始化读表信息
	FGyraPawnInitInfo PawnInitInfo;
};
