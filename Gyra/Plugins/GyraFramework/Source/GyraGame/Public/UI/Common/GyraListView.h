// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "CommonListView.h"

#include "GyraListView.generated.h"

class UUserWidget;
class ULocalPlayer;
class UGyraWidgetFactory;

UCLASS(meta = (DisableNativeTick))
class GYRAGAME_API UGyraListView : public UCommonListView
{
	GENERATED_BODY()

public:
	UGyraListView(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());




#if WITH_EDITOR
	virtual void ValidateCompiledDefaults(IWidgetCompilerLog& InCompileLog) const override;
#endif

protected:
	virtual UUserWidget& OnGenerateEntryWidgetInternal(UObject* Item, TSubclassOf<UUserWidget> DesiredEntryClass, const TSharedRef<STableViewBase>& OwnerTable) override;
	//virtual bool OnIsSelectableOrNavigableInternal(UObject* SelectedItem) override;

protected:
	UPROPERTY(EditAnywhere, Instanced, Category="Entry Creation")
	TArray<TObjectPtr<UGyraWidgetFactory>> FactoryRules;
};
