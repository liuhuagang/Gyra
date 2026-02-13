// Copyright 2024 Huagang Liu. All Rights Reserved.

#include "GyraGameplayEffectContext.h"

#include "GyraAbilitySourceInterface.h"
#include "Engine/HitResult.h"
#include "PhysicalMaterials/PhysicalMaterial.h"

#if UE_WITH_IRIS
#include "Iris/ReplicationState/PropertyNetSerializerInfoRegistry.h"
#include "Serialization/GameplayEffectContextNetSerializer.h"
#endif

#include UE_INLINE_GENERATED_CPP_BY_NAME(GyraGameplayEffectContext)

class FArchive;

FGyraGameplayEffectContext* FGyraGameplayEffectContext::ExtractEffectContext(struct FGameplayEffectContextHandle Handle)
{
	FGameplayEffectContext* BaseEffectContext = Handle.Get();
	if ((BaseEffectContext != nullptr) && BaseEffectContext->GetScriptStruct()->IsChildOf(FGyraGameplayEffectContext::StaticStruct()))
	{
		return (FGyraGameplayEffectContext*)BaseEffectContext;
	}

	return nullptr;
}

bool FGyraGameplayEffectContext::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	FGameplayEffectContext::NetSerialize(Ar, Map, bOutSuccess);

	// Not serialized for post-activation use:
	// 未序列化用于激活后使用的：
	// CartridgeID
	// 子弹ID
	return true;

}

#if UE_WITH_IRIS
namespace UE::Net
{
	// Forward to FGameplayEffectContextNetSerializer
	// Note: If FGyraGameplayEffectContext::NetSerialize() is modified, a custom NetSerializesr must be implemented as the current fallback will no longer be sufficient.
	UE_NET_IMPLEMENT_FORWARDING_NETSERIALIZER_AND_REGISTRY_DELEGATES(GyraGameplayEffectContext, FGameplayEffectContextNetSerializer);
}
#endif

void FGyraGameplayEffectContext::SetAbilitySource(const IGyraAbilitySourceInterface* InObject, float InSourceLevel)
{
	AbilitySourceObject = MakeWeakObjectPtr(Cast<const UObject>(InObject));
	//SourceLevel = InSourceLevel;
}

const IGyraAbilitySourceInterface* FGyraGameplayEffectContext::GetAbilitySource() const
{
	return Cast<IGyraAbilitySourceInterface>(AbilitySourceObject.Get());
}

const UPhysicalMaterial* FGyraGameplayEffectContext::GetPhysicalMaterial() const
{
	if (const FHitResult* HitResultPtr = GetHitResult())
	{
		return HitResultPtr->PhysMaterial.Get();
	}
	return nullptr;
}

