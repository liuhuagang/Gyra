// Copyright 2024 Huagang Liu. All Rights Reserved.

#include "GyraCharacterBase.h"

#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"

#include "GyraAbilitySystemComponent.h"

#include "GyraHealthComponent.h"
#include "GyraStatsComponent.h"
#include "GyraPawnExtensionComponent.h"
#include "GyraCharacterMovementComponent.h"

#include "GyraGameplayTags.h"

#include "Player/GyraPlayerController.h"
#include "Player/GyraPlayerState.h"


#include "LogGyraGame.h"

static FName NAME_GyraCharacterCollisionProfile_Capsule(TEXT("GyraPawnCapsule"));
static FName NAME_GyraCharacterCollisionProfile_Mesh(TEXT("GyraPawnMesh"));

AGyraCharacterBase::AGyraCharacterBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UGyraCharacterMovementComponent>(
		ACharacter::CharacterMovementComponentName))
{
	// Avoid ticking characters if possible.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	check(CapsuleComp);
	CapsuleComp->InitCapsuleSize(40.0f, 90.0f);
	CapsuleComp->SetCollisionProfileName(NAME_GyraCharacterCollisionProfile_Capsule);

	USkeletalMeshComponent* MeshComp = GetMesh();
	check(MeshComp);
	MeshComp->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	// Rotate mesh to be X forward since it is exported as Y forward.
	MeshComp->SetCollisionProfileName(NAME_GyraCharacterCollisionProfile_Mesh);

	UGyraCharacterMovementComponent* GyraMoveComp = CastChecked<
		UGyraCharacterMovementComponent>(GetCharacterMovement());
	GyraMoveComp->GravityScale = 1.0f;
	GyraMoveComp->MaxAcceleration = 2400.0f;
	GyraMoveComp->BrakingFrictionFactor = 1.0f;
	GyraMoveComp->BrakingFriction = 6.0f;
	GyraMoveComp->GroundFriction = 8.0f;
	GyraMoveComp->BrakingDecelerationWalking = 1400.0f;
	GyraMoveComp->bUseControllerDesiredRotation = false;
	GyraMoveComp->bOrientRotationToMovement = false;
	GyraMoveComp->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
	GyraMoveComp->bAllowPhysicsRotationDuringAnimRootMotion = false;
	GyraMoveComp->GetNavAgentPropertiesRef().bCanCrouch = true;
	GyraMoveComp->bCanWalkOffLedgesWhenCrouching = true;
	GyraMoveComp->SetCrouchedHalfHeight(65.0f);

	PawnExtComponent = CreateDefaultSubobject<UGyraPawnExtensionComponent>(TEXT("PawnExtensionComponent"));
	
	PawnExtComponent->OnAbilitySystemInitialized_RegisterAndCall(
		FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAbilitySystemInitialized));
	
	PawnExtComponent->OnAbilitySystemUninitialized_Register(
		FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAbilitySystemUninitialized));

	StatsComponent = CreateDefaultSubobject<UGyraStatsComponent>(TEXT("StatsComponent"));

	HealthComponent = CreateDefaultSubobject<UGyraHealthComponent>(TEXT("HealthComponent"));
	HealthComponent->OnDeathStarted.AddDynamic(this, &ThisClass::OnDeathStarted);
	HealthComponent->OnDeathFinished.AddDynamic(this, &ThisClass::OnDeathFinished);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	BaseEyeHeight = 80.0f;
	CrouchedEyeHeight = 50.0f;
}

AGyraPlayerController* AGyraCharacterBase::GetGyraPlayerController() const
{
	return CastChecked<AGyraPlayerController>(Controller, ECastCheckedType::NullAllowed);
}

AGyraPlayerState* AGyraCharacterBase::GetGyraPlayerState() const
{
	return CastChecked<AGyraPlayerState>(GetPlayerState(), ECastCheckedType::NullAllowed);
}

UGyraAbilitySystemComponent* AGyraCharacterBase::GetGyraAbilitySystemComponent() const
{
	return Cast<UGyraAbilitySystemComponent>(GetAbilitySystemComponent());
}

UAbilitySystemComponent* AGyraCharacterBase::GetAbilitySystemComponent() const
{
	if (PawnExtComponent == nullptr)
	{
		return nullptr;
	}

	return PawnExtComponent->GetGyraAbilitySystemComponent();
}

