// Copyright 2024 Huagang Liu. All Rights Reserved.

#include "GyraVerbMessageReplication.h"

#include "GameFramework/GameplayMessageSubsystem.h"
#include "GyraVerbMessage.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GyraVerbMessageReplication)

//////////////////////////////////////////////////////////////////////
// FGyraVerbMessageReplicationEntry

FString FGyraVerbMessageReplicationEntry::GetDebugString() const
{
	return Message.ToString();
}

//////////////////////////////////////////////////////////////////////
// FGyraVerbMessageReplication

void FGyraVerbMessageReplication::AddMessage(const FGyraVerbMessage& Message)
{
	FGyraVerbMessageReplicationEntry& NewStack = CurrentMessages.Emplace_GetRef(Message);
	MarkItemDirty(NewStack);
}

void FGyraVerbMessageReplication::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	// 	for (int32 Index : RemovedIndices)
	// 	{
	// 		const FGameplayTag Tag = CurrentMessages[Index].Tag;
	// 		TagToCountMap.Remove(Tag);
	// 	}
}

void FGyraVerbMessageReplication::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	for (int32 Index : AddedIndices)
	{
		const FGyraVerbMessageReplicationEntry& Entry = CurrentMessages[Index];
		RebroadcastMessage(Entry.Message);
	}
}

void FGyraVerbMessageReplication::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	for (int32 Index : ChangedIndices)
	{
		const FGyraVerbMessageReplicationEntry& Entry = CurrentMessages[Index];
		RebroadcastMessage(Entry.Message);
	}
}

void FGyraVerbMessageReplication::RebroadcastMessage(const FGyraVerbMessage& Message)
{
	check(Owner);
	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(Owner);
	MessageSystem.BroadcastMessage(Message.Verb, Message);
}

