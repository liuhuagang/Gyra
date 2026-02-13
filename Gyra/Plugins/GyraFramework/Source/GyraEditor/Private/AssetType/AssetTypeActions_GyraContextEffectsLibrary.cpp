// Copyright 2024 Huagang Liu. All Rights Reserved.

#include "AssetTypeActions_GyraContextEffectsLibrary.h"

#include "GyraContextEffectsLibrary.h"

class UClass;

#define LOCTEXT_NAMESPACE "AssetTypeActions"

UClass* FAssetTypeActions_GyraContextEffectsLibrary::GetSupportedClass() const
{
	return UGyraContextEffectsLibrary::StaticClass();
}

#undef LOCTEXT_NAMESPACE
