// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "Components/ActorComponent.h"
#include "GameFramework/GameplayMessageSubsystem.h"

#include "GameplayMessageProcessor.generated.h"

namespace EEndPlayReason { enum Type : int; }

class UObject;

/**
 * UGameplayMessageProcessor
 *
 * Base class for any message processor which observes other gameplay messages
 * and potentially re-emits updates (e.g., when a chain or combo is detected)
 * 用于任何消息处理器的基类，用于观察其他游戏玩法信息并可能重新发出更新（例如，当检测到链或组合时）
 *
 * Note that these processors are spawned on the server once (not per player)
 * and should do their own internal filtering if only relevant for some players.
 * 请注意，这些处理器只在服务器上生成一次（而不是每个玩家），如果只与某些玩家相关，则应该执行自己的内部过滤。
 */
UCLASS(BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
class GYRAGAME_API UGameplayMessageProcessor : public UActorComponent
{
	GENERATED_BODY()

public:
	//~UActorComponent interface
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~End of UActorComponent interface

	virtual void StartListening();
	virtual void StopListening();

protected:
	void AddListenerHandle(FGameplayMessageListenerHandle&& Handle);
	double GetServerTime() const;

private:
	TArray<FGameplayMessageListenerHandle> ListenerHandles;
};
