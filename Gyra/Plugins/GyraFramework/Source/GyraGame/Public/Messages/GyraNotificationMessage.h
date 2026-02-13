// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "NativeGameplayTags.h"

#include "GyraNotificationMessage.generated.h"

class UObject;

GYRAGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gyra_AddNotification_Message);

class APlayerState;

// A message destined for a transient log (e.g., an elimination feed or inventory pickup stream)
// 用于临时日志的消息（例如，消除馈送或库存拾取流）
USTRUCT(BlueprintType)
struct GYRAGAME_API FGyraNotificationMessage
{
	GENERATED_BODY()

	// The destination channel
	// 目标通道
	UPROPERTY(BlueprintReadWrite, Category = Notification)
	FGameplayTag TargetChannel;

	// The target player (if none is set then it will display for all local players)
	// 目标播放器（如果没有设置则显示所有本地播放器）
	UPROPERTY(BlueprintReadWrite, Category = Notification)
	TObjectPtr<APlayerState> TargetPlayer = nullptr;

	// The message to display
	// 要显示的消息
	UPROPERTY(BlueprintReadWrite, Category = Notification)
	FText PayloadMessage;
	// Extra payload specific to the target channel (e.g., a style or definition asset)
	// 特定于目标通道的额外负载（例如，样式或定义资产）
	UPROPERTY(BlueprintReadWrite, Category = Notification)
	FGameplayTag PayloadTag;

	// Extra payload specific to the target channel (e.g., a style or definition asset)
	// 特定于目标通道的额外负载（例如，样式或定义资产）
	UPROPERTY(BlueprintReadWrite, Category = Notification)
	TObjectPtr<UObject> PayloadObject = nullptr;
};
