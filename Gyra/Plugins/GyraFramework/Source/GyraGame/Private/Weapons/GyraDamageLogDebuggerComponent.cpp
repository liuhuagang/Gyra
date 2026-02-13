// Copyright 2024 Huagang Liu. All Rights Reserved.

#include "GyraDamageLogDebuggerComponent.h"

#include "Engine/World.h"
#include "LogGyraGame.h"
#include "GyraVerbMessage.h"
#include "NativeGameplayTags.h"

#include "GyraGameplayTags.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GyraDamageLogDebuggerComponent)


UGyraDamageLogDebuggerComponent::UGyraDamageLogDebuggerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.SetTickFunctionEnable(true);
}

void UGyraDamageLogDebuggerComponent::BeginPlay()
{
	Super::BeginPlay();

	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
	ListenerHandle = MessageSubsystem.RegisterListener(GyraGameplayTags::TAG_Gyra_Damage_Message, this, &ThisClass::OnDamageMessage);
}

void UGyraDamageLogDebuggerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
	MessageSubsystem.UnregisterListener(ListenerHandle);

	Super::EndPlay(EndPlayReason);
}

void UGyraDamageLogDebuggerComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	const double TimeSinceDamage = GetWorld()->GetTimeSeconds() - LastDamageEntryTime;
	if ((TimeSinceDamage >= SecondsBetweenDamageBeforeLogging) && (DamageLog.Num() > 0))
	{
		TArray<FFrameDamageEntry> Entries;
		DamageLog.GenerateValueArray(/*out*/ Entries);
		DamageLog.Reset();

		Entries.Sort([](const FFrameDamageEntry& A, const FFrameDamageEntry& B) { return A.TimeOfFirstHit < B.TimeOfFirstHit; });

		double TotalDamage = 0.0;
		int32 NumImpacts = 0;
		int32 NumFrames = Entries.Num();
		double MinInterval = TNumericLimits<double>::Max();
		double MaxInterval = -TNumericLimits<double>::Max();
		double TotalInterval = 0.0;

		for (int32 i = 0; i < Entries.Num(); ++i)
		{
			FFrameDamageEntry& EntryA = Entries[i];
			NumImpacts += EntryA.NumImpacts;
			TotalDamage += EntryA.SumDamage;

			if (i + 1 < Entries.Num())
			{
				FFrameDamageEntry& EntryB = Entries[i+1];

				const double TimeGap = EntryB.TimeOfFirstHit - EntryA.TimeOfFirstHit;
				MinInterval = FMath::Min(MinInterval, TimeGap);
				MaxInterval = FMath::Max(MaxInterval, TimeGap);
				TotalInterval += TimeGap;
			}
		}

		UE_LOG(LogGyraGame, Warning, TEXT("%d impacts in %d distinct frames over %.2f seconds did %.2f damage"),
			NumImpacts, NumFrames, TotalInterval, TotalDamage);
		if (TotalInterval > 0.0)
		{
			UE_LOG(LogGyraGame, Warning, TEXT("Interval ranged from %.1f ms to %.1f ms (avg %.1f ms)"),
				MinInterval * 1000.0, MaxInterval * 1000.0, (MaxInterval + MinInterval) / 2.0 * 1000.0);
			UE_LOG(LogGyraGame, Warning, TEXT("DPS %.2f"), TotalDamage / TotalInterval);
		}
		UE_LOG(LogGyraGame, Warning, TEXT("\n"));
	}
}

void UGyraDamageLogDebuggerComponent::OnDamageMessage(FGameplayTag Channel, const FGyraVerbMessage& Payload)
{
	if (Payload.Target == GetOwner())
	{
		FFrameDamageEntry& LogEntry = DamageLog.FindOrAdd(GFrameCounter);
		
		if (LogEntry.TimeOfFirstHit == 0.0)
		{
			LogEntry.TimeOfFirstHit = GetWorld()->GetTimeSeconds();
			LastDamageEntryTime = LogEntry.TimeOfFirstHit;
		}
		LogEntry.NumImpacts++;
		LogEntry.SumDamage += -Payload.Magnitude;
	}
}

