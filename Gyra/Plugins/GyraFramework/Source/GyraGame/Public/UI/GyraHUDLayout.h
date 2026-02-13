// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "GyraActivatableWidget.h"
#include "Containers/Ticker.h"
#include "GameplayTagContainer.h"

#include "GyraHUDLayout.generated.h"

class UCommonActivatableWidget;
class UObject;

class UGyraControllerDisconnectedScreen;

/**
 * UGyraHUDLayout
 *
 *	Widget used to lay out the player's HUD (typically specified by an Add Widgets action in the experience)
 *	
 *	用于布局玩家HUD的widget（通常由体验中的Add Widgets操作指定）
 *	
 * 
 */
UCLASS(Abstract, BlueprintType, Blueprintable, Meta = (DisplayName = "Gyra HUD Layout", Category = "Gyra|HUD"))
class UGyraHUDLayout : public UGyraActivatableWidget
{
	GENERATED_BODY()

public:

	UGyraHUDLayout(const FObjectInitializer& ObjectInitializer);

	void NativeOnInitialized() override;

	virtual void NativeDestruct() override;

protected:

	void HandleEscapeAction();


	/**
	* Callback for when controllers are disconnected. This will check if the player now has
	* no mapped input devices to them, which would mean that they can't play the game.
	*
	* If this is the case, then call DisplayControllerDisconnectedMenu.
	* 
	* 控制器断开连接时的回调。这将检查玩家现在是否没有映射的输入设备，这意味着他们不能玩游戏。
	* 如果是这种情况，那么调用DisplayControllerDisconnectedMenu。
	* 
	*/
	void HandleInputDeviceConnectionChanged(EInputDeviceConnectionState NewConnectionState, FPlatformUserId PlatformUserId, FInputDeviceId InputDeviceId);

	/**
	* Callback for when controllers change their owning platform user. We will use this to check
	* if we no longer need to display the "Controller Disconnected" menu
	* 
	* 当控制器改变自己的平台用户时回调。我们将使用它来检查是否不再需要显示“Controller Disconnected”菜单
	* 
	*/
	void HandleInputDevicePairingChanged(FInputDeviceId InputDeviceId, FPlatformUserId NewUserPlatformId, FPlatformUserId OldUserPlatformId);

	/**
	* Notify this widget that the state of controllers for the player have changed. Queue a timer for next tick to
	* process them and see if we need to show/hide the "controller disconnected" widget.
	* 
	* 通知这个小部件，播放器的控制器状态已经改变。为下一个tick设置一个计时器来处理它们，看看我们是否需要显示/隐藏“控制器断开连接”小部件。
	* 
	*/
	void NotifyControllerStateChangeForDisconnectScreen();


	/**
	 * This will check the state of the connected controllers to the player. If they do not have
	 * any controllers connected to them, then we should display the Disconnect menu. If they do have
	 * controllers connected to them, then we can hide the disconnect menu if its showing.
	 * 
	 * 这将检查连接到播放器的控制器的状态。如果它们没有任何控制器连接到它们，那么我们应该显示Disconnect菜单。如果它们确实有控制器连接到它们，那么我们可以隐藏断开连接菜单，如果它显示。
	 * 
	 */
	virtual void ProcessControllerDevicesHavingChangedForDisconnectScreen();

	/**
	 * Returns true if this platform supports a "controller disconnected" screen.
	 * 
	 * 如果此平台支持“控制器断开连接”屏幕，则返回true。
	 * 
	 */
	virtual bool ShouldPlatformDisplayControllerDisconnectScreen() const;

	/**
	* Pushes the ControllerDisconnectedMenuClass to the Menu layer (UI.Layer.Menu)
	* 
	* 将controllerdisconnectedmenucclass推送到菜单层（UI.Layer.Menu）
	* 
	*/
	UFUNCTION(BlueprintNativeEvent, Category = "Controller Disconnect Menu")
	void DisplayControllerDisconnectedMenu();


	/**
	* Hides the controller disconnected menu if it is active.
	* 
	* 如果控制器已断开连接菜单处于活动状态，则隐藏该菜单。
	* 
	*/
	UFUNCTION(BlueprintNativeEvent, Category = "Controller Disconnect Menu")
	void HideControllerDisconnectedMenu();

	/**
	 * The menu to be displayed when the user presses the "Pause" or "Escape" button
	 * 当用户按下“暂停”或“退出”按钮时显示的菜单
	 * 
	 */
	UPROPERTY(EditDefaultsOnly)
	TSoftClassPtr<UCommonActivatableWidget> EscapeMenuClass;

	/**
	* The widget which should be presented to the user if all of their controllers are disconnected.
	* 当用户的所有控制器都断开连接时，应该显示给用户的小部件。
	* 
	*/
	UPROPERTY(EditDefaultsOnly, Category = "Controller Disconnect Menu")
	TSubclassOf<UGyraControllerDisconnectedScreen> ControllerDisconnectedScreen;

	/**
	 * The platform tags that are required in order to show the "Controller Disconnected" screen.
	 * 显示“Controller Disconnected”屏幕所需的平台标签。
	 * 
	 * 
	 *
	 * 如果这些标签没有在这个平台的INI文件中设置，那么控制器断开屏幕将永远不会显示。
	 * 
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Controller Disconnect Menu")
	FGameplayTagContainer PlatformRequiresControllerDisconnectScreen;


	/**
	 * Pointer to the active "Controller Disconnected" menu if there is one.
	 * 
	 * 指针指向活动的“控制器断开连接”菜单（如果有）。
	 * 
	 */
	UPROPERTY(Transient)
	TObjectPtr<UCommonActivatableWidget> SpawnedControllerDisconnectScreen;


	/**
	 *
	 * Handle from the FSTicker for when we want to process the controller state of our player
	 * 当我们想要处理播放器的控制器状态时，来自FSTicker的句柄
	 * 
	 */
	FTSTicker::FDelegateHandle RequestProcessControllerStateHandle;


};
