// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "GyraWidgetFactory.h"
#include "Templates/SubclassOf.h"
#include "UObject/SoftObjectPtr.h"

#include "GyraWidgetFactory_Class.generated.h"

class UObject;
class UUserWidget;

UCLASS()
class GYRAGAME_API UGyraWidgetFactory_Class : public UGyraWidgetFactory
{
	GENERATED_BODY()

public:
	UGyraWidgetFactory_Class() { }

	virtual TSubclassOf<UUserWidget> FindWidgetClassForData_Implementation(const UObject* Data) const override;
	
protected:
	UPROPERTY(EditAnywhere, Category = ListEntries, meta = (AllowAbstract))
	TMap<TSoftClassPtr<UObject>, TSubclassOf<UUserWidget>> EntryWidgetForClass;
};
