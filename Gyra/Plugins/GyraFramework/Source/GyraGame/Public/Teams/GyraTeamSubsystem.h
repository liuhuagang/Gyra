// Copyright 2024 Huagang Liu. All Rights Reserved.
#pragma once

#include "Subsystems/WorldSubsystem.h"

#include "GyraTeamSubsystem.generated.h"

class AActor;
class AGyraPlayerState;
class AGyraTeamInfoBase;
class AGyraTeamPrivateInfo;
class AGyraTeamPublicInfo;
class FSubsystemCollectionBase;
class UGyraTeamDisplayAsset;
struct FFrame;
struct FGameplayTag;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGyraTeamDisplayAssetChangedDelegate, const UGyraTeamDisplayAsset*, DisplayAsset);

USTRUCT()
struct FGyraTeamTrackingInfo
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TObjectPtr<AGyraTeamPublicInfo> PublicInfo = nullptr;

	UPROPERTY()
	TObjectPtr<AGyraTeamPrivateInfo> PrivateInfo = nullptr;

	UPROPERTY()
	TObjectPtr<UGyraTeamDisplayAsset> DisplayAsset = nullptr;

	UPROPERTY()
	FOnGyraTeamDisplayAssetChangedDelegate OnTeamDisplayAssetChanged;

public:
	void SetTeamInfo(AGyraTeamInfoBase* Info);
	void RemoveTeamInfo(AGyraTeamInfoBase* Info);
};

// Result of comparing the team affiliation for two actors
// 比较两个演员的团队隶属关系的结果
UENUM(BlueprintType)
enum class EGyraTeamComparison : uint8
{
	// Both actors are members of the same team
	// 两个演员是同一个团队的成员
	OnSameTeam,

	// The actors are members of opposing teams
	// 演员们是对立队伍的成员
	DifferentTeams,

	// One (or both) of the actors was invalid or not part of any team
	// 其中一个（或两个）参与者无效或不属于任何团队
	InvalidArgument
};