void AGyraCharacterBase::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	if (const UGyraAbilitySystemComponent* GyraASC = GetGyraAbilitySystemComponent())
	{
		GyraASC->GetOwnedGameplayTags(TagContainer);
	}
}

bool AGyraCharacterBase::HasMatchingGameplayTag(FGameplayTag TagToCheck) const
{
	if (const UGyraAbilitySystemComponent* GyraASC = GetGyraAbilitySystemComponent())
	{
		return GyraASC->HasMatchingGameplayTag(TagToCheck);
	}

	return false;
}

bool AGyraCharacterBase::HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	if (const UGyraAbilitySystemComponent* GyraASC = GetGyraAbilitySystemComponent())
	{
		return GyraASC->HasAllMatchingGameplayTags(TagContainer);
	}

	return false;
}

bool AGyraCharacterBase::HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	if (const UGyraAbilitySystemComponent* GyraASC = GetGyraAbilitySystemComponent())
	{
		return GyraASC->HasAnyMatchingGameplayTags(TagContainer);
	}

	return false;
}


void AGyraCharacterBase::PreInitializeComponents()
{
	Super::PreInitializeComponents();
}

void AGyraCharacterBase::BeginPlay()
{
	Super::BeginPlay();
}

void AGyraCharacterBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AGyraCharacterBase::Reset()
{
	DisableMovementAndCollision();

	K2_OnReset();

	UninitAndDestroy();
}

void AGyraCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ThisClass, ReplicatedAcceleration, COND_SimulatedOnly);
	DOREPLIFETIME(ThisClass, MyTeamID)
}

void AGyraCharacterBase::PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker)
{
	Super::PreReplication(ChangedPropertyTracker);

	if (UCharacterMovementComponent* MovementComponent = GetCharacterMovement())
	{
		// Compress Acceleration: XY components as direction + magnitude, Z component as direct value
		const double MaxAccel = MovementComponent->MaxAcceleration;
		const FVector CurrentAccel = MovementComponent->GetCurrentAcceleration();
		double AccelXYRadians, AccelXYMagnitude;
		FMath::CartesianToPolar(CurrentAccel.X, CurrentAccel.Y, AccelXYMagnitude, AccelXYRadians);

		ReplicatedAcceleration.AccelXYRadians = FMath::FloorToInt((AccelXYRadians / TWO_PI) * 255.0);
		// [0, 2PI] -> [0, 255]
		ReplicatedAcceleration.AccelXYMagnitude = FMath::FloorToInt((AccelXYMagnitude / MaxAccel) * 255.0);
		// [0, MaxAccel] -> [0, 255]
		ReplicatedAcceleration.AccelZ = FMath::FloorToInt((CurrentAccel.Z / MaxAccel) * 127.0);
		// [-MaxAccel, MaxAccel] -> [-127, 127]
	}
}

void AGyraCharacterBase::NotifyControllerChanged()
{
	const FGenericTeamId OldTeamId = GetGenericTeamId();

	Super::NotifyControllerChanged();

	// Update our team ID based on the controller
	if (HasAuthority() && (Controller != nullptr))
	{
		if (IGyraTeamAgentInterface* ControllerWithTeam = Cast<IGyraTeamAgentInterface>(Controller))
		{
			MyTeamID = ControllerWithTeam->GetGenericTeamId();
			ConditionalBroadcastTeamChanged(this, OldTeamId, MyTeamID);
		}
	}
}


void AGyraCharacterBase::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	if (GetController() == nullptr)
	{
		if (HasAuthority())
		{
			const FGenericTeamId OldTeamID = MyTeamID;
			MyTeamID = NewTeamID;
			ConditionalBroadcastTeamChanged(this, OldTeamID, MyTeamID);
		}
		else
		{
			UE_LOG(LogGyraTeams, Error, TEXT("You can't set the team ID on a character (%s) except on the authority"),
			       *GetPathNameSafe(this));
		}
	}
	else
	{
		UE_LOG(LogGyraTeams, Error,
		       TEXT("You can't set the team ID on a possessed character (%s); it's driven by the associated controller"
		       ), *GetPathNameSafe(this));
	}
}

