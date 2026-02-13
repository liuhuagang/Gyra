// Copyright 2024 Huagang Liu. All Rights Reserved.

#include "GyraGameSession.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GyraGameSession)


AGyraGameSession::AGyraGameSession(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

bool AGyraGameSession::ProcessAutoLogin()
{
	// This is actually handled in GyraGameMode::TryDedicatedServerLogin
	// 通过GameMode处理DS服务器登录的情况!
	return true;
}

void AGyraGameSession::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();
}

void AGyraGameSession::HandleMatchHasEnded()
{
	Super::HandleMatchHasEnded();
}

