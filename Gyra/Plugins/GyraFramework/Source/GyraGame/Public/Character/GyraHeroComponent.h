// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "Components/GameFrameworkInitStateInterface.h"
#include "Components/PawnComponent.h"
#include "GameFeatureAction_AddInputContextMapping.h"
#include "GameplayAbilitySpecHandle.h"

#include "GyraHeroComponent.generated.h"

namespace EEndPlayReason
{
	enum Type : int;
}

class UGameFrameworkComponentManager;
class UInputComponent;
class UGyraCameraMode;
class UGyraInputConfig;
class UObject;
struct FActorInitStateChangedParams;
struct FFrame;
struct FGameplayTag;
struct FInputActionValue;

/**
 * Component that sets up input and camera handling for player controlled pawns (or bots that simulate players).
 * This depends on a PawnExtensionComponent to coordinate initialization.
 * 为玩家控制的小兵（或模拟玩家的机器人）设置输入和镜头处理的组件。
 * 这取决于一个PawnExtensionComponent来协调初始化。
 * 
 */
UCLASS(Blueprintable, Meta = (BlueprintSpawnableComponent))
class GYRAGAME_API UGyraHeroComponent : public UPawnComponent, public IGameFrameworkInitStateInterface
{
	GENERATED_BODY()

public:
	UGyraHeroComponent(const FObjectInitializer& ObjectInitializer);

	/** Returns the hero component if one exists on the specified actor. */
	/** 返回指定角色所拥有的英雄组件（如果存在的话）。*/
	UFUNCTION(BlueprintPure, Category = "Gyra|Hero")
	static UGyraHeroComponent* FindHeroComponent(const AActor* Actor);

	/** Overrides the camera from an active gameplay ability */
	/** 替换当前游戏中的活跃技能所使用的摄像机 */
	void SetAbilityCameraMode(TSubclassOf<UGyraCameraMode> CameraMode,
	                          const FGameplayAbilitySpecHandle& OwningSpecHandle);

	/** Clears the camera override if it is set */
	/** 清除相机的自定义设置（如果已设置的话） */
	void ClearAbilityCameraMode(const FGameplayAbilitySpecHandle& OwningSpecHandle);

	/** Adds mode-specific input config */
	/** 添加模式特定的输入配置 */
	void AddAdditionalInputConfig(const UGyraInputConfig* InputConfig);

	/** Removes a mode-specific input config if it has been added */
	/** 如果已添加了特定模式的输入配置，则将其删除 */
	void RemoveAdditionalInputConfig(const UGyraInputConfig* InputConfig);

	/** True if this is controlled by a real player and has progressed far enough in initialization where additional input bindings can be added */
	/** 如果此内容是由真实玩家控制的，并且在初始化过程中已经进展到足够深入的阶段，以便能够添加更多的输入绑定，则返回 true */
	bool IsReadyToBindInputs() const;

	/** The name of the extension event sent via UGameFrameworkComponentManager when ability inputs are ready to bind */
	/** 当能力输入准备好进行绑定时，通过 UGameFrameworkComponentManager 发送的扩展事件的名称 */
	static const FName NAME_BindInputsNow;

	/** The name of this component-implemented feature */
	/** 此组件实现的功能的名称 */
	static const FName NAME_ActorFeatureName;

	//~ Begin IGameFrameworkInitStateInterface interface
	virtual FName GetFeatureName() const override ;
	virtual bool CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState,
	                                FGameplayTag DesiredState) const override;
	virtual void HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState,
	                                   FGameplayTag DesiredState) override;
	virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) override;
	virtual void CheckDefaultInitialization() override;
	//~ End IGameFrameworkInitStateInterface interface

protected:
	virtual void OnRegister() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void InitializePlayerInput(UInputComponent* PlayerInputComponent);

	void Input_AbilityInputTagPressed(FGameplayTag InputTag);
	void Input_AbilityInputTagReleased(FGameplayTag InputTag);

	void Input_Move(const FInputActionValue& InputActionValue);
	void Input_LookMouse(const FInputActionValue& InputActionValue);
	void Input_LookStick(const FInputActionValue& InputActionValue);
	void Input_Crouch(const FInputActionValue& InputActionValue);
	void Input_AutoRun(const FInputActionValue& InputActionValue);

	TSubclassOf<UGyraCameraMode> DetermineCameraMode() const;

protected:

	UPROPERTY(EditAnywhere)
	TArray<FInputMappingContextAndPriority> DefaultInputMappings;

	/** Camera mode set by an ability. */
	UPROPERTY()
	TSubclassOf<UGyraCameraMode> AbilityCameraMode;

	/** Spec handle for the last ability to set a camera mode. */
	FGameplayAbilitySpecHandle AbilityCameraModeOwningSpecHandle;

	/** True when player input bindings have been applied, will never be true for non - players */
	bool bReadyToBindInputs;
};