FGenericTeamId AGyraCharacterBase::GetGenericTeamId() const
{
	return MyTeamID;
}

FOnGyraTeamIndexChangedDelegate* AGyraCharacterBase::GetOnTeamIndexChangedDelegate()
{
	return &OnTeamChangedDelegate;
}

FName AGyraCharacterBase::GetGyraPawnInitInfoName()
{
	return PawnExtComponent->GetGyraPawnName();
}


void AGyraCharacterBase::ToggleCrouch()
{
	const UGyraCharacterMovementComponent* GyraMoveComp = CastChecked<UGyraCharacterMovementComponent>(
		GetCharacterMovement());

	if (bIsCrouched || GyraMoveComp->bWantsToCrouch)
	{
		UnCrouch();
	}
	else if (GyraMoveComp->IsMovingOnGround())
	{
		Crouch();
	}
}

void AGyraCharacterBase::OnAbilitySystemInitialized()
{
	UGyraAbilitySystemComponent* GyraASC = GetGyraAbilitySystemComponent();
	check(GyraASC);

	StatsComponent->InitializeWithAbilitySystem(GyraASC);
	HealthComponent->InitializeWithAbilitySystem(GyraASC);

	InitializeGameplayTags();
}

void AGyraCharacterBase::OnAbilitySystemUninitialized()
{
	HealthComponent->UninitializeFromAbilitySystem();
	StatsComponent->UninitializeFromAbilitySystem();
}


void AGyraCharacterBase::PossessedBy(AController* NewController)
{
	const FGenericTeamId OldTeamID = MyTeamID;

	Super::PossessedBy(NewController);

	PawnExtComponent->HandleControllerChanged();

	// Grab the current team ID and listen for future changes
	if (IGyraTeamAgentInterface* ControllerAsTeamProvider = Cast<IGyraTeamAgentInterface>(NewController))
	{
		MyTeamID = ControllerAsTeamProvider->GetGenericTeamId();
		ControllerAsTeamProvider->GetTeamChangedDelegateChecked().AddDynamic(this, &ThisClass::OnControllerChangedTeam);
	}
	ConditionalBroadcastTeamChanged(this, OldTeamID, MyTeamID);
}

void AGyraCharacterBase::UnPossessed()
{
	AController* const OldController = Controller;

	// Stop listening for changes from the old controller
	const FGenericTeamId OldTeamID = MyTeamID;
	if (IGyraTeamAgentInterface* ControllerAsTeamProvider = Cast<IGyraTeamAgentInterface>(OldController))
	{
		ControllerAsTeamProvider->GetTeamChangedDelegateChecked().RemoveAll(this);
	}

	Super::UnPossessed();

	PawnExtComponent->HandleControllerChanged();

	// Determine what the new team ID should be afterwards
	MyTeamID = DetermineNewTeamAfterPossessionEnds(OldTeamID);
	ConditionalBroadcastTeamChanged(this, OldTeamID, MyTeamID);
}

void AGyraCharacterBase::OnRep_Controller()
{
	Super::OnRep_Controller();

	PawnExtComponent->HandleControllerChanged();
}

void AGyraCharacterBase::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	PawnExtComponent->HandlePlayerStateReplicated();
}

void AGyraCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PawnExtComponent->SetupPlayerInputComponent();
}

void AGyraCharacterBase::InitializeGameplayTags()
{
	// Clear tags that may be lingering on the ability system from the previous pawn.
	if (UGyraAbilitySystemComponent* GyraASC = GetGyraAbilitySystemComponent())
	{
		for (const TPair<uint8, FGameplayTag>& TagMapping : GyraGameplayTags::MovementModeTagMap)
		{
			if (TagMapping.Value.IsValid())
			{
				GyraASC->SetLooseGameplayTagCount(TagMapping.Value, 0);
			}
		}

		for (const TPair<uint8, FGameplayTag>& TagMapping : GyraGameplayTags::CustomMovementModeTagMap)
		{
			if (TagMapping.Value.IsValid())
			{
				GyraASC->SetLooseGameplayTagCount(TagMapping.Value, 0);
			}
		}

		UGyraCharacterMovementComponent* GyraMoveComp = CastChecked<UGyraCharacterMovementComponent>(
			GetCharacterMovement());
		SetMovementModeTag(GyraMoveComp->MovementMode, GyraMoveComp->CustomMovementMode, true);
	}
}

