// Copyright 2024 Huagang Liu. All Rights Reserved.

#pragma once

#include "GameplayEffectTypes.h"

#include "GyraGameplayEffectContext.generated.h"

class AActor;
class FArchive;
class IGyraAbilitySourceInterface;
class UObject;
class UPhysicalMaterial;


USTRUCT()
struct FGyraGameplayEffectContext : public FGameplayEffectContext
{
	GENERATED_BODY()

	FGyraGameplayEffectContext()
		: FGameplayEffectContext()
	{
	}

	FGyraGameplayEffectContext(AActor* InInstigator, AActor* InEffectCauser)
		: FGameplayEffectContext(InInstigator, InEffectCauser)
	{
	}

	/** Returns the wrapped FGyraGameplayEffectContext from the handle, or nullptr if it doesn't exist or is the wrong type */
	/** 从句柄返回包装的FGyraGameplayEffectContext，如果它不存在或类型错误，则返回nullptr */
	static GYRAGAME_API FGyraGameplayEffectContext* ExtractEffectContext(struct FGameplayEffectContextHandle Handle);

	/** Sets the object used as the ability source */
	/** 设置用作能力源的对象 */
	void SetAbilitySource(const IGyraAbilitySourceInterface* InObject, float InSourceLevel);

	/** Returns the ability source interface associated with the source object. Only valid on the authority. */
	/** 返回与源对象关联的能力源接口。仅对授权有效。 */
	const IGyraAbilitySourceInterface* GetAbilitySource() const;

	virtual FGameplayEffectContext* Duplicate() const override
	{
		FGyraGameplayEffectContext* NewContext = new FGyraGameplayEffectContext();
		*NewContext = *this;
		if (GetHitResult())
		{
			// Does a deep copy of the hit result
			// 执行命中结果的深度拷贝
			NewContext->AddHitResult(*GetHitResult(), true);
		}
		return NewContext;
	}

	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FGyraGameplayEffectContext::StaticStruct();
	}

	/** Overridden to serialize new fields */
	/** 重写以序列化新字段 */
	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess) override;

	/** Returns the physical material from the hit result if there is one */
	/** 如果有的话，返回命中结果的物理材料 */
	const UPhysicalMaterial* GetPhysicalMaterial() const;

public:
	/** ID to allow the identification of multiple bullets that were part of the same cartridge */
	/** ID可以识别属于同一弹壳的多颗子弹 */
	UPROPERTY()
	int32 CartridgeID = -1;

protected:
	/** Ability Source object (should implement IGyraAbilitySourceInterface). NOT replicated currently */
	/** 能力源对象（应该实现IGyraAbilitySourceInterface）。目前没有复制 */
	UPROPERTY()
	TWeakObjectPtr<const UObject> AbilitySourceObject;
};

template<>
struct TStructOpsTypeTraits<FGyraGameplayEffectContext> : public TStructOpsTypeTraitsBase2<FGyraGameplayEffectContext>
{
	enum
	{
		WithNetSerializer = true,
		WithCopy = true
	};
};