/** A subsystem for easy access to team information for team-based actors (e.g., pawns or player states) */
/** 一个子系统，可以方便地访问基于团队的参与者（例如，棋子或玩家状态）的团队信息 注意它是WorldSubsystem 每次关卡就会重置 */
UCLASS()
class GYRAGAME_API UGyraTeamSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UGyraTeamSubsystem();

	//~USubsystem interface
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	//~End of USubsystem interface

	// Tries to registers a new team
	// 尝试注册一个新队伍
	bool RegisterTeamInfo(AGyraTeamInfoBase* TeamInfo);

	// Tries to unregister a team, will return false if it didn't work
	// 尝试注销一个队伍，如果失败将返回false
	bool UnregisterTeamInfo(AGyraTeamInfoBase* TeamInfo);

	// Changes the team associated with this actor if possible
	// 如果可能，更改与此actor关联的团队
	// Note: This function can only be called on the authority
	// 注意：此函数只能在权限上调用
	bool ChangeTeamForActor(AActor* ActorToChange, int32 NewTeamId);

	// Returns the team this object belongs to, or INDEX_NONE if it is not part of a team
	// 返回该对象所属的球队，如果不是球队的一部分，则返回INDEX_NONE
	int32 FindTeamFromObject(const UObject* TestObject) const;

	// Returns the associated player state for this actor, or INDEX_NONE if it is not associated with a player
	// 返回该actor的关联播放器状态，如果没有关联播放器则返回INDEX_NONE
	const AGyraPlayerState* FindPlayerStateFromActor(const AActor* PossibleTeamActor) const;

	// Returns the team this object belongs to, or INDEX_NONE if it is not part of a team
	// 返回该对象所属的球队，如果不是球队的一部分，则返回INDEX_NONE
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = Teams, meta = (Keywords = "Get"))
	void FindTeamFromActor(const UObject* TestActor, bool& bIsPartOfTeam, int32& TeamId) const;

	// Compare the teams of two actors and returns a value indicating if they are on same teams, different teams, or one/both are invalid
	// 比较两个参与者的团队并返回一个值，指示他们是在同一个团队，不同的团队，还是一个/两个无效
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = Teams, meta = (ExpandEnumAsExecs = ReturnValue))
	EGyraTeamComparison CompareTeams(const UObject* A, const UObject* B, int32& TeamIdA, int32& TeamIdB) const;

	// Compare the teams of two actors and returns a value indicating if they are on same teams, different teams, or one/both are invalid
	// 比较两个参与者的团队并返回一个值，指示他们是在同一个团队，不同的团队，还是一个/两个无效
	EGyraTeamComparison CompareTeams(const UObject* A, const UObject* B) const;

	// Returns true if the instigator can damage the target, taking into account the friendly fire settings
	// 考虑到友军火力设置，如果煽动者能够伤害目标，返回true
	bool CanCauseDamage(const UObject* Instigator, const UObject* Target, bool bAllowDamageToSelf = true) const;

	// Adds a specified number of stacks to the tag (does nothing if StackCount is below 1)
	// 为标签添加指定数量的堆栈（如果StackCount低于1，则不执行任何操作）
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Teams)
	void AddTeamTagStack(int32 TeamId, FGameplayTag Tag, int32 StackCount);

	// Removes a specified number of stacks from the tag (does nothing if StackCount is below 1)
	// 从标签中移除指定数量的堆栈（如果StackCount低于1，则不执行任何操作）
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Teams)
	void RemoveTeamTagStack(int32 TeamId, FGameplayTag Tag, int32 StackCount);

	// Returns the stack count of the specified tag (or 0 if the tag is not present)
	// 返回指定标签的堆栈计数（如果标签不存在，则返回0）
	UFUNCTION(BlueprintCallable, Category = Teams)
	int32 GetTeamTagStackCount(int32 TeamId, FGameplayTag Tag) const;

	// Returns true if there is at least one stack of the specified tag
	// 如果指定的标记至少有一个堆栈，则返回true
	UFUNCTION(BlueprintCallable, Category = Teams)
	bool TeamHasTag(int32 TeamId, FGameplayTag Tag) const;

	// Returns true if the specified team exists
	// 如果指定的队伍存在，则返回true
	UFUNCTION(BlueprintCallable, Category = Teams)
	bool DoesTeamExist(int32 TeamId) const;

	// Gets the team display asset for the specified team, from the perspective of the specified team
	// (You have to specify a viewer too, in case the game mode is in a 'local player is always blue team' sort of situation)
	// 从指定团队的角度获取指定团队的团队显示资产
	// （你也必须指定一个观察者，以防游戏模式是在“本地玩家总是蓝队”的情况下）
	UFUNCTION(BlueprintCallable, Category = Teams)
	UGyraTeamDisplayAsset* GetTeamDisplayAsset(int32 TeamId, int32 ViewerTeamId);

	// Gets the team display asset for the specified team, from the perspective of the specified team
	// (You have to specify a viewer too, in case the game mode is in a 'local player is always blue team' sort of situation)
	// 从指定团队的角度获取指定团队的团队显示资产
	// （你也必须指定一个观察者，以防游戏模式是在“本地玩家总是蓝队”的情况下）
	UFUNCTION(BlueprintCallable, Category = Teams)
	UGyraTeamDisplayAsset* GetEffectiveTeamDisplayAsset(int32 TeamId, UObject* ViewerTeamAgent);

	// Gets the list of teams
	// 获取球队列表
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = Teams)
	TArray<int32> GetTeamIDs() const;

	// Called when a team display asset has been edited, causes all team color observers to update
	// 当团队显示资源被编辑时调用，导致所有团队颜色观察者更新
	void NotifyTeamDisplayAssetModified(UGyraTeamDisplayAsset* ModifiedAsset);

	// Register for a team display asset notification for the specified team ID
	// 为指定的团队ID注册一个团队显示资产通知
	FOnGyraTeamDisplayAssetChangedDelegate& GetTeamDisplayAssetChangedDelegate(int32 TeamId);

private:
	UPROPERTY()
	TMap<int32, FGyraTeamTrackingInfo> TeamMap;

	FDelegateHandle CheatManagerRegistrationHandle;
};
