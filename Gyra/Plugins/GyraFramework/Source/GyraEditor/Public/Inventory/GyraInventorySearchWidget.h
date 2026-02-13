// Copyright 2025 Huagang Liu. All Rights Reserved.

#pragma once

#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine/DataTable.h"

#include "EditorUtilitySubsystem.h"
#include "EditorUtilityWidget.h"

#include "GyraInventorySearchWidget.generated.h"

class UDataTable;
UCLASS()
class GYRAEDITOR_API UGyraInventorySearchWidget : public UEditorUtilityWidget
{
    GENERATED_BODY()

public:

    UFUNCTION(BlueprintCallable, Category = "Blueprint Tools")
    void SearchAllInventoryItems();

protected:



public:

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Asset")
    TObjectPtr<UDataTable> DT_InventoryList;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Asset",meta = (ContentDir))
    FDirectoryPath   TargetDirectory;

};