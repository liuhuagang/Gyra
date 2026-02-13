// Copyright 2025 Huagang Liu. All Rights Reserved.

#include "GyraMutableInfo.h"
#include "MuCO/CustomizableObjectInstance.h"

FGyraMutableParameterInfo::FGyraMutableParameterInfo()
{
}

FGyraMutableParameterInfo::FGyraMutableParameterInfo(FString InParameterName, FString InParameterIntValue)
:ParameterName(InParameterName),
ParameterIntValue(InParameterIntValue)
{
}

FGyraMutableParameterInfo::FGyraMutableParameterInfo(FString InParameterName, bool InParameterBoolValue)
:ParameterName(ParameterName),
ParameterBoolValue(ParameterBoolValue)
{
	bIsBool =true;
}

FGyraMutableParameterInfo::~FGyraMutableParameterInfo()
{
}


FGyraMutableInfo::FGyraMutableInfo()
{

	Add(TEXT("HeadColor"), TEXT("04"));
	Add(TEXT("LashesColor"), TEXT("01"));
	Add(TEXT("LeftEyeColor"), TEXT("01"));
	Add(TEXT("SkinColor"), TEXT("01"));
	Add(TEXT("HeadColor"), TEXT("04"));
	Add(TEXT("EarringsGroup"), TEXT("None"));

	//Add(TEXT("LeavesObject"), false);


	Add(TEXT("HairBoundColor"), TEXT("01"));
	Add(TEXT("HairBoundsStyle"), TEXT("00"));
	Add(TEXT("HairBowColor"), TEXT("01"));
	Add(TEXT("HairBowsStyle"), TEXT("00"));
	Add(TEXT("HairMainColor"), TEXT("01"));
	Add(TEXT("HairMainStyle"), TEXT("00"));

	Add(TEXT("DressGroup"), TEXT("None"));
	Add(TEXT("GloveGroup"), TEXT("None"));
	Add(TEXT("PantGroup"), TEXT("None"));
	Add(TEXT("ShoeGroup"), TEXT("None"));
	Add(TEXT("ShoulderGroup"), TEXT("None"));

}


FGyraMutableInfo::~FGyraMutableInfo()
{


}

void FGyraMutableInfo::Add(FString InParameterName, FString InParameterIntValue)
{
	ParameterList.Add(FGyraMutableParameterInfo(InParameterName, InParameterIntValue));

}

//void FGyraMutableInfo::Add(FString InParameterName, bool InParameterBoolValue)
//{
//	ParameterList.Add(FGyraMutableParameterInfo(InParameterName, InParameterBoolValue));
//}

void FGyraMutableInfo::SerializeMutableInfo(UCustomizableObjectInstance* InObject)
{
	for (auto& Tmp : ParameterList)
	{
		if (Tmp.bIsBool)
		{
			Tmp.ParameterBoolValue = InObject->GetBoolParameterSelectedOption(Tmp.ParameterName);
		}
		else
		{
			Tmp.ParameterIntValue = InObject->GetIntParameterSelectedOption(Tmp.ParameterName);
		}

	}

}

void FGyraMutableInfo::DeserializeMutableInfo(UCustomizableObjectInstance* InObject)
{
	for (auto& Tmp : ParameterList)
	{
		if (Tmp.bIsBool)
		{
			InObject->SetBoolParameterSelectedOption(Tmp.ParameterName, Tmp.ParameterBoolValue);
		}
		else
		{
			InObject->SetIntParameterSelectedOption(Tmp.ParameterName, Tmp.ParameterIntValue);
		}

	}


}

