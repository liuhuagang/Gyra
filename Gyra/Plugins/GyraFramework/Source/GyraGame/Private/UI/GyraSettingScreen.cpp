// Copyright 2024 Huagang Liu. All Rights Reserved.

#include "GyraSettingScreen.h"

#include "Input/CommonUIInputTypes.h"

#include "GyraLocalPlayer.h"
#include "GyraGameSettingRegistry.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GyraSettingScreen)

class UGameSettingRegistry;

void UGyraSettingScreen::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	BackHandle = RegisterUIActionBinding(FBindUIActionArgs(BackInputActionData, true, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleBackAction)));
	ApplyHandle = RegisterUIActionBinding(FBindUIActionArgs(ApplyInputActionData, true, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleApplyAction)));
	CancelChangesHandle = RegisterUIActionBinding(FBindUIActionArgs(CancelChangesInputActionData, true, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleCancelChangesAction)));
}

UGameSettingRegistry* UGyraSettingScreen::CreateRegistry()
{
	UGyraGameSettingRegistry* NewRegistry = NewObject<UGyraGameSettingRegistry>();

	if (UGyraLocalPlayer* LocalPlayer = CastChecked<UGyraLocalPlayer>(GetOwningLocalPlayer()))
	{
		NewRegistry->Initialize(LocalPlayer);
	}

	return NewRegistry;
}

bool UGyraSettingScreen::NativeOnHandleBackAction()
{
	// 这里跳过了蓝图实现,直接在C++层面搞定就行!
	
	if (AttemptToPopNavigation())
	{
		return true;
	}

	ApplyChanges();

	DeactivateWidget();
	
	return true;
}

void UGyraSettingScreen::HandleApplyAction()
{
	ApplyChanges();
}

void UGyraSettingScreen::HandleCancelChangesAction()
{
	CancelChanges();
}

void UGyraSettingScreen::OnSettingsDirtyStateChanged_Implementation(bool bSettingsDirty)
{
	if (bSettingsDirty)
	{
		if (!GetActionBindings().Contains(ApplyHandle))
		{
			AddActionBinding(ApplyHandle);
		}
		if (!GetActionBindings().Contains(CancelChangesHandle))
		{
			AddActionBinding(CancelChangesHandle);
		}
	}
	else
	{
		RemoveActionBinding(ApplyHandle);
		RemoveActionBinding(CancelChangesHandle);
	}
}