void AGyraCharacterBase::FellOutOfWorld(const class UDamageType& dmgType)
{
	HealthComponent->DamageSelfDestruct(/*bFellOutOfWorld=*/ true);
}

void AGyraCharacterBase::OnDeathStarted(AActor*)
{
	DisableMovementAndCollision();
}

void AGyraCharacterBase::OnDeathFinished(AActor*)
{
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::DestroyDueToDeath);
}


void AGyraCharacterBase::DisableMovementAndCollision()
{
	if (Controller)
	{
		Controller->SetIgnoreMoveInput(true);
	}

	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	check(CapsuleComp);
	CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CapsuleComp->SetCollisionResponseToAllChannels(ECR_Ignore);

	UGyraCharacterMovementComponent* GyraMoveComp = CastChecked<
		UGyraCharacterMovementComponent>(GetCharacterMovement());
	GyraMoveComp->StopMovementImmediately();
	GyraMoveComp->DisableMovement();
}

void AGyraCharacterBase::DestroyDueToDeath()
{
	K2_OnDeathFinished();

	UninitAndDestroy();
}

void AGyraCharacterBase::UninitAndDestroy()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		DetachFromControllerPendingDestroy();
		SetLifeSpan(0.1f);
	}

	// Uninitialize the ASC if we're still the avatar actor (otherwise another pawn already did it when they became the avatar actor)
	if (UGyraAbilitySystemComponent* GyraASC = GetGyraAbilitySystemComponent())
	{
		if (GyraASC->GetAvatarActor() == this)
		{
			PawnExtComponent->UninitializeAbilitySystem();
		}
	}

	SetActorHiddenInGame(true);
}

void AGyraCharacterBase::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);

	UGyraCharacterMovementComponent* GyraMoveComp = CastChecked<
		UGyraCharacterMovementComponent>(GetCharacterMovement());

	SetMovementModeTag(PrevMovementMode, PreviousCustomMode, false);
	SetMovementModeTag(GyraMoveComp->MovementMode, GyraMoveComp->CustomMovementMode, true);
}

void AGyraCharacterBase::SetMovementModeTag(EMovementMode MovementMode, uint8 CustomMovementMode, bool bTagEnabled)
{
	if (UGyraAbilitySystemComponent* GyraASC = GetGyraAbilitySystemComponent())
	{
		const FGameplayTag* MovementModeTag = nullptr;
		if (MovementMode == MOVE_Custom)
		{
			MovementModeTag = GyraGameplayTags::CustomMovementModeTagMap.Find(CustomMovementMode);
		}
		else
		{
			MovementModeTag = GyraGameplayTags::MovementModeTagMap.Find(MovementMode);
		}

		if (MovementModeTag && MovementModeTag->IsValid())
		{
			GyraASC->SetLooseGameplayTagCount(*MovementModeTag, (bTagEnabled ? 1 : 0));
		}
	}
}


void AGyraCharacterBase::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	if (UGyraAbilitySystemComponent* GyraASC = GetGyraAbilitySystemComponent())
	{
		GyraASC->SetLooseGameplayTagCount(GyraGameplayTags::Status_Crouching, 1);
	}


	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
}

void AGyraCharacterBase::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	if (UGyraAbilitySystemComponent* GyraASC = GetGyraAbilitySystemComponent())
	{
		GyraASC->SetLooseGameplayTagCount(GyraGameplayTags::Status_Crouching, 0);
	}

	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
}

bool AGyraCharacterBase::CanJumpInternal_Implementation() const
{
	// same as ACharacter's implementation but without the crouch check
	return JumpIsAllowedInternal();
}

FGenericTeamId AGyraCharacterBase::DetermineNewTeamAfterPossessionEnds(FGenericTeamId OldTeamID) const
{
	// This could be changed to return, e.g., OldTeamID if you want to keep it assigned afterwards, or return an ID for some neutral faction, or etc...
	// 这可以修改为返回诸如“OldTeamID”之类的值（如果您希望之后仍保留该团队的标识，则可采用此方式），或者返回某个中立阵营的标识，或者等等。
	return FGenericTeamId::NoTeam;
}


