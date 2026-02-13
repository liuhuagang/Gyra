// Copyright 2025 Huagang Liu. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

class FGyraGameModule : public IModuleInterface
{
	
public:

	virtual void StartupModule() override;
	
	virtual void ShutdownModule() override;
	
};
