// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "CommonGameInstance.h"

#include "GyraGameInstance.generated.h"

enum class ECommonUserOnlineContext : uint8;
class AGyraPlayerController;
class UObject;
/*
 * 插件所使用的GyraGameInstance
 * 主要是嵌入一下,读取本地用户设置LocalSettings的时机.
 * 
 */
UCLASS(Config = Game)
class GYRAGAME_API UGyraGameInstance : public UCommonGameInstance
{
	GENERATED_BODY()

public:
	
	UGyraGameInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	AGyraPlayerController* GetPrimaryPlayerController() const;

	virtual bool CanJoinRequestedSession() const override;

	virtual void HandlerUserInitialized(const UCommonUserInfo* UserInfo,
	                                    bool bSuccess, FText Error, ECommonUserPrivilege RequestedPrivilege,
	                                    ECommonUserOnlineContext OnlineContext) override;

protected:
	
	virtual void Init() override;
	
	virtual void Shutdown() override;

	void OnPreClientTravelToSession(FString& URL);
};
