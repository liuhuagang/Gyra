// Copyright 2025 Huagang Liu. All Rights Reserved.

#include "GyraInventorySearchWidget.h"
#include "LogGyraEditor.h"
#include "GyraInventoryItemDefinition.h"
#include "GyraInventoryIndexInfo.h"
#include "DataTableEditorUtils.h"

void UGyraInventorySearchWidget::SearchAllInventoryItems()
{
	UE_LOG(LogGyraEditor,Display,TEXT("Begin to Search Inventory Item"));

	if (!DT_InventoryList)
	{
		return;
	}

	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");




	// 2. 确保路径格式正确（以"/"结尾）
	FString SearchPath = TargetDirectory.Path;

	// 3. 递归扫描路径（包含子目录）
	AssetRegistryModule.Get().ScanPathsSynchronous({ SearchPath }, true);

	// 4. 创建过滤器
	FARFilter Filter;
	Filter.PackagePaths.Add(*SearchPath);
	Filter.bRecursivePaths = true; // 启用递归
	Filter.bRecursiveClasses =true;
	Filter.ClassPaths.Add(UBlueprint::StaticClass()->GetClassPathName());

	// 5. 获取资产数据
	TArray<FAssetData> AssetDataList;
	AssetRegistryModule.Get().GetAssets(Filter, AssetDataList);



	TArray<UBlueprint*> FoundBlueprints;
	// 6. 转换为UBlueprint对象
	for (const FAssetData& AssetData : AssetDataList)
	{

		if (UBlueprint* Blueprint = Cast<UBlueprint>(AssetData.GetAsset()))
		{
			// 关键：检查生成的类是否继承自目标父类
			if (Blueprint->GeneratedClass &&
				Blueprint->GeneratedClass->IsChildOf(UGyraInventoryItemDefinition::StaticClass()))
			{
				FoundBlueprints.Add(Blueprint);

			
			}
			

		}
	}

	for (auto& Tmp: FoundBlueprints)
	{
		UObject* ItemCDO= Cast<UClass>(Tmp->GeneratedClass)->GetDefaultObject(true);
		UGyraInventoryItemDefinition* ItemDef =Cast<UGyraInventoryItemDefinition>(ItemCDO);
	
		
		if (ItemDef)
		{
			ItemDef->UniqueName;

			FDataTableEditorUtils::RemoveRow(DT_InventoryList, ItemDef->UniqueName);
			FGyraInventoryItemIndexInfo ItemIndexInfo;
			ItemIndexInfo.InventoryItemDefinition = Tmp->GeneratedClass;

			uint8* DataPtr = FDataTableEditorUtils::AddRow(DT_InventoryList, ItemDef->UniqueName);
			FMemory::Memcpy(DataPtr, &ItemIndexInfo, FGyraInventoryItemIndexInfo::StaticStruct()->GetStructureSize());


		}
	;
	}

	DT_InventoryList->MarkPackageDirty();

	//FAssetRegistryModule::AssetSaved(*DT_InventoryList);
}
