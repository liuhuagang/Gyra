// Copyright 2024 Huagang Liu. All Rights Reserved.

#include "GyraPerfStatContainerBase.h"

#include "Blueprint/WidgetTree.h"
#include "GyraPerfStatWidgetBase.h"
#include "GyraSettingsLocal.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GyraPerfStatContainerBase)

//////////////////////////////////////////////////////////////////////
// UGyraPerfStatsContainerBase

UGyraPerfStatContainerBase::UGyraPerfStatContainerBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UGyraPerfStatContainerBase::NativeConstruct()
{
	Super::NativeConstruct();
	UpdateVisibilityOfChildren();

	UGyraSettingsLocal::Get()->OnPerfStatDisplayStateChanged().AddUObject(this, &ThisClass::UpdateVisibilityOfChildren);
}

void UGyraPerfStatContainerBase::NativeDestruct()
{
	UGyraSettingsLocal::Get()->OnPerfStatDisplayStateChanged().RemoveAll(this);

	Super::NativeDestruct();
}

void UGyraPerfStatContainerBase::UpdateVisibilityOfChildren()
{
	UGyraSettingsLocal* UserSettings = UGyraSettingsLocal::Get();

	const bool bShowTextWidgets = (StatDisplayModeFilter == EGyraStatDisplayMode::TextOnly) || (StatDisplayModeFilter == EGyraStatDisplayMode::TextAndGraph);
	const bool bShowGraphWidgets = (StatDisplayModeFilter == EGyraStatDisplayMode::GraphOnly) || (StatDisplayModeFilter == EGyraStatDisplayMode::TextAndGraph);
	
	check(WidgetTree);
	WidgetTree->ForEachWidget([&](UWidget* Widget)
	{
		if (UGyraPerfStatWidgetBase* TypedWidget = Cast<UGyraPerfStatWidgetBase>(Widget))
		{
			const EGyraStatDisplayMode SettingMode = UserSettings->GetPerfStatDisplayState(TypedWidget->GetStatToDisplay());

			bool bShowWidget = false;
			switch (SettingMode)
			{
			case EGyraStatDisplayMode::Hidden:
				bShowWidget = false;
				break;
			case EGyraStatDisplayMode::TextOnly:
				bShowWidget = bShowTextWidgets;
				break;
			case EGyraStatDisplayMode::GraphOnly:
				bShowWidget = bShowGraphWidgets;
				break;
			case EGyraStatDisplayMode::TextAndGraph:
				bShowWidget = bShowTextWidgets || bShowGraphWidgets;
				break;
			}

			TypedWidget->SetVisibility(bShowWidget ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
		}
	});
}

