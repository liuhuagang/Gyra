// Copyright 2024 Huagang Liu. All Rights Reserved.

#include "GyraArchiveSubsystem.h"
#include "GyraPlayerController.h"
#include "Kismet/GameplayStatics.h"


UGyraArchiveSubsystem* UGyraArchiveSubsystem::ArchiveSubsystemPtr = nullptr;



void UGyraArchiveSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{

	Super::Initialize(Collection);

	ArchiveSubsystemPtr = this;


	LoadArchivePanelInformationFromLocal();
	LoadRecentArchiveSlot();

}

void UGyraArchiveSubsystem::Deinitialize()
{
	//AutoSaveArchiveSlot();
	//SaveArchivePanelInformationToLocal();
	ArchiveSubsystemPtr = nullptr;

	Super::Deinitialize();

}

UGyraArchiveSubsystem* UGyraArchiveSubsystem::GetArchiveSubsystem()
{
	return ArchiveSubsystemPtr;
}



FString UGyraArchiveSubsystem::GetArchiveSlotName(int32 InSlotIndex)
{
	return ArchiveSlotName + FString::FromInt(InSlotIndex);
}

void UGyraArchiveSubsystem::LoadRecentArchiveSlot()
{
	bool bLoaded =false;

	if (PanelInformation->LastActiveSlotIndex != -1)
	{
		bLoaded =LoadArchiveSlot(PanelInformation->LastActiveSlotIndex);
		ActiveSlotIndex = PanelInformation->LastActiveSlotIndex;
	}

	if (!bLoaded)
	{
		for (auto& Tmp : PanelInformation->ActiveArchiveSlots)
		{
			bLoaded = LoadArchiveSlot(Tmp.Key);

			if (bLoaded)
			{
				ActiveSlotIndex = Tmp.Key;
				break;
			}

		}
	}

	if (!bLoaded)
	{
		UGyraArchiveSlotInformation* Empty =Cast<UGyraArchiveSlotInformation>(UGameplayStatics::CreateSaveGameObject(UGyraArchiveSlotInformation::StaticClass()));

		bLoaded =SaveArchiveSlot(0, Empty);
		ActiveSlotIndex=0;
	}

}

void UGyraArchiveSubsystem::AutoSaveArchiveSlot()
{
	SaveArchiveSlot(ActiveSlotIndex, ActiveSlotInformation);
}

bool UGyraArchiveSubsystem::SaveArchiveSlot(int32 InSlotIndex, UGyraArchiveSlotInformation* InSlotInformation)
{
	bool bSaved =false;

	FString ThisSlotName = GetArchiveSlotName(InSlotIndex);
	
	bSaved = UGameplayStatics::SaveGameToSlot(InSlotInformation, ThisSlotName, 0);

	if (bSaved)
	{
		//更新面板信息并序列化到本地
		FGyraArchivePanelItemBriefInformation PanelItemBriefInformation;
		PanelItemBriefInformation.DateTime = FDateTime::Now().ToString();

		PanelInformation->ActiveArchiveSlots.Add(InSlotIndex, PanelItemBriefInformation);
		PanelInformation->LastActiveSlotIndex = InSlotIndex;

		
		SaveArchivePanelInformationToLocal();
	}


	return bSaved;

}

bool UGyraArchiveSubsystem::DeleteArchiveSlot(int32 InSlotIndex)
{
	FString ThisSlotName = GetArchiveSlotName(InSlotIndex);
	bool bRemoved= UGameplayStatics::DeleteGameInSlot(ThisSlotName, 0);

	if (bRemoved)
	{
		//更新存档管理的信息
		PanelInformation->ActiveArchiveSlots.Remove(InSlotIndex);
		SaveArchivePanelInformationToLocal();
	}


	return bRemoved;

}

bool UGyraArchiveSubsystem::LoadArchiveSlot(int32 InSlotIndex)
{
	bool bLoaded = false;

	FString ThisSlotName = GetArchiveSlotName(InSlotIndex);


	bool bExist = UGameplayStatics::DoesSaveGameExist(ThisSlotName, 0);

	if (bExist)
	{
		USaveGame* SaveGameObject = UGameplayStatics::LoadGameFromSlot(ThisSlotName, 0);

		if (UGyraArchiveSlotInformation* ThisSlotInformation =  Cast<UGyraArchiveSlotInformation>(SaveGameObject))
		{
			//加载成功
			bLoaded = true;
			ActiveSlotInformation= ThisSlotInformation;
			ActiveSlotIndex = InSlotIndex;

		}
	}

	return bLoaded;

}

void UGyraArchiveSubsystem::LoadArchivePanelInformationFromLocal()
{
	PanelInformation = Cast<UGyraArchivePanelInformation>(UGameplayStatics::LoadGameFromSlot(ArchivePanelName, 0));

	if (!PanelInformation)
	{
		PanelInformation = Cast<UGyraArchivePanelInformation>(UGameplayStatics::CreateSaveGameObject(UGyraArchivePanelInformation::StaticClass()));
	}

	UGameplayStatics::SaveGameToSlot(PanelInformation, ArchivePanelName, 0);
}

void UGyraArchiveSubsystem::SaveArchivePanelInformationToLocal()
{
	if (!PanelInformation)
	{
		PanelInformation = Cast<UGyraArchivePanelInformation>(UGameplayStatics::CreateSaveGameObject(UGyraArchivePanelInformation::StaticClass()));
	}

	UGameplayStatics::SaveGameToSlot(PanelInformation, ArchivePanelName, 0);
}
