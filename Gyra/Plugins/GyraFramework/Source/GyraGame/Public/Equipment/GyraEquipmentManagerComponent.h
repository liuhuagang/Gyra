// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "AbilitySystem/GyraAbilitySet.h"
#include "Components/PawnComponent.h"
#include "Net/Serialization/FastArraySerializer.h"

#include "GyraEquipmentManagerComponent.generated.h"

class UActorComponent;
class UGyraAbilitySystemComponent;
class UGyraInventoryInstance;
class UGyraEquipmentDefinition;
class UGyraEquipmentInstance;
class UGyraEquipmentManagerComponent;
class UObject;
struct FFrame;
struct FGyraEquipmentList;
struct FNetDeltaSerializeInfo;
struct FReplicationFlags;

/** A single piece of applied equipment */
USTRUCT(BlueprintType)
struct FGyraAppliedEquipmentEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

	FGyraAppliedEquipmentEntry()
	{}

	FString GetDebugString() const;

private:
	friend FGyraEquipmentList;
	friend UGyraEquipmentManagerComponent;

	// The equipment class that got equipped
	UPROPERTY()
	TSubclassOf<UGyraEquipmentDefinition> EquipmentDefinition;

	UPROPERTY()
	TObjectPtr<UGyraEquipmentInstance> Instance = nullptr;

	// Authority-only list of granted handles
	UPROPERTY(NotReplicated)
	FGyraAbilitySet_GrantedHandles GrantedHandles;
};

/** List of applied equipment */
USTRUCT(BlueprintType)
struct FGyraEquipmentList : public FFastArraySerializer
{
	GENERATED_BODY()

	FGyraEquipmentList()
		: OwnerComponent(nullptr)
	{
	}

	FGyraEquipmentList(UActorComponent* InOwnerComponent)
		: OwnerComponent(InOwnerComponent)
	{
	}

public:
	//~FFastArraySerializer contract
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);
	//~End of FFastArraySerializer contract

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FGyraAppliedEquipmentEntry, FGyraEquipmentList>(Entries, DeltaParms, *this);
	}

	UGyraEquipmentInstance* AddEntry(TSubclassOf<UGyraEquipmentDefinition> EquipmentDefinition,const TMap<FGameplayTag, float>& InAttributeMap);
	void RemoveEntry(UGyraEquipmentInstance* Instance);

private:
	UGyraAbilitySystemComponent* GetAbilitySystemComponent() const;

	friend UGyraEquipmentManagerComponent;

private:
	// Replicated list of equipment entries
	UPROPERTY()
	TArray<FGyraAppliedEquipmentEntry> Entries;

	UPROPERTY(NotReplicated)
	TObjectPtr<UActorComponent> OwnerComponent;
};

template<>
struct TStructOpsTypeTraits<FGyraEquipmentList> : public TStructOpsTypeTraitsBase2<FGyraEquipmentList>
{
	enum { WithNetDeltaSerializer = true };
};










/**
 * Manages equipment applied to a pawn
 * 管理角色装备
 */
UCLASS(BlueprintType, Const)
class UGyraEquipmentManagerComponent : public UPawnComponent
{
	GENERATED_BODY()

public:
	UGyraEquipmentManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	UGyraEquipmentInstance* EquipItem(TSubclassOf<UGyraEquipmentDefinition> EquipmentDefinition, const TMap<FGameplayTag, float>& InAttributeMap);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void UnequipItem(UGyraEquipmentInstance* ItemInstance);

	//~UObject interface
	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	//~End of UObject interface

	//~UActorComponent interface
	//virtual void EndPlay() override;
	virtual void InitializeComponent() override;
	virtual void UninitializeComponent() override;
	virtual void ReadyForReplication() override;
	//~End of UActorComponent interface

	/** Returns the first equipped instance of a given type, or nullptr if none are found */
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UGyraEquipmentInstance* GetFirstInstanceOfType(TSubclassOf<UGyraEquipmentInstance> InstanceType);

 	/**
 	 * Returns all equipped instances of a given type, or an empty array if none are found
 	 * 返回给定类型的所有已配置实例，如果没有找到则返回空数组
 	 * @param InstanceType
 	 * @return 
 	 */
 	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<UGyraEquipmentInstance*> GetEquipmentInstancesOfType(TSubclassOf<UGyraEquipmentInstance> InstanceType) const;

	template <typename T>
	T* GetFirstInstanceOfType()
	{
		return (T*)GetFirstInstanceOfType(T::StaticClass());
	}

private:
	UPROPERTY(Replicated)
	FGyraEquipmentList EquipmentList;
};