void AGyraCharacterBase::OnRep_MyTeamID(FGenericTeamId OldTeamID)
{
	ConditionalBroadcastTeamChanged(this, OldTeamID, MyTeamID);
}

void AGyraCharacterBase::OnRep_ReplicatedAcceleration()
{
	if (UGyraCharacterMovementComponent* GyraMovementComponent = Cast<UGyraCharacterMovementComponent>(
		GetCharacterMovement()))
	{
		// Decompress Acceleration
		const double MaxAccel = GyraMovementComponent->MaxAcceleration;
		const double AccelXYMagnitude = double(ReplicatedAcceleration.AccelXYMagnitude) * MaxAccel / 255.0;
		// [0, 255] -> [0, MaxAccel]
		const double AccelXYRadians = double(ReplicatedAcceleration.AccelXYRadians) * TWO_PI / 255.0;
		// [0, 255] -> [0, 2PI]

		FVector UnpackedAcceleration(FVector::ZeroVector);
		FMath::PolarToCartesian(AccelXYMagnitude, AccelXYRadians, UnpackedAcceleration.X, UnpackedAcceleration.Y);
		UnpackedAcceleration.Z = double(ReplicatedAcceleration.AccelZ) * MaxAccel / 127.0;
		// [-127, 127] -> [-MaxAccel, MaxAccel]

		GyraMovementComponent->SetReplicatedAcceleration(UnpackedAcceleration);
	}
}

void AGyraCharacterBase::OnControllerChangedTeam(UObject* TeamAgent, int32 OldTeam, int32 NewTeam)
{
	const FGenericTeamId MyOldTeamID = MyTeamID;
	MyTeamID = IntegerToGenericTeamId(NewTeam);
	ConditionalBroadcastTeamChanged(this, MyOldTeamID, MyTeamID);
}


UAnimMontage* AGyraCharacterBase::GetDeathMontage()
{
	int32 MontageNum = DeathMontageList.Num();

	check(MontageNum>0);
	int32 MontageIndex = FMath::RandRange(0, MontageNum - 1);

	UAnimMontage* ThisMontage = DeathMontageList[MontageIndex];

	return ThisMontage;
}

UAnimMontage* AGyraCharacterBase::GetHurtMontage(EGyraCharacterHurtDirection InHurtDirection)
{
	UAnimMontage* ThisMontage = nullptr;

	switch (InHurtDirection)
	{
	case EGyraCharacterHurtDirection::None:
		break;
	case EGyraCharacterHurtDirection::Front:
		ThisMontage = GetHurtMontage_InternalList(HurtFontMontageList);
		break;
	case EGyraCharacterHurtDirection::Back:
		ThisMontage = GetHurtMontage_InternalList(HurtBackMontageList);
		break;
	case EGyraCharacterHurtDirection::Left:
		ThisMontage = GetHurtMontage_InternalList(HurtLeftMontageList);
		break;
	case EGyraCharacterHurtDirection::Right:
		ThisMontage = GetHurtMontage_InternalList(HurtRightMontageList);
		break;
	case EGyraCharacterHurtDirection::Max:
		break;
	default:
		break;
	}
	if (!ThisMontage)
	{
		ThisMontage = DefaultHurtMontage;
	}

	check(ThisMontage);

	return ThisMontage;
}

UAnimMontage* AGyraCharacterBase::GetHurtMontage_InternalList(TArray<TObjectPtr<UAnimMontage>> InAnimList)
{
	UAnimMontage* ThisMontage = nullptr;
	int32 MontageNum = InAnimList.Num();

	if (MontageNum > 0)
	{
		int32 MontageIndex = FMath::RandRange(0, MontageNum - 1);

		ThisMontage = InAnimList[MontageIndex];
	}

	return ThisMontage;
}

UAnimMontage* AGyraCharacterBase::GetStunMontage()
{
	int32 MontageNum = StunMontageList.Num();

	check(MontageNum > 0);
	int32 MontageIndex = FMath::RandRange(0, MontageNum - 1);

	UAnimMontage* ThisMontage = StunMontageList[MontageIndex];

	return ThisMontage;
}
