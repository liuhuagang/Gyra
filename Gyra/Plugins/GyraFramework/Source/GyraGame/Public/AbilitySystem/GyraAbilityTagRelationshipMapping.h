// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"

#include "GyraAbilityTagRelationshipMapping.generated.h"


class UObject;

/** Struct that defines the relationship between different ability tags */
/** 结构，该结构定义了不同能力标签之间的关系 */
USTRUCT()
struct FGyraAbilityTagRelationship
{
	GENERATED_BODY()

	/** The tag that this container relationship is about. Single tag, but abilities can have multiple of these */
	/** 这个容器关系所涉及的标签。一个标签，但是能力可以有多个标签 */
	UPROPERTY(EditAnywhere, Category = Ability, meta = (Categories = "Gameplay.Action"))
	FGameplayTag AbilityTag;

	/** The other ability tags that will be blocked by any ability using this tag */
	/** 其他能力标签将被任何使用这个标签的能力所阻挡 */
	UPROPERTY(EditAnywhere, Category = Ability)
	FGameplayTagContainer AbilityTagsToBlock;

	/** The other ability tags that will be canceled by any ability using this tag */
	/** 其他能力标签将被任何使用此标签的能力取消 */
	UPROPERTY(EditAnywhere, Category = Ability)
	FGameplayTagContainer AbilityTagsToCancel;

	/** If an ability has the tag, this is implicitly added to the activation required tags of the ability */
	/** 如果一个能力有标签，它会隐式地添加到该能力的激活所需标签中 */
	UPROPERTY(EditAnywhere, Category = Ability)
	FGameplayTagContainer ActivationRequiredTags;

	/** If an ability has the tag, this is implicitly added to the activation blocked tags of the ability */
	/** 如果一个能力有这个标签，它会被隐式地添加到该能力的激活阻塞标签中 */
	UPROPERTY(EditAnywhere, Category = Ability)
	FGameplayTagContainer ActivationBlockedTags;
};


/** Mapping of how ability tags block or cancel other abilities */
/** 能力标签如何阻碍或取消其他能力的映射 */
UCLASS()
class UGyraAbilityTagRelationshipMapping : public UDataAsset
{
	GENERATED_BODY()

private:

	/** The list of relationships between different gameplay tags (which ones block or cancel others) */
	/** 不同玩法标签之间的关系列表（哪些标签会阻止或取消其他标签） */
	UPROPERTY(EditAnywhere, Category = Ability, meta = (TitleProperty = "AbilityTag"))
	TArray<FGyraAbilityTagRelationship> AbilityTagRelationships;

public:

	/** Given a set of ability tags, parse the tag relationship and fill out tags to block and cancel */
	/** 给定一组功能标记，解析标记关系并填写标记以阻止和取消 */
	void GetAbilityTagsToBlockAndCancel(const FGameplayTagContainer& AbilityTags, FGameplayTagContainer* OutTagsToBlock, FGameplayTagContainer* OutTagsToCancel) const;


	/** Given a set of ability tags, add additional required and blocking tags */
	/** 给定一组能力标签，添加额外的必需和阻塞标签 */
	void GetRequiredAndBlockedActivationTags(const FGameplayTagContainer& AbilityTags, FGameplayTagContainer* OutActivationRequired, FGameplayTagContainer* OutActivationBlocked) const;


	/** Returns true if the specified ability tags are canceled by the passed in action tag */
	/** 如果指定的能力标签被传入的动作标签取消，则返回true */
	bool IsAbilityCancelledByTag(const FGameplayTagContainer& AbilityTags, const FGameplayTag& ActionTag) const;

};




