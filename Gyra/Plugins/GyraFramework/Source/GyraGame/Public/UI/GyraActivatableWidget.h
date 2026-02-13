// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "CommonActivatableWidget.h"

#include "GyraActivatableWidget.generated.h"

struct FUIInputConfig;

UENUM(BlueprintType)
enum class EGyraWidgetInputMode : uint8
{
	Default,
	GameAndMenu,
	Game,
	Menu
};

// An activatable widget that automatically drives the desired input config when activated
// 一个可激活的组件，激活时会自动执行所需的输入配置操作。
UCLASS(Abstract, Blueprintable)
class UGyraActivatableWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	UGyraActivatableWidget(const FObjectInitializer& ObjectInitializer);
	
public:
	
	//~UCommonActivatableWidget interface
	virtual TOptional<FUIInputConfig> GetDesiredInputConfig() const override;
	//~End of UCommonActivatableWidget interface

#if WITH_EDITOR
	virtual void ValidateCompiledWidgetTree(const UWidgetTree& BlueprintWidgetTree, class IWidgetCompilerLog& CompileLog) const override;
#endif
	
protected:
	/** The desired input mode to use while this UI is activated, for example do you want key presses to still reach the game/player controller? */
	/** 当此用户界面被激活时所期望使用的输入模式，例如您是否希望键盘按键仍能传递给游戏/玩家控制器？*/
	UPROPERTY(EditDefaultsOnly, Category = Input)
	EGyraWidgetInputMode InputConfig = EGyraWidgetInputMode::Default;

	/** The desired mouse behavior when the game gets input. */
	/** 游戏接收到输入时所期望的鼠标行为。*/
	UPROPERTY(EditDefaultsOnly, Category = Input)
	EMouseCaptureMode GameMouseCaptureMode = EMouseCaptureMode::CapturePermanently;
};
