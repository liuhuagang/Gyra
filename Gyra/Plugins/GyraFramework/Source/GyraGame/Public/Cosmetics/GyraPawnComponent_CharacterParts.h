// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "Components/PawnComponent.h"
#include "GyraCosmeticAnimationTypes.h"
#include "GyraCharacterPartTypes.h"
#include "Net/Serialization/FastArraySerializer.h"

#include "GyraPawnComponent_CharacterParts.generated.h"

class UGyraPawnComponent_CharacterParts;
namespace EEndPlayReason { enum Type : int; }
struct FGameplayTag;
struct FGyraCharacterPartList;

class AActor;
class UChildActorComponent;
class UObject;
class USceneComponent;
class USkeletalMeshComponent;
struct FFrame;
struct FNetDeltaSerializeInfo;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGyraSpawnedCharacterPartsChanged, UGyraPawnComponent_CharacterParts*, ComponentWithChangedParts);


/** A component that spawns an Actor when registered, and destroys it when unregistered.*/
UCLASS(BlueprintType,Blueprintable,meta=(BlueprintSpawnableComponent))
class UGyraChildActorComponent : public UChildActorComponent
{
	GENERATED_BODY()
};

//////////////////////////////////////////////////////////////////////

// A single applied character part
USTRUCT()
struct FGyraAppliedCharacterPartEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

	FGyraAppliedCharacterPartEntry()
	{}

	FString GetDebugString() const;

private:
	friend FGyraCharacterPartList;
	friend UGyraPawnComponent_CharacterParts;

private:
	// The character part being represented
	// 所展示的字符部分
	UPROPERTY()
	FGyraCharacterPart Part;

	// Handle index we returned to the user (server only)
	// 处理我们向用户返回的索引（仅限服务器端）
	UPROPERTY(NotReplicated)
	int32 PartHandle = INDEX_NONE;

	// The spawned actor instance (client only)
	// 生成的演员实例（仅限客户端）
	UPROPERTY(NotReplicated)
	TObjectPtr<UChildActorComponent> SpawnedComponent = nullptr;
	
};

//////////////////////////////////////////////////////////////////////

// Replicated list of applied character parts
USTRUCT(BlueprintType)
struct FGyraCharacterPartList : public FFastArraySerializer
{
	GENERATED_BODY()

	FGyraCharacterPartList()
		: OwnerComponent(nullptr)
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
		return FFastArraySerializer::FastArrayDeltaSerialize<FGyraAppliedCharacterPartEntry, FGyraCharacterPartList>(Entries, DeltaParms, *this);
	}

	FGyraCharacterPartHandle AddEntry(FGyraCharacterPart NewPart);
	void RemoveEntry(FGyraCharacterPartHandle Handle);
	void ClearAllEntries(bool bBroadcastChangeDelegate);

	FGameplayTagContainer CollectCombinedTags() const;

	void SetOwnerComponent(UGyraPawnComponent_CharacterParts* InOwnerComponent)
	{
		OwnerComponent = InOwnerComponent;
	}
	
private:
	friend UGyraPawnComponent_CharacterParts;

	bool SpawnActorForEntry(FGyraAppliedCharacterPartEntry& Entry);
	bool DestroyActorForEntry(FGyraAppliedCharacterPartEntry& Entry);

private:
	// Replicated list of equipment entries
	UPROPERTY()
	TArray<FGyraAppliedCharacterPartEntry> Entries;

	// The component that contains this list
	UPROPERTY(NotReplicated)
	TObjectPtr<UGyraPawnComponent_CharacterParts> OwnerComponent;

	// Upcounter for handles
	int32 PartHandleCounter = 0;
};

template<>
struct TStructOpsTypeTraits<FGyraCharacterPartList> : public TStructOpsTypeTraitsBase2<FGyraCharacterPartList>
{
	enum { WithNetDeltaSerializer = true };
};

//////////////////////////////////////////////////////////////////////

// A component that handles spawning cosmetic actors attached to the owner pawn on all clients
UCLASS(meta=(BlueprintSpawnableComponent))
class UGyraPawnComponent_CharacterParts : public UPawnComponent
{
	GENERATED_BODY()

public:
	UGyraPawnComponent_CharacterParts(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~UActorComponent interface
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void OnRegister() override;
	//~End of UActorComponent interface

	// Adds a character part to the actor that owns this customization component, should be called on the authority only
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Cosmetics)
	FGyraCharacterPartHandle AddCharacterPart(const FGyraCharacterPart& NewPart);

	// Removes a previously added character part from the actor that owns this customization component, should be called on the authority only
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Cosmetics)
	void RemoveCharacterPart(FGyraCharacterPartHandle Handle);

	// Removes all added character parts, should be called on the authority only
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Cosmetics)
	void RemoveAllCharacterParts();

	// Gets the list of all spawned character parts from this component
	UFUNCTION(BlueprintCallable, BlueprintPure=false, BlueprintCosmetic, Category=Cosmetics)
	TArray<AActor*> GetCharacterPartActors() const;

	// If the parent actor is derived from ACharacter, returns the Mesh component, otherwise nullptr
	USkeletalMeshComponent* GetParentMeshComponent() const;

	// Returns the scene component to attach the spawned actors to
	// If the parent actor is derived from ACharacter, we'll use the Mesh component, otherwise the root component
	USceneComponent* GetSceneComponentToAttachTo() const;

	// Returns the set of combined gameplay tags from attached character parts, optionally filtered to only tags that start with the specified root
	UFUNCTION(BlueprintCallable, BlueprintPure=false, BlueprintCosmetic, Category=Cosmetics)
	FGameplayTagContainer GetCombinedTags(FGameplayTag RequiredPrefix) const;

	void BroadcastChanged();

public:
	// Delegate that will be called when the list of spawned character parts has changed
	UPROPERTY(BlueprintAssignable, Category=Cosmetics, BlueprintCallable)
	FGyraSpawnedCharacterPartsChanged OnCharacterPartsChanged;

	UPROPERTY(BlueprintReadWrite,EditDefaultsOnly, Category=Cosmetics)
	TSubclassOf<UChildActorComponent> ChildActorComponentClass;
	
private:
	// List of character parts
	UPROPERTY(Replicated, Transient)
	FGyraCharacterPartList CharacterPartList;

	// Rules for how to pick a body style mesh for animation to play on, based on character part cosmetics tags
	UPROPERTY(EditAnywhere, Category=Cosmetics)
	FGyraAnimBodyStyleSelectionSet BodyMeshes;
};
