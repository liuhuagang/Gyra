// Copyright 2024 Huagang Liu. All Rights Reserved.

#include "GyraGameSettings.h"
#include "DataRegistry.h"
#include "GyraWorldCollectable.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(GyraGameSettings)

UGyraGameSettings::UGyraGameSettings()
{
	CategoryName = TEXT("Game");
}


UGyraGameSettings* UGyraGameSettings::GetGyraGameSettings() 
{
	
	UGyraGameSettings* Settings = GetMutableDefault<UGyraGameSettings>();

	return Settings;

	
}

UDataRegistry* UGyraGameSettings::GetPawnListDataRegistry()
{
	if (!PawnListDataRegistry)
	{
		if (PawnListName.IsValid())
		{
			PawnListDataRegistry = Cast<UDataRegistry>(PawnListName.TryLoad());
			PawnListDataRegistry->AddToRoot();
		}
	}
	return PawnListDataRegistry;
}

UDataRegistry* UGyraGameSettings::GetCharacterAttributeSetDataRegistry()
{

	if (!CharacterAttributeSetDataRegistry)
	{
		if (CharacterAttributeSetName.IsValid())
		{
			CharacterAttributeSetDataRegistry = Cast<UDataRegistry>(CharacterAttributeSetName.TryLoad());
			CharacterAttributeSetDataRegistry->AddToRoot();
		}
	}
	return CharacterAttributeSetDataRegistry;
}

UDataRegistry* UGyraGameSettings::GetInventoryListDataRegistry()
{
	if (!InventoryListDataRegistry)
	{
		if (InventoryListName.IsValid())
		{
			InventoryListDataRegistry = Cast<UDataRegistry>(InventoryListName.TryLoad());
			InventoryListDataRegistry->AddToRoot();
		}
	}
	return InventoryListDataRegistry;
}

UDataRegistry* UGyraGameSettings::GetDropListDataRegistry()
{
	if (!DropListDataRegistry)
	{
		if (DropListName.IsValid())
		{
			DropListDataRegistry = Cast<UDataRegistry>(DropListName.TryLoad());
			DropListDataRegistry->AddToRoot();
		}
	}
	return DropListDataRegistry;
}

TSubclassOf<AGyraWorldCollectable> UGyraGameSettings::GetWorldCollectableBase()
{
	if (!WorldCollectableBaseClass)
	{
		WorldCollectableBaseClass =WorldCollectableBaseClassName.TryLoadClass<AGyraWorldCollectable>();
		WorldCollectableBaseClass->AddToRoot();
	}
	return WorldCollectableBaseClass;
}

FLinearColor UGyraGameSettings::GetItemQualityColor(EGyraInventoryItemQuality InItemQuality)
{

	if (FLinearColor* ThisColor = QualityColorList.Find(InItemQuality))
	{
		return *ThisColor;
	}
	
	return FLinearColor::White;
}

TArray<FGyraItemInstanceTagDefiniton> UGyraGameSettings::GetRandomTagDefaultList()
{
	return RandomTagDefaultList;
}

