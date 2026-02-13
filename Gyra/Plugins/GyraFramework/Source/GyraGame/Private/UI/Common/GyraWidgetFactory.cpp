// Copyright 2024 Huagang Liu. All Rights Reserved.

#include "GyraWidgetFactory.h"
#include "Templates/SubclassOf.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GyraWidgetFactory)

class UUserWidget;

TSubclassOf<UUserWidget> UGyraWidgetFactory::FindWidgetClassForData_Implementation(const UObject* Data) const
{
	return TSubclassOf<UUserWidget>();
}
