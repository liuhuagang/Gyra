// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GyraWorldCollectable.h"
#include "GyraGameSettings.generated.h"

class UDataTable;
class UDataRegistry;
class AGyraWorldCollectable;
struct FGyraItemInstanceTagDefiniton;
enum class EGyraInventoryItemQuality :uint8;
/**
 * Gyra的游戏性设置
 * 用于放置怪物属性表,玩家属性表,物品列表,技能列表等全局数据
 *
 */
UCLASS(config = Game, defaultconfig, meta = (DisplayName = "Gyra Game Settings"))
class UGyraGameSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:

	UGyraGameSettings();

public:

	UPROPERTY(Config, EditDefaultsOnly, Category = "Gyra|Pawn", meta = (DisplayName = "Gyra Pawn List Data Register", AllowedClasses = "/Script/DataRegistry.DataRegistry", ConfigRestartRequired = true))
	FSoftObjectPath PawnListName;

	UPROPERTY(Config, EditDefaultsOnly, Category = "Gyra|Pawn", meta = (DisplayName = "Gyra Character AttributeSet Data Register", AllowedClasses = "/Script/DataRegistry.DataRegistry", ConfigRestartRequired = true))
	FSoftObjectPath CharacterAttributeSetName;

	UPROPERTY(Config, EditDefaultsOnly, Category = "Gyra|Item", meta = (DisplayName = "Gyra Inventory List Data Register", AllowedClasses = "/Script/DataRegistry.DataRegistry", ConfigRestartRequired = true))
	FSoftObjectPath InventoryListName;

	UPROPERTY(Config, EditDefaultsOnly, Category = "Gyra|Item", meta = (DisplayName = "Gyra Drop List Data Register", AllowedClasses = "/Script/DataRegistry.DataRegistry", ConfigRestartRequired = true))
	FSoftObjectPath DropListName;

	UPROPERTY(Config, EditDefaultsOnly, Category = "Gyra|Item", meta = (DisplayName = "Gyra World Collectable Base Class", AllowedClasses = "/Script/GyraGame.GyraWorldCollectable", ConfigRestartRequired = true))
	FSoftClassPath WorldCollectableBaseClassName = AGyraWorldCollectable::StaticClass();

	UPROPERTY(Config, EditDefaultsOnly, Category = "Gyra|Item", meta = (DisplayName = "Gyra Inventory Item Quality Color"))
	TMap<EGyraInventoryItemQuality, FLinearColor> QualityColorList;

	UPROPERTY(Config, EditDefaultsOnly, Category = "Gyra|Item", meta = (DisplayName = "Gyra Inventory Item Random Tag List"))
	TArray<FGyraItemInstanceTagDefiniton> RandomTagDefaultList;

public:

	UFUNCTION(BlueprintCallable, Category = "Gyra|Settings")
	static UGyraGameSettings* GetGyraGameSettings();

	UFUNCTION(BlueprintPure, Category = "Gyra|Settings")
	UDataRegistry* GetPawnListDataRegistry();

	UFUNCTION(BlueprintPure, Category = "Gyra|Settings")
	UDataRegistry* GetCharacterAttributeSetDataRegistry();

	UFUNCTION(BlueprintPure, Category = "Gyra|Settings")
	UDataRegistry* GetInventoryListDataRegistry();

	UFUNCTION(BlueprintPure, Category = "Gyra|Settings")
	UDataRegistry* GetDropListDataRegistry();

	UFUNCTION(BlueprintPure, Category = "Gyra|Settings")
	TSubclassOf<AGyraWorldCollectable> GetWorldCollectableBase();

	UFUNCTION(BlueprintPure, Category = "Gyra|Settings")
	FLinearColor GetItemQualityColor(EGyraInventoryItemQuality InItemQuality);

	UFUNCTION(BlueprintPure, Category = "Gyra|Settings")
	TArray<FGyraItemInstanceTagDefiniton> GetRandomTagDefaultList( );


protected:

	UPROPERTY()
	TObjectPtr<UDataRegistry> PawnListDataRegistry;

	UPROPERTY()
	TObjectPtr<UDataRegistry> CharacterAttributeSetDataRegistry;

	UPROPERTY()
	TObjectPtr<UDataRegistry> InventoryListDataRegistry;

	UPROPERTY()
	TObjectPtr<UDataRegistry> DropListDataRegistry;

	UPROPERTY()
	TSubclassOf<AGyraWorldCollectable> WorldCollectableBaseClass;

};
