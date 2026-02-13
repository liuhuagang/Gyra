// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "GameFramework/CharacterMovementComponent.h"
#include "NativeGameplayTags.h"

#include "GyraCharacterMovementComponent.generated.h"

class UObject;
struct FFrame;

GYRAGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_MovementStopped);

/**
 * FGyraCharacterGroundInfo
 *
 * Information about the ground under the character.  It only gets updated as needed.
 * 关于人物下地面的信息。它只在需要时更新。
 */
USTRUCT(BlueprintType)
struct FGyraCharacterGroundInfo
{
	GENERATED_BODY()

	FGyraCharacterGroundInfo()
		: LastUpdateFrame(0)
		, GroundDistance(0.0f)
	{}

	uint64 LastUpdateFrame;

	UPROPERTY(BlueprintReadOnly)
	FHitResult GroundHitResult;

	UPROPERTY(BlueprintReadOnly)
	float GroundDistance;
};


/**
 * UGyraCharacterMovementComponent
 *
 * The base character movement component class used by this project.
 * 这个项目使用的基本字符移动组件类。
 */
UCLASS(Config = Game)
class GYRAGAME_API UGyraCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:

	UGyraCharacterMovementComponent(const FObjectInitializer& ObjectInitializer);

	virtual void SimulateMovement(float DeltaTime) override;

	virtual bool CanAttemptJump() const override;

	// Returns the current ground info.  Calling this will update the ground info if it's out of date.
	UFUNCTION(BlueprintCallable, Category = "Gyra|CharacterMovement")
	const FGyraCharacterGroundInfo& GetGroundInfo();

	void SetReplicatedAcceleration(const FVector& InAcceleration);

	//~UMovementComponent interface
	virtual FRotator GetDeltaRotation(float DeltaTime) const override;
	virtual float GetMaxSpeed() const override;
	//~End of UMovementComponent interface

protected:

	virtual void InitializeComponent() override;

protected:

	// Cached ground info for the character.  Do not access this directly!  It's only updated when accessed via GetGroundInfo().
	FGyraCharacterGroundInfo CachedGroundInfo;

	UPROPERTY(Transient)
	bool bHasReplicatedAcceleration = false;
};
