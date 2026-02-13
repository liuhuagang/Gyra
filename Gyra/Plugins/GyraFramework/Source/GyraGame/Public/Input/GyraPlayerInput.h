// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "EnhancedPlayerInput.h"

#include "GyraPlayerInput.generated.h"

/**
 * Custom player input class for Lyra. This extends the functionality of Enhanced Input to also include
 * some input latency tracking on key press.
 *
 * Extend this class if you have any special logic which you may want to run relating to when keys are pressed
 * or when input is flushed.
 */

/**
 * 为莱拉定制的玩家输入类。此类扩展了增强输入的功能，还增加了按键按下时的输入延迟跟踪功能。*
 * 如果您有任何与按键按下或输入刷新相关的特殊逻辑需要执行，请在此类中添加相关内容。
 * 或者，如果您有其他特殊需求，请在此类中进行相关设置。
 *   
 */
UCLASS(config = Input, transient)
class UGyraPlayerInput : public UEnhancedPlayerInput
{
	GENERATED_BODY()

public:
	UGyraPlayerInput();
	virtual ~UGyraPlayerInput() override;

protected:
	//~ Begin UEnhancedPlayerInput Interface
	virtual bool InputKey(const FInputKeyEventArgs& Params) override;
	//~ End of UEnhancedPlayerInput interface
};
