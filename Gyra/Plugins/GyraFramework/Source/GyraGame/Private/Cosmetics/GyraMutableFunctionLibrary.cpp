// Copyright 2025 Huagang Liu. All Rights Reserved.

#include "GyraMutableFunctionLibrary.h"
#include "GyraMutableInfo.h"

void UGyraMutableFunctionLibrary::SerializeMutableInfo(UCustomizableObjectInstance* InObject, FGyraMutableInfo& OutInfo)
{
	OutInfo.SerializeMutableInfo(InObject);
}

void UGyraMutableFunctionLibrary::DeserializeMutableInfo(UCustomizableObjectInstance* InObject, FGyraMutableInfo InInfo)
{
	InInfo.DeserializeMutableInfo(InObject);
}
