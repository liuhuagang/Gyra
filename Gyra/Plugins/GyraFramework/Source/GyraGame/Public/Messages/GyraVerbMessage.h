// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include "GyraVerbMessage.generated.h"

// Represents a generic message of the form Instigator Verb Target (in Context, with Magnitude)
// 表示启动动词目标（在上下文中，带有大小）形式的通用消息
USTRUCT(BlueprintType)
struct FGyraVerbMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = Gameplay)
	FGameplayTag Verb;

	UPROPERTY(BlueprintReadWrite, Category = Gameplay)
	TObjectPtr<UObject> Instigator = nullptr;

	UPROPERTY(BlueprintReadWrite, Category = Gameplay)
	TObjectPtr<UObject> Target = nullptr;

	UPROPERTY(BlueprintReadWrite, Category = Gameplay)
	FGameplayTagContainer InstigatorTags;

	UPROPERTY(BlueprintReadWrite, Category = Gameplay)
	FGameplayTagContainer TargetTags;

	UPROPERTY(BlueprintReadWrite, Category = Gameplay)
	FGameplayTagContainer ContextTags;

	UPROPERTY(BlueprintReadWrite, Category = Gameplay)
	double Magnitude = 1.0;

	//用于物品拾取时确认唯一主键
	UPROPERTY(BlueprintReadWrite, Category = Gameplay)
	FString TargetItemName = TEXT("");



	// Returns a debug string representation of this message
	// 返回此消息的调试字符串表示形式
	GYRAGAME_API FString ToString() const;
};
