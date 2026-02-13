// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "UObject/Object.h"

#include "GyraWidgetFactory.generated.h"

template <class TClass> class TSubclassOf;

class UUserWidget;
struct FFrame;

UCLASS(Abstract, Blueprintable, BlueprintType, EditInlineNew)
class GYRAGAME_API UGyraWidgetFactory : public UObject
{
	GENERATED_BODY()

public:
	UGyraWidgetFactory() { }

	UFUNCTION(BlueprintNativeEvent)
	TSubclassOf<UUserWidget> FindWidgetClassForData(const UObject* Data) const;
};
