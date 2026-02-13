// Copyright 2024 Huagang Liu. All Rights Reserved.

#include "GyraGameInstance.h"

#include "CommonSessionSubsystem.h"
#include "CommonUserSubsystem.h"
#include "Components/GameFrameworkComponentManager.h"
#include "HAL/IConsoleManager.h"
#include "GyraGameplayTags.h"
#include "GyraPlayerController.h"
#include "GyraLocalPlayer.h"
#include "GameFramework/PlayerState.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GyraGameInstance)


UGyraGameInstance::UGyraGameInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UGyraGameInstance::Init()
{
	Super::Init();

	// Register our custom init states
	// 注册我们自定义的初始化状态
	UGameFrameworkComponentManager* ComponentManager = GetSubsystem<UGameFrameworkComponentManager>(this);

	if (ensure(ComponentManager))
	{
		/** 会添加一个新的全局角色特性状态，该状态可置于现有状态之前或之后。通常此操作会在游戏全局初始化或游戏特性初始化时执行 */
		ComponentManager->RegisterInitState(GyraGameplayTags::InitState_Spawned, false, FGameplayTag());
		ComponentManager->RegisterInitState(GyraGameplayTags::InitState_DataPrepared, false, GyraGameplayTags::InitState_Spawned);
		ComponentManager->RegisterInitState(GyraGameplayTags::InitState_DataAvailable, false, GyraGameplayTags::InitState_DataPrepared);
		ComponentManager->RegisterInitState(GyraGameplayTags::InitState_DataInitialized, false, GyraGameplayTags::InitState_DataAvailable);
		ComponentManager->RegisterInitState(GyraGameplayTags::InitState_GameplayReady, false, GyraGameplayTags::InitState_DataInitialized);
	}

	if (UCommonSessionSubsystem* SessionSubsystem = GetSubsystem<UCommonSessionSubsystem>())
	{
		SessionSubsystem->OnPreClientTravelEvent.AddUObject(this, &UGyraGameInstance::OnPreClientTravelToSession);
	}
}

void UGyraGameInstance::Shutdown()
{
	if (UCommonSessionSubsystem* SessionSubsystem = GetSubsystem<UCommonSessionSubsystem>())
	{
		SessionSubsystem->OnPreClientTravelEvent.RemoveAll(this);
	}

	Super::Shutdown();
}

AGyraPlayerController* UGyraGameInstance::GetPrimaryPlayerController() const
{
	return Cast<AGyraPlayerController>(Super::GetPrimaryPlayerController(false));
}

bool UGyraGameInstance::CanJoinRequestedSession() const
{
	// Temporary first pass:  Always return true
	// This will be fleshed out to check the player's state
	
	// 临时初步处理：始终返回真值
	// 这部分将被进一步完善，以检查玩家的状态
	if (!Super::CanJoinRequestedSession())
	{
		return false;
	}
	return true;
}

void UGyraGameInstance::HandlerUserInitialized(const UCommonUserInfo* UserInfo, bool bSuccess, FText Error, ECommonUserPrivilege RequestedPrivilege, ECommonUserOnlineContext OnlineContext)
{
	Super::HandlerUserInitialized(UserInfo, bSuccess, Error, RequestedPrivilege, OnlineContext);

	// If login succeeded, tell the local player to load their settings
	// 如果登录成功，就告知本地玩家加载他们的设置
	if (bSuccess && ensure(UserInfo))
	{
		UGyraLocalPlayer* LocalPlayer = Cast<UGyraLocalPlayer>(GetLocalPlayerByIndex(UserInfo->LocalPlayerIndex));

		// There will not be a local player attached to the dedicated server user
		// 专用服务器用户不会与本地玩家相关联。
		if (LocalPlayer)
		{
			LocalPlayer->LoadSharedSettingsFromDisk();
		}
	}
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void UGyraGameInstance::OnPreClientTravelToSession(FString& URL)
{
	// 之前用于测试加密密钥的验证的代码,现已移除
	
}
