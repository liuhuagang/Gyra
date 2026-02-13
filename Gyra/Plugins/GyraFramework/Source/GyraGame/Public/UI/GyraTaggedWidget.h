// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "CommonUserWidget.h"
#include "GameplayTagContainer.h"

#include "GyraTaggedWidget.generated.h"

class UObject;

/**
 * An widget in a layout that has been tagged (can be hidden or shown via tags on the owning player)
 * 
 * 布局中的一个组件，该组件已被标记（可通过所属玩家面板上的标记来隐藏或显示）
 */
UCLASS(Abstract, Blueprintable)
class UGyraTaggedWidget : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	UGyraTaggedWidget(const FObjectInitializer& ObjectInitializer);

	//~UWidget interface
	virtual void SetVisibility(ESlateVisibility InVisibility) override;
	//~End of UWidget interface

	//~UUserWidget interface
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	//~End of UUserWidget interface

protected:
	
	/** If the owning player has any of these tags, this widget will be hidden (using HiddenVisibility) */
	/** 如果拥有该玩家控件的玩家身上带有以下任何标签，此控件将被隐藏（通过“隐藏可见性”实现） */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HUD")
	FGameplayTagContainer HiddenByTags;

	/** The visibility to use when this widget is shown (not hidden by gameplay tags). */
	/** 当此控件显示时应采用的可见性设置（即不受游戏玩法标签影响的可见性）。*/
	UPROPERTY(EditAnywhere, Category = "HUD")
	ESlateVisibility ShownVisibility = ESlateVisibility::Visible;

	/** The visibility to use when this widget is hidden by gameplay tags. */
	/** 当此控件因游戏玩法标签而被隐藏时所应采用的可见性设置。*/
	UPROPERTY(EditAnywhere, Category = "HUD")
	ESlateVisibility HiddenVisibility = ESlateVisibility::Collapsed;

	/** Do we want to be visible (ignoring tags)? */
	/** 我们是否希望被展示出来（忽略标签）？*/
	bool bWantsToBeVisible = true;

private:
	void OnWatchedTagsChanged();
};
