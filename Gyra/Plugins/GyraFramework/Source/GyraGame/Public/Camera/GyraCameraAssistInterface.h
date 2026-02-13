// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "GyraCameraAssistInterface.generated.h"

/** */
UINTERFACE(BlueprintType)
class UGyraCameraAssistInterface : public UInterface
{
	GENERATED_BODY()
};

class IGyraCameraAssistInterface
{
	GENERATED_BODY()

public:
	/**
	 * Get the list of actors that we're allowing the camera to penetrate. Useful in 3rd person cameras
	 * when you need the following camera to ignore things like the a collection of view targets, the pawn,
	 * a vehicle..etc.
	 * 
	 * 拿到我们允许镜头进入的演员名单。在第三人称摄像机中非常有用，当你需要下面的摄像机来忽略诸如视图目标集合，棋子，车辆等。
	 */
	virtual void GetIgnoredActorsForCameraPentration(TArray<const AActor*>& OutActorsAllowPenetration) const { }

	/**
	 * The target actor to prevent penetration on.  Normally, this is almost always the view target, which if
	 * unimplemented will remain true.  However, sometimes the view target, isn't the same as the root actor
	 * you need to keep in frame.
	 * 
	 * 防止渗透的目标角色。通常，这几乎总是视图目标，如果未实现，它将保持为真。然而，有时视图目标与需要保存在框架中的根参与者并不相同。
	 */
	virtual TOptional<AActor*> GetCameraPreventPenetrationTarget() const
	{
		return TOptional<AActor*>();
	}

	/** Called if the camera penetrates the focal target.  Useful if you want to hide the target actor when being overlapped. */
	/** 如果摄像机穿透了焦点目标就会调用。如果您想在重叠时隐藏目标角色，则非常有用。 */
	virtual void OnCameraPenetratingTarget() { }
};
