// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "GameplayTagContainer.h"
#include "GyraInventoryItemDefinition.h"

#include "InventoryFragment_RandomStats.generated.h"

class UGyraInventoryItemInstance;
class UObject;
struct FGameplayTag;


USTRUCT(BlueprintType)
struct FGyraItemInstanceTagDefiniton
{
	GENERATED_BODY()

	//属性tag
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment|RandomTags")
	FGameplayTag SetbyCallerTag;

	//是否参与随机
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment|RandomTags")
	bool bEnableInRandom =true;

	//最小值
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment|RandomTags")
	int32 Min = 1 ;

	//最大值
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment|RandomTags")
	int32 Max = 10;

	bool operator==(const FGyraItemInstanceTagDefiniton& Other) const
	{

		return SetbyCallerTag == Other.SetbyCallerTag ? true : false;
	}

	bool operator==(const FGameplayTag& Other) const
	{

		return SetbyCallerTag == Other ? true : false;
	}
};

/**
 * 
 * 用来定义物品的随机词缀
 *  
 */
UCLASS()
class UInventoryFragment_RandomStats : public UGyraInventoryItemFragment
{
	GENERATED_BODY()

public:
	
	UInventoryFragment_RandomStats();


protected:


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment|RandomTags")
	int32 RandomNum = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment|RandomTags")
	bool bForceTagFullValue =false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment|RandomTags")
	TArray<FGyraItemInstanceTagDefiniton> RandomTags;




public:

	virtual void OnInstanceCreated(UGyraInventoryItemInstance* Instance) const override;

	UFUNCTION(BlueprintPure, Category = "VALUE")
	virtual bool GetItemStatByTag(FGameplayTag Tag, FGyraItemInstanceTagDefiniton& OutItemInstanceTagDefiniton) const;


protected:

	virtual void InteralRandom(UGyraInventoryItemInstance* Instance, 
	TArray<FGyraItemInstanceTagDefiniton>& LeftRandomTags,
	int32& LeftRandomNum) const;

	virtual void GetOnceRandomTag(const FGyraItemInstanceTagDefiniton& InTagDefintion, FGameplayTag&OutTag,int32& OutValue) const;

};
