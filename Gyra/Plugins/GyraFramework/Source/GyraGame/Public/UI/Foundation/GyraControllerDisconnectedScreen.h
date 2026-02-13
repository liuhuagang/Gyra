// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "CommonActivatableWidget.h"
#include "GameplayTagContainer.h"

#include "GyraControllerDisconnectedScreen.generated.h"

class UHorizontalBox;
class UObject;
class UCommonButtonBase;
struct FPlatformUserSelectionCompleteParams;

/**
 * A screen to display when the user has had all of their controllers disconnected and needs to
 * re-connect them to continue playing the game.
 * 
 * 当用户断开所有控制器并需要重新连接以继续玩游戏时显示的屏幕。
 * 
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class UGyraControllerDisconnectedScreen : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:

	UGyraControllerDisconnectedScreen(const FObjectInitializer& ObjectInitializer);

protected:

	virtual void NativeOnActivated() override;

	virtual void HandleChangeUserClicked();

	/**
	 * Called when the user has changed after selecting the prompt to change platform users.
	 * 当用户更改后，选择提示更改平台用户时调用。
	 * 
	 */
	virtual void HandleChangeUserCompleted(const FPlatformUserSelectionCompleteParams& Params);

	/**
	 * Returns true if the Change User button should be displayed.
	 * This will check the ICommonUIModule's platform trait tags at runtime.
	 * 
	 * 如果应该显示Change User按钮，返回true。这将在运行时检查ICommonUIModule的平台trait标签。
	 * 
	 */
	virtual bool ShouldDisplayChangeUserButton() const;

	/**
	 * Required platform traits that, when met, will display the "Change User" button
	 * allowing the player to change what signed in user is currently mapped to an input
	 * device.
	 * 
	 * 当满足所需的平台特征时，将显示“更改用户”按钮，允许玩家更改当前映射到输入设备的登录用户。
	 * 
	 */
	UPROPERTY(EditDefaultsOnly)
	FGameplayTagContainer PlatformSupportsUserChangeTags;

	/**
	 * Platforms that have "strict" user pairing requirements may want to allow you to change your user right from
	 * the in-game UI here. These platforms are tagged with "Platform.Trait.Input.HasStrictControllerPairing" in
	 * Common UI.
	 *
	 * This HBox will be set to invisible if the platform you are on does NOT have that platform trait.
	 * 
	 * 拥有“严格”用户配对要求的平台可能希望允许你在游戏UI中更改用户。这些平台被标记为“Platform.Trait.Input.HasStrictControllerPairing”。HasStrictControllerPairing" in Common UI。
	 * 如果你所在的平台没有这个特性，这个HBox将被设置为不可见。
	 * 
	 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHorizontalBox> HBox_SwitchUser;


	/**
	* 
	* A button to handle changing the user on platforms with strict user pairing requirements.
	* 在具有严格用户配对要求的平台上处理更改用户的按钮。
	* 
	* @see HBox_SwitchUser
	*/
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonButtonBase> Button_ChangeUser;
};
