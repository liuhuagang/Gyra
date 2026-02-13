// Copyright 2024 Huagang Liu. All Rights Reserved.

#include "GyraContextEffectsLibraryFactory.h"

#include "GyraContextEffectsLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GyraContextEffectsLibraryFactory)

class FFeedbackContext;
class UClass;
class UObject;

UGyraContextEffectsLibraryFactory::UGyraContextEffectsLibraryFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SupportedClass = UGyraContextEffectsLibrary::StaticClass();

	bCreateNew = true;
	bEditorImport = false;
	bEditAfterNew = true;
}

UObject* UGyraContextEffectsLibraryFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	UGyraContextEffectsLibrary* GyraContextEffectsLibrary = NewObject<UGyraContextEffectsLibrary>(InParent, Name, Flags);

	return GyraContextEffectsLibrary;
}
