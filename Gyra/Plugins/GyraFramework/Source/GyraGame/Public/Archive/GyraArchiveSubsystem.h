// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once


#include "CoreMinimal.h"

#include "GyraArchivePanelInformation.h"
#include "GyraArchiveSlotInformation.h"
#include "GyraMutableInfo.h"

#include "GyraArchiveSubsystem.generated.h"


class AGyraPlayerController;
struct FGyraMutableInfo;

/**
 * 存档设计:
 * ArchivePanelInformation为管理所有存档的面板信息,只有一个
 * ArchiveSlot_为存档信息.
 * 0,是自动存档,避免意外,自动存档会随时切换
 * 1,2,3,4是手动存档
 *
 * 运行时 会分别持有ArchivePanelInformation和ArchiveSlot_各一个
 * 每个存档的 主动武器,被动装备,背包时独立的
 * 仓库是公用的 
 * 
 * 装扮信息尚未重构处理
 * 
 *
 */


/**
 * 这是个用于存档管理的子系统
 */
UCLASS()
class GYRAGAME_API UGyraArchiveSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Deinitialize() override;

	static UGyraArchiveSubsystem* GetArchiveSubsystem();


public:

	/** 获取对应插槽索引的插槽命名 */
	UFUNCTION(BlueprintPure, Category = "ArchiveSubsystem")
	FString GetArchiveSlotName(int32 InSlotIndex);

	/** 加载最近的存档 如果没有则创建一个新的 */
	UFUNCTION(BlueprintCallable, Category = "ArchiveSubsystem")
	void LoadRecentArchiveSlot();

	/** 加载最近的存档 如果没有则创建一个新的 */
	UFUNCTION(BlueprintCallable, Category = "ArchiveSubsystem")
	void AutoSaveArchiveSlot();


	/** 存储信息到指定Slot */
	UFUNCTION(BlueprintCallable, Category = "ArchiveSubsystem")
	bool SaveArchiveSlot(int32 InSlotIndex , UGyraArchiveSlotInformation* InSlotInformation);

	/** 删除指定Slot */
	UFUNCTION(BlueprintCallable, Category = "ArchiveSubsystem")
	bool DeleteArchiveSlot(int32 InSlotIndex);

	/** 加载指定Slot的信息 如果存档不存在则加载失败*/
	UFUNCTION(BlueprintCallable, Category = "ArchiveSubsystem")
	bool LoadArchiveSlot(int32 InSlotIndex);

	UFUNCTION(BlueprintCallable, Category = "ArchiveSubsystem")
	FORCEINLINE	UGyraArchivePanelInformation* GetGyraArchivePanelInformation() {

		return PanelInformation;

	}

	UFUNCTION(BlueprintCallable)
	void SetMutableInfo(FGyraMutableInfo InMutableInfo)
	{
		if (!ActiveSlotInformation)
		{
			LoadRecentArchiveSlot();
		}
		ActiveSlotInformation->MutableInfo = InMutableInfo;
	}

	UFUNCTION(BlueprintCallable)
	FGyraMutableInfo GetMutableInfo()
	{
		if (!ActiveSlotInformation)
		{
			LoadRecentArchiveSlot();
		}

		return ActiveSlotInformation->MutableInfo;
	}

public:

	/** 加载本地存档面板信息到系统中来，如果没有就创建 */
	void LoadArchivePanelInformationFromLocal();

	/** 存储系统的面板信息到本地，如果是空的就创建一个空白的面板信息存到本地 */
	void SaveArchivePanelInformationToLocal();

public:

	static UGyraArchiveSubsystem* ArchiveSubsystemPtr;

	FString ArchivePanelName = TEXT("ArchivePanelInformation");

	FString ArchiveSlotName = TEXT("ArchiveSlot_");

	/** 运行时持有的面板信息 */
	UPROPERTY()
	TObjectPtr<UGyraArchivePanelInformation> PanelInformation = nullptr;

	/** 当前激活的存档 */
	UPROPERTY()
	TObjectPtr<UGyraArchiveSlotInformation> ActiveSlotInformation = nullptr;

	/** 当前激活的存档索引 */
	int32 ActiveSlotIndex =-1;

	
};


