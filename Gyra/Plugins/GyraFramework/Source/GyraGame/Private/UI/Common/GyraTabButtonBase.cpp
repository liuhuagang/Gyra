// Copyright 2024 Huagang Liu. All Rights Reserved.

#include "GyraTabButtonBase.h"

#include "CommonLazyImage.h"
#include "GyraTabListWidgetBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GyraTabButtonBase)

class UObject;
struct FSlateBrush;

void UGyraTabButtonBase::SetIconFromLazyObject(TSoftObjectPtr<UObject> LazyObject)
{
	if (LazyImage_Icon)
	{
		LazyImage_Icon->SetBrushFromLazyDisplayAsset(LazyObject);
	}
}

void UGyraTabButtonBase::SetIconBrush(const FSlateBrush& Brush)
{
	if (LazyImage_Icon)
	{
		LazyImage_Icon->SetBrush(Brush);
	}
}

void UGyraTabButtonBase::SetTabLabelInfo_Implementation(const FGyraTabDescriptor& TabLabelInfo)
{
	SetButtonText(TabLabelInfo.TabText);
	SetIconBrush(TabLabelInfo.IconBrush);
}

