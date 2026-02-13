// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "Engine/DataAsset.h"

#include "GyraUserFacingExperienceDefinition.generated.h"

class FString;
class UCommonSession_HostSessionRequest;
class UObject;
class UTexture2D;
class UUserWidget;
struct FFrame;

/** Description of settings used to display experiences in the UI and start a new session */
/** 用于在UI中显示体验和开始新会话的设置描述 */
UCLASS(BlueprintType)
class UGyraUserFacingExperienceDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	/** The specific map to load */
	/** 要加载的特定映射 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Experience, meta = (AllowedTypes = "Map"))
	FPrimaryAssetId MapID;

	/** The gameplay experience to load */
	/** 游戏体验加载 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Experience, meta = (AllowedTypes = "GyraExperienceDefinition"))
	FPrimaryAssetId ExperienceID;

	/** Extra arguments passed as URL options to the game */
	/** 作为URL选项传递给游戏的额外参数 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Experience)
	TMap<FString, FString> ExtraArgs;

	/** Primary title in the UI */
	/** UI中的主标题 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Experience)
	FText TileTitle;

	/** Secondary title */
	/** 二级标题 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Experience)
	FText TileSubTitle;

	/** Full description */
	/** 完整描述 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Experience)
	FText TileDescription;

	/** Icon used in the UI */
	/** UI中使用的图标 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Experience)
	TObjectPtr<UTexture2D> TileIcon;

	/** The loading screen widget to show when loading into (or back out of) a given experience */
	/** 加载屏幕小部件显示加载到（或退出）一个给定的体验 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = LoadingScreen)
	TSoftClassPtr<UUserWidget> LoadingScreenWidget;

	/** If true, this is a default experience that should be used for quick play and given priority in the UI */
	/** 如果为真，这是一个默认的体验，应该用于快速播放并在UI中给予优先级 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Experience)
	bool bIsDefaultExperience = false;

	/** If true, this will show up in the experiences list in the front-end */
	/** 如果为true，这将显示在前端的体验列表中 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Experience)
	bool bShowInFrontEnd = true;

	/** If true, a replay will be recorded of the game */
	/** 如果为true，将记录比赛重放 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Experience)
	bool bRecordReplay = false;

	/** Max number of players for this session */
	/** 此会话的最大玩家数 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Experience)
	int32 MaxPlayerCount = 16;

public:
	/** Create a request object that is used to actually start a session with these settings */
	/** 创建一个request对象，用这些设置来实际启动会话 */
	UFUNCTION(BlueprintCallable, BlueprintPure = false, meta = (WorldContext = "WorldContextObject"))
	UCommonSession_HostSessionRequest* CreateHostingRequest(const UObject* WorldContextObject) const;
};

