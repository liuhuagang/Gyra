// Copyright 2024 Huagang Liu. All Rights Reserved.

#include "GyraLoadingScreenSubsystem.h"

#include "Blueprint/UserWidget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GyraLoadingScreenSubsystem)

class UUserWidget;

//////////////////////////////////////////////////////////////////////
// UGyraLoadingScreenSubsystem

UGyraLoadingScreenSubsystem::UGyraLoadingScreenSubsystem()
{
}

void UGyraLoadingScreenSubsystem::SetLoadingScreenContentWidget(TSubclassOf<UUserWidget> NewWidgetClass)
{
	if (LoadingScreenWidgetClass != NewWidgetClass)
	{
		LoadingScreenWidgetClass = NewWidgetClass;

		OnLoadingScreenWidgetChanged.Broadcast(LoadingScreenWidgetClass);
	}
}

TSubclassOf<UUserWidget> UGyraLoadingScreenSubsystem::GetLoadingScreenContentWidget() const
{
	return LoadingScreenWidgetClass;
}

