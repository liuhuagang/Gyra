// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "Components/GameFrameworkInitStateInterface.h"
#include "Components/PawnComponent.h"

#include "GyraPawnExtensionComponent.generated.h"

namespace EEndPlayReason
{
	enum Type : int;
}

class UGameFrameworkComponentManager;
class UGyraAbilitySystemComponent;
class UGyraPawnData;
class UObject;
struct FActorInitStateChangedParams;
struct FFrame;
struct FGameplayTag;

/**
 * Component that adds functionality to all Pawn classes so it can be used for characters/vehicles/etc.
 * This coordinates the initialization of other components.
 * 
 * 该组件为所有Pawn类添加了功能，使其能够用于角色/车辆等对象。
 * 此组件负责协调其他组件的初始化工作。
 * 
 */
UCLASS()
class GYRAGAME_API UGyraPawnExtensionComponent : public UPawnComponent, public IGameFrameworkInitStateInterface
{
	GENERATED_BODY()

public:
	// 构造函数
	UGyraPawnExtensionComponent(const FObjectInitializer& ObjectInitializer);

	/** The name of this overall feature, this one depends on the other named component features */
	/** 此整体功能的名称，该名称取决于其他已命名的组件功能 */
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

	/** Returns the pawn extension component if one exists on the specified actor. */
	/** 返回指定角色上存在的兵类扩展组件。*/
	UFUNCTION(BlueprintPure, Category = "Gyra|Pawn")
	static UGyraPawnExtensionComponent* FindPawnExtensionComponent(const AActor* Actor)
	{
		return (Actor ? Actor->FindComponentByClass<UGyraPawnExtensionComponent>() : nullptr);
	}

	/** Gets the pawn data, which is used to specify pawn properties in data */
	/** 获取兵卒数据，该数据用于指定兵卒的相关属性 */
	template <class T>
	const T* GetPawnData() const { return Cast<T>(PawnData); }

	/** Sets the current pawn data */
	/** 设置当前棋子数据 */
	void SetPawnData(const UGyraPawnData* InPawnData);

	/** Gets the current ability system component, which may be owned by a different actor */
	/** 获取当前的能力系统组件，该组件可能属于不同的角色 */
	UFUNCTION(BlueprintPure, Category = "Gyra|Pawn")
	UGyraAbilitySystemComponent* GetGyraAbilitySystemComponent() const { return AbilitySystemComponent; }

	/**
	 * 用于获取该角色的名字,如果对应了角色资产的话
	 * 
	 * @return 
	 */
	UFUNCTION(BlueprintPure, Category = "Gyra|Pawn")
	FName GetGyraPawnName() const;

	/** Should be called by the owning pawn to become the avatar of the ability system. */
	/** 应由拥有该能力的棋子调用，以成为能力系统的化身。*/
	void InitializeAbilitySystem(UGyraAbilitySystemComponent* InASC, AActor* InOwnerActor);

	/** Should be called by the owning pawn to remove itself as the avatar of the ability system. */
	/** 应由拥有该能力的棋子调用，以解除自身作为能力系统化身的角色身份。*/
	void UninitializeAbilitySystem();

	/** Should be called by the owning pawn when the pawn's controller changes. */
	/** 当棋子的控制者发生变化时，应由该棋子的拥有者调用此函数。*/
	void HandleControllerChanged();

	/** Should be called by the owning pawn when the player state has been replicated. */
	/** 当玩家状态被复制后，应由所属棋子调用此方法。*/
	void HandlePlayerStateReplicated();

	/** Should be called by the owning pawn when the input component is setup. */
	/** 当输入组件设置完成后，应由所属棋子调用此函数。*/
	void SetupPlayerInputComponent();

	/** Register with the OnAbilitySystemInitialized delegate and broadcast if our pawn has been registered with the ability system component */
	/** 注册到 OnAbilitySystemInitialized 事件中，并在我们的角色已与能力系统组件完成注册后进行广播 */
	void OnAbilitySystemInitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate Delegate);

	/** Register with the OnAbilitySystemUninitialized delegate fired when our pawn is removed as the ability system's avatar actor */
	/** 向 OnAbilitySystemUninitialized 事件注册，该事件会在我们的角色被当作能力系统中的角色扮演者而移除时触发 */
	void OnAbilitySystemUninitialized_Register(FSimpleMulticastDelegate::FDelegate Delegate);

protected:
	
	virtual void OnRegister() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	void OnRep_PawnData();

	/** Delegate fired when our pawn becomes the ability system's avatar actor */
	/** 当我们的兵卒成为能力系统中的角色扮演者时触发的委托事件 */
	FSimpleMulticastDelegate OnAbilitySystemInitialized;

	/** Delegate fired when our pawn is removed as the ability system's avatar actor */
	/** 当我们的棋子被能力系统视为角色扮演对象而被移除时触发的委托事件 */
	FSimpleMulticastDelegate OnAbilitySystemUninitialized;

	/** Pawn data used to create the pawn. Specified from a spawn function or on a placed instance. */
	/** 用于创建兵卒的数据。该数据可在生成函数中指定，或在放置实例时指定。*/
	UPROPERTY(EditInstanceOnly, ReplicatedUsing = OnRep_PawnData, Category = "Gyra|Pawn")
	TObjectPtr<const UGyraPawnData> PawnData;

	/** Pointer to the ability system component that is cached for convenience. */
	/** 指向用于缓存的“能力系统”组件的指针，此举旨在方便使用。*/
	UPROPERTY(Transient)
	TObjectPtr<UGyraAbilitySystemComponent> AbilitySystemComponent;
};
