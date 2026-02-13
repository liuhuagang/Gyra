// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "GameplayTagContainer.h"
#include "Net/Serialization/FastArraySerializer.h"

#include "GyraVerbMessage.h"

#include "GyraVerbMessageReplication.generated.h"

class UObject;
struct FGyraVerbMessageReplication;
struct FNetDeltaSerializeInfo;

/**
 * Represents one verb message
 * 表示一个动词消息
 */
USTRUCT(BlueprintType)
struct FGyraVerbMessageReplicationEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

	FGyraVerbMessageReplicationEntry()
	{}

	FGyraVerbMessageReplicationEntry(const FGyraVerbMessage& InMessage)
		: Message(InMessage)
	{
	}

	FString GetDebugString() const;

private:
	friend FGyraVerbMessageReplication;

	UPROPERTY()
	FGyraVerbMessage Message;
};

/** Container of verb messages to replicate */
/**要复制的动词消息的容器*/
USTRUCT(BlueprintType)
struct FGyraVerbMessageReplication : public FFastArraySerializer
{
	GENERATED_BODY()

	FGyraVerbMessageReplication()
	{
	}

public:
	void SetOwner(UObject* InOwner) { Owner = InOwner; }

	// Broadcasts a message from server to clients
	void AddMessage(const FGyraVerbMessage& Message);

	//~FFastArraySerializer contract
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);
	//~End of FFastArraySerializer contract

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FGyraVerbMessageReplicationEntry, FGyraVerbMessageReplication>(CurrentMessages, DeltaParms, *this);
	}

private:
	void RebroadcastMessage(const FGyraVerbMessage& Message);

private:
	// Replicated list of gameplay tag stacks
	UPROPERTY()
	TArray<FGyraVerbMessageReplicationEntry> CurrentMessages;

	// Owner (for a route to a world)
	UPROPERTY()
	TObjectPtr<UObject> Owner = nullptr;
};

template<>
struct TStructOpsTypeTraits<FGyraVerbMessageReplication> : public TStructOpsTypeTraitsBase2<FGyraVerbMessageReplication>
{
	enum
	{
		WithNetDeltaSerializer = true,
	};
};
