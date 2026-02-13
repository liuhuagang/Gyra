// Copyright 2024 Huagang Liu. All Rights Reserved.

#include "GyraAbilitySet.h"

#include "GyraGameData.h"
#include "GyraAssetManager.h"

#include "GyraGameplayAbility.h"
#include "GyraAbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"


#include "LogGyraGame.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GyraAbilitySet)

void FGyraAbilitySet_GrantedHandles::AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle)
{
	if (Handle.IsValid())
	{
		AbilitySpecHandles.Add(Handle);
	}
}

void FGyraAbilitySet_GrantedHandles::AddGameplayEffectHandle(const FActiveGameplayEffectHandle& Handle)
{
	if (Handle.IsValid())
	{
		GameplayEffectHandles.Add(Handle);
	}
}

void FGyraAbilitySet_GrantedHandles::AddAttributeSet(UAttributeSet* Set)
{
	GrantedAttributeSets.Add(Set);
}

void FGyraAbilitySet_GrantedHandles::TakeFromAbilitySystem(UGyraAbilitySystemComponent* GyraASC)
{
	check(GyraASC);

	if (!GyraASC->IsOwnerActorAuthoritative())
	{
		// Must be authoritative to give or take ability sets.
		//给予或接受能力集必须具有权威性。
		return;
	}

	for (const FGameplayAbilitySpecHandle& Handle : AbilitySpecHandles)
	{
		if (Handle.IsValid())
		{
			GyraASC->ClearAbility(Handle);
		}
	}

	for (const FActiveGameplayEffectHandle& Handle : GameplayEffectHandles)
	{
		if (Handle.IsValid())
		{
			GyraASC->RemoveActiveGameplayEffect(Handle);
		}
	}

	for (UAttributeSet* Set : GrantedAttributeSets)
	{
		GyraASC->RemoveSpawnedAttribute(Set);
	}




	AbilitySpecHandles.Reset();
	GameplayEffectHandles.Reset();
	GrantedAttributeSets.Reset();

}

UGyraAbilitySet::UGyraAbilitySet(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{


}

void UGyraAbilitySet::GiveToAbilitySystem(UGyraAbilitySystemComponent* GyraASC, 
	const TMap<FGameplayTag, float> & InAttributeMap,
FGyraAbilitySet_GrantedHandles* OutGrantedHandles, 
UObject* SourceObject) const
{
	check(GyraASC);

	if (!GyraASC->IsOwnerActorAuthoritative())
	{
		// Must be authoritative to give or take ability sets.
		return;
	}

	// Grant the attribute sets.
	for (int32 SetIndex = 0; SetIndex < GrantedAttributes.Num(); ++SetIndex)
	{
		const FGyraAbilitySet_AttributeSet& SetToGrant = GrantedAttributes[SetIndex];

		if (!IsValid(SetToGrant.AttributeSet))
		{
			UE_LOG(LogGyraAbilitySystem, Error, TEXT("GrantedAttributes[%d] on ability set [%s] is not valid"), SetIndex, *GetNameSafe(this));
			continue;
		}

		UAttributeSet* NewSet = NewObject<UAttributeSet>(GyraASC->GetOwner(), SetToGrant.AttributeSet);
		GyraASC->AddAttributeSetSubobject(NewSet);

		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddAttributeSet(NewSet);
		}
	}

	// Grant the gameplay abilities.
	for (int32 AbilityIndex = 0; AbilityIndex < GrantedGameplayAbilities.Num(); ++AbilityIndex)
	{
		const FGyraAbilitySet_GameplayAbility& AbilityToGrant = GrantedGameplayAbilities[AbilityIndex];

		if (!IsValid(AbilityToGrant.Ability))
		{
			UE_LOG(LogGyraAbilitySystem, Error, TEXT("GrantedGameplayAbilities[%d] on ability set [%s] is not valid."), AbilityIndex, *GetNameSafe(this));
			continue;
		}

		UGyraGameplayAbility* AbilityCDO = AbilityToGrant.Ability->GetDefaultObject<UGyraGameplayAbility>();

		FGameplayAbilitySpec AbilitySpec(AbilityCDO, AbilityToGrant.AbilityLevel);


		AbilitySpec.SourceObject = SourceObject;
		AbilitySpec.GetDynamicSpecSourceTags().AddTag(AbilityToGrant.InputTag);

		const FGameplayAbilitySpecHandle AbilitySpecHandle = GyraASC->GiveAbility(AbilitySpec);

		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddAbilitySpecHandle(AbilitySpecHandle);
		}
	}

	// Grant the gameplay effects.
	// 授予GE
	for (int32 EffectIndex = 0; EffectIndex < GrantedGameplayEffects.Num(); ++EffectIndex)
	{
		const FGyraAbilitySet_GameplayEffect& EffectToGrant = GrantedGameplayEffects[EffectIndex];

		if (!IsValid(EffectToGrant.GameplayEffect))
		{
			UE_LOG(LogGyraAbilitySystem, Error, TEXT("GrantedGameplayEffects[%d] on ability set [%s] is not valid"), EffectIndex, *GetNameSafe(this));
			continue;
		}



		const UGameplayEffect* GameplayEffect = EffectToGrant.GameplayEffect->GetDefaultObject<UGameplayEffect>();
		const FActiveGameplayEffectHandle GameplayEffectHandle = GyraASC->ApplyGameplayEffectToSelf(GameplayEffect, EffectToGrant.EffectLevel, GyraASC->MakeEffectContext());

		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddGameplayEffectHandle(GameplayEffectHandle);
		}
	}

	//此处施加一个默认的装备属性增加
	if (InAttributeMap.Num()>0)
	{

		for (auto& Tmp : InAttributeMap)
		{
			if (const TSoftClassPtr<UGameplayEffect>* GameplayEffectPath = UGyraGameData::Get().EquipmentGameplayEffect_SetByCaller.Find(Tmp.Key))
			{
				const TSubclassOf<UGameplayEffect> EquipmentGE = UGyraAssetManager::GetSubclass(*GameplayEffectPath);

				if (EquipmentGE)
				{
					//这里暂时未考虑装备等级,以后可用作强化等级
					FGameplayEffectSpecHandle EffectSpecHandle = GyraASC->MakeOutgoingSpec(EquipmentGE, 1, GyraASC->MakeEffectContext());


					UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(EffectSpecHandle, Tmp.Key, Tmp.Value);

					const FActiveGameplayEffectHandle GameplayEffectHandle = GyraASC->BP_ApplyGameplayEffectSpecToSelf(EffectSpecHandle);

					if (OutGrantedHandles)
					{
						OutGrantedHandles->AddGameplayEffectHandle(GameplayEffectHandle);
					}
				}
			}
		}
	}
}
