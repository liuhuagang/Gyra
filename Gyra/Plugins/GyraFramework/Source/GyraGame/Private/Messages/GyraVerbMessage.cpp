// Copyright 2024 Huagang Liu. All Rights Reserved.

#include "GyraVerbMessage.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GyraVerbMessage)

FString FGyraVerbMessage::ToString() const
{
	FString HumanReadableMessage;
	FGyraVerbMessage::StaticStruct()->ExportText(/*out*/ HumanReadableMessage,
	this, /*Defaults=*/ nullptr, /*OwnerObject=*/ nullptr, PPF_None, /*ExportRootScope=*/ nullptr);
	return HumanReadableMessage;
}
