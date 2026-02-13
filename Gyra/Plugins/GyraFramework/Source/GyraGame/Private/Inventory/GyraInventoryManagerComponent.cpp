// Copyright 2024 Huagang Liu. All Rights Reserved.

#include "GyraInventoryManagerComponent.h"

#include "Engine/ActorChannel.h"
#include "Engine/World.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "GyraInventoryItemDefinition.h"
#include "GyraInventoryItemInstance.h"
#include "NativeGameplayTags.h"
#include "Net/UnrealNetwork.h"

#include "GyraGameState.h"
#include "GyraPlayerController.h"
#include "GyraExperienceDefinition.h"
#include "GyraExperienceManagerComponent.h"
#include "GyraArchiveSubsystem.h"
#include "GyraCharacterBase.h"
#include "NavigationSystem.h"

#include "LogGyraGame.h"
#include "GyraInventoryFunctionLibrary.h"


//#include UE_INLINE_GENERATED_CPP_BY_NAME(GyraInventoryManagerComponent)

class FLifetimeProperty;
struct FReplicationFlags;

//////////////////////////////////////////////////////////////////////
// UGyraInventoryManagerComponent

UGyraInventoryManagerComponent::UGyraInventoryManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	  , QuickEquipmentList(this)
	  , PassiveEquipmentList(this)
	  , BackpackList(this)
	  , WarehouseList(this)
{
	SetIsReplicatedByDefault(true);
}

void UGyraInventoryManagerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, QuickEquipmentList);
	DOREPLIFETIME(ThisClass, PassiveEquipmentList);
	DOREPLIFETIME(ThisClass, BackpackList);
	DOREPLIFETIME(ThisClass, WarehouseList);
}

#pragma region ListInnerOperation

FGyraInventoryList& UGyraInventoryManagerComponent::GetOperatedInventroyListByEnum(EGyraInventoryListType InListType)
{
	switch (InListType)
	{
	case EGyraInventoryListType::None:
		check(false);
		break;

	case EGyraInventoryListType::QuickEquipment:

		return QuickEquipmentList;
		break;

	case EGyraInventoryListType::PassiveEquipment:

		return PassiveEquipmentList;
		break;

	case EGyraInventoryListType::Backpack:
		return BackpackList;
		break;

	case EGyraInventoryListType::Warehouse:
		return WarehouseList;
		break;

	case EGyraInventoryListType::Max:
		check(false);
		break;

	default:
		check(false);
		break;
	}

	//此处必要要有返回值,就先随意返回一个,上面已经做了断言
	//不能返回栈上的临时变量,这是个很容易犯错的地方,栈上的临时变量过了作用域就析构了,再访问就直接崩溃了!
	return BackpackList;
}

FGyraArchiveListInfomation UGyraInventoryManagerComponent::GetArchiveListInfomationByEnum(
	EGyraInventoryListType InListType)
{
	UGyraArchiveSubsystem* ArchiveSubsystem = UGyraArchiveSubsystem::GetArchiveSubsystem();

	FGyraArchiveListInfomation ArchiveListInfomation;

	switch (InListType)
	{
	case EGyraInventoryListType::None:
		break;

	case EGyraInventoryListType::QuickEquipment:
		ArchiveListInfomation = ArchiveSubsystem->ActiveSlotInformation->QuickEquipmentList;
		break;

	case EGyraInventoryListType::PassiveEquipment:
		ArchiveListInfomation = ArchiveSubsystem->ActiveSlotInformation->PassiveEquipmentList;
		break;

	case EGyraInventoryListType::Backpack:
		ArchiveListInfomation = ArchiveSubsystem->ActiveSlotInformation->BackpackList;
		break;

	case EGyraInventoryListType::Warehouse:
		ArchiveListInfomation = ArchiveSubsystem->PanelInformation->WarehouseList;
		break;

	case EGyraInventoryListType::Max:
		break;

	default:
		break;
	}

	return ArchiveListInfomation;
}

void UGyraInventoryManagerComponent::InitInventoryList(EGyraInventoryListType InListType, int32 InListNum)
{
	AActor* OwningActor = GetOwner();

	if (!OwningActor || !OwningActor->HasAuthority())
	{
		check(false);
	}

	FGyraInventoryList& OperatedInventoryList = GetOperatedInventroyListByEnum(InListType);

	TArray<UGyraInventoryItemInstance*> Results = OperatedInventoryList.InitEmptyList(InListNum);
}


void UGyraInventoryManagerComponent::NetInitInventoryArchiveList_Implementation(
	EGyraInventoryListType InListType, FGyraArchiveListInfomation InListInfo, int32 InListNum)
{
	AActor* OwningActor = GetOwner();

	if (!OwningActor || !OwningActor->HasAuthority())
	{
		check(false);
	}
	FGyraInventoryList& OperatedInventoryList = GetOperatedInventroyListByEnum(InListType);
	TArray<UGyraInventoryItemInstance*> Results = OperatedInventoryList.InitArchiveList(InListInfo, InListNum);

	CallClientFinishInitInventoryArchiveList(InListType);
}

void UGyraInventoryManagerComponent::CallClientFinishInitInventoryArchiveList_Implementation(
	EGyraInventoryListType InListType)
{
	RepArchiveMap.Add(InListType, true);
}

bool UGyraInventoryManagerComponent::CanAddItemDefinition(EGyraInventoryListType InListType,
                                                          TSubclassOf<UGyraInventoryItemDefinition> ItemDef,
                                                          int32 StackCount)
{
	check(ItemDef);

	if (ItemDef != nullptr)
	{
		FGyraInventoryList& OperatedInventoryList = GetOperatedInventroyListByEnum(InListType);

		return OperatedInventoryList.CheckOverAddEntry(ItemDef, StackCount);
	}

	return false;
}

UGyraInventoryItemInstance* UGyraInventoryManagerComponent::AddItemDefinition(EGyraInventoryListType InListType,
                                                                              TSubclassOf<UGyraInventoryItemDefinition>
                                                                              ItemDef,
                                                                              bool& bSuccess)
{
	UGyraInventoryItemInstance* ResultObj = nullptr;

	int32 StackCount = 1;
	bool bAddToFree = true;

	check(ItemDef);

	if (ItemDef != nullptr)
	{
		FGyraInventoryList& OperatedInventoryList = GetOperatedInventroyListByEnum(InListType);

		ResultObj = OperatedInventoryList.AddEntry(ItemDef, bSuccess, StackCount, bAddToFree);
	}
	return ResultObj;
}

bool UGyraInventoryManagerComponent::OverAddItemDefinition(EGyraInventoryListType InListType,
                                                           TSubclassOf<UGyraInventoryItemDefinition> ItemDef,
                                                           int32 StackCount)
{
	check(ItemDef);

	if (ItemDef != nullptr)
	{
		FGyraInventoryList& OperatedInventoryList = GetOperatedInventroyListByEnum(InListType);

		return OperatedInventoryList.OverAddEntry(ItemDef, StackCount);
	}
	return false;
}

bool UGyraInventoryManagerComponent::CanAddItemInstance(EGyraInventoryListType InListType,
                                                        UGyraInventoryItemInstance* InItemInstance, int32 StackCount)
{
	check(InItemInstance);

	if (InItemInstance != nullptr)
	{
		FGyraInventoryList& OperatedInventoryList = GetOperatedInventroyListByEnum(InListType);

		return OperatedInventoryList.CheckAddEntry(InItemInstance, StackCount);
	}
	return false;
}

bool UGyraInventoryManagerComponent::AddItemInstance(EGyraInventoryListType InListType,
                                                     UGyraInventoryItemInstance* InItemInstance,
                                                     int32 InStackCount = 1)
{
	check(InItemInstance);

	FGyraInventoryList& OperatedInventoryList = GetOperatedInventroyListByEnum(InListType);

	return OperatedInventoryList.AddEntry(InItemInstance, InStackCount);
}


void UGyraInventoryManagerComponent::RemoveItemInstance(EGyraInventoryListType InListType,
                                                        UGyraInventoryItemInstance* ItemInstance)
{
	FGyraInventoryList& OperatedInventoryList = GetOperatedInventroyListByEnum(InListType);

	OperatedInventoryList.RemoveEntry(ItemInstance);
}

void UGyraInventoryManagerComponent::CallServerRemoveItemInstance_Implementation(
	EGyraInventoryListType InListType, UGyraInventoryItemInstance* ItemInstance)
{
	bool bRightA = GetItemInstanceInList(ItemInstance) == InListType;

	if (!bRightA)
	{
		return;
	}
	RemoveItemInstance(InListType, ItemInstance);
}

void UGyraInventoryManagerComponent::CallServerDropItemInstance_Implementation(
	EGyraInventoryListType InListType, UGyraInventoryItemInstance* ItemInstance)
{
	bool bRightA = GetItemInstanceInList(ItemInstance) == InListType;

	if (!bRightA)
	{
		return;
	}

	AActor* OwningActor = GetOwner();

	AGyraPlayerController* ThisPlayerController = Cast<AGyraPlayerController>(OwningActor);

	ACharacter* ThisCharacter = ThisPlayerController->GetCharacter();

	//通过玩家控制的对象来寻找位置
	if (ThisCharacter != nullptr)
	{
		int32 StackNum = GetItemCountByInstance(InListType, ItemInstance);

		FVector RandomLocation;
		bool bLocationItem = UNavigationSystemV1::K2_GetRandomReachablePointInRadius(
			OwningActor,
			ThisCharacter->GetActorLocation(),
			RandomLocation, 500);

		if (bLocationItem)
		{
			AGyraWorldCollectable* DropItem = UGyraInventoryFunctionLibrary::SpawnDropItemInstance(
				OwningActor, OwningActor,
				RandomLocation, ItemInstance, StackNum);

			//在场景创建成功才移除
			if (DropItem != nullptr)
			{
				RemoveItemInstance(InListType, ItemInstance);
			}
			else
			{
				UE_LOG(LogGyraGame, Warning, TEXT("没有在场景中创建成功"));
			}
		}
		else
		{
			UE_LOG(LogGyraGame, Warning, TEXT("没有在场景中找到合适的位置"));
		}
	}
}


void UGyraInventoryManagerComponent::CallServerMoveAllItemFromBackToWarehouseInstance_Implementation()
{
	TArray<UGyraInventoryItemInstance*> BackpackItemList = BackpackList.GetAllItems();

	for (auto& TmpItem : BackpackItemList)
	{
		EGyraInventoryItemType ItemType = UGyraInventoryFunctionLibrary::FindItemTypeByInstance(TmpItem);

		if (ItemType == EGyraInventoryItemType::None)
		{
			continue;
		}

		int32 FreeIndex = WarehouseList.FindFirstFreeEntry();

		if (FreeIndex != INDEX_NONE)
		{
			SwapEntryCross(EGyraInventoryListType::Backpack,
			               TmpItem,
			               EGyraInventoryListType::Warehouse,
			               WarehouseList.FindIndexByInstance(FreeIndex));
		}
		else
		{
			break;
		}
	}
}

bool UGyraInventoryManagerComponent::ConsumeItemsByDefinition(EGyraInventoryListType InListType,
                                                              TSubclassOf<UGyraInventoryItemDefinition> ItemDef,
                                                              int32 NumToConsume)
{
	AActor* OwningActor = GetOwner();

	if (!OwningActor || !OwningActor->HasAuthority())
	{
		return false;
	}

	//@TODO: N squared right now as there's no acceleration structure
	//@TODO: N的平方，因为现在没有加速结构
	int32 TotalConsumed = 0;
	while (TotalConsumed < NumToConsume)
	{
		if (UGyraInventoryItemInstance* Instance = UGyraInventoryManagerComponent::FindFirstItemStackByDefinition(
			InListType, ItemDef))
		{
			//这里需要考虑移除的Object同步
			//QucikEquipmentList.RemoveEntry(Instance);
			RemoveItemInstance(InListType, Instance);

			++TotalConsumed;
		}
		else
		{
			return false;
		}
	}

	return TotalConsumed == NumToConsume;
}

bool UGyraInventoryManagerComponent::CheckConsumeItemStacksByDefinition(EGyraInventoryListType InListType,
                                                                        TSubclassOf<UGyraInventoryItemDefinition>
                                                                        ItemDef, int32 NumToConsume)
{
	AActor* OwningActor = GetOwner();

	if (!OwningActor || !OwningActor->HasAuthority())
	{
		return false;
	}
	TArray<UGyraInventoryItemInstance*> Results;
	FGyraInventoryList& OperatedInventoryList = GetOperatedInventroyListByEnum(InListType);

	bool bRemoved = OperatedInventoryList.RemoveEntry(ItemDef, NumToConsume, Results);

	return bRemoved;
}

bool UGyraInventoryManagerComponent::ConsumeItemStacksByDefinition(EGyraInventoryListType InListType,
                                                                   TSubclassOf<UGyraInventoryItemDefinition> ItemDef,
                                                                   int32 NumToConsume)
{
	AActor* OwningActor = GetOwner();

	if (!OwningActor || !OwningActor->HasAuthority())
	{
		return false;
	}
	TArray<UGyraInventoryItemInstance*> OutResult;
	FGyraInventoryList& OperatedInventoryList = GetOperatedInventroyListByEnum(InListType);
	bool bCheckRemoved = OperatedInventoryList.RemoveEntry(ItemDef, NumToConsume, OutResult);
	return bCheckRemoved;
}

TArray<UGyraInventoryItemInstance*> UGyraInventoryManagerComponent::GetAllItems(EGyraInventoryListType InListType)
{
	FGyraInventoryList& OperatedInventoryList = GetOperatedInventroyListByEnum(InListType);

	return OperatedInventoryList.GetAllItems();
}

UGyraInventoryItemInstance* UGyraInventoryManagerComponent::FindFirstItemStackByDefinition(
	EGyraInventoryListType InListType,
	TSubclassOf<UGyraInventoryItemDefinition> ItemDef)
{
	FGyraInventoryList& OperatedInventoryList = GetOperatedInventroyListByEnum(InListType);

	for (const FGyraInventoryEntry& Entry : OperatedInventoryList.Entries)
	{
		UGyraInventoryItemInstance* Instance = Entry.Instance;

		if (IsValid(Instance))
		{
			if (Instance->GetItemDef() == ItemDef)
			{
				return Instance;
			}
		}
	}

	return nullptr;
}

TArray<UGyraInventoryItemInstance*> UGyraInventoryManagerComponent::FindAllItemWithDefinition(
	EGyraInventoryListType InListType,
	TSubclassOf<UGyraInventoryItemDefinition> ItemDef)
{
	FGyraInventoryList& OperatedInventoryList = GetOperatedInventroyListByEnum(InListType);

	TArray<UGyraInventoryItemInstance*> OutAllItem;

	for (const FGyraInventoryEntry& Entry : OperatedInventoryList.Entries)
	{
		UGyraInventoryItemInstance* Instance = Entry.Instance;

		if (IsValid(Instance))
		{
			if (Instance->GetItemDef() == ItemDef)
			{
				OutAllItem.Add(Instance);
			}
		}
	}

	return OutAllItem;
}

int32 UGyraInventoryManagerComponent::FindItemIndexByInstance(EGyraInventoryListType InListType,
                                                              UGyraInventoryItemInstance* InInstance)
{
	FGyraInventoryList& OperatedInventoryList = GetOperatedInventroyListByEnum(InListType);

	return OperatedInventoryList.FindIndexByInstance(InInstance);
}

int32 UGyraInventoryManagerComponent::GetItemCountByDefinition(
	EGyraInventoryListType InListType,
	TSubclassOf<UGyraInventoryItemDefinition> ItemDef)
{
	FGyraInventoryList& OperatedInventoryList = GetOperatedInventroyListByEnum(InListType);

	int32 TotalCount = 0;

	for (const FGyraInventoryEntry& Entry : OperatedInventoryList.Entries)
	{
		UGyraInventoryItemInstance* Instance = Entry.Instance;

		if (IsValid(Instance))
		{
			if (Instance->GetItemDef() == ItemDef)
			{
				++TotalCount;
			}
		}
	}

	return TotalCount;
}

int32 UGyraInventoryManagerComponent::GetItemStackCountByDefinition(
	EGyraInventoryListType InListType,
	TSubclassOf<UGyraInventoryItemDefinition> ItemDef)
{
	FGyraInventoryList& OperatedInventoryList = GetOperatedInventroyListByEnum(InListType);

	int32 TotalCount = 0;

	for (const FGyraInventoryEntry& Entry : OperatedInventoryList.Entries)
	{
		UGyraInventoryItemInstance* Instance = Entry.Instance;

		if (IsValid(Instance))
		{
			if (Instance->GetItemDef() == ItemDef)
			{
				TotalCount += Entry.StackCount;
			}
		}
	}

	return TotalCount;
}

int32 UGyraInventoryManagerComponent::GetItemCountByInstance(
	EGyraInventoryListType InListType,
	UGyraInventoryItemInstance* ItemInstance)
{
	FGyraInventoryList& OperatedInventoryList = GetOperatedInventroyListByEnum(InListType);

	return OperatedInventoryList.GetItemCountByInstance(ItemInstance);
}


void UGyraInventoryManagerComponent::SwapEntryInner_Implementation(
	EGyraInventoryListType InListType,
	UGyraInventoryItemInstance* InInstanceA, UGyraInventoryItemInstance* InInstanceB)
{
	bool bRightA = GetItemInstanceInList(InInstanceA) == InListType;
	bool bRightB = GetItemInstanceInList(InInstanceB) == InListType;

	if (bRightA && bRightB)
	{
		FGyraInventoryList& OperatedInventoryList = GetOperatedInventroyListByEnum(InListType);
		OperatedInventoryList.SwapEntry(InInstanceA, InInstanceB);
	}
}

#pragma endregion ListInnerOperation

//列表 跨表操作
#pragma region ListCrossOperation

EGyraInventoryListType UGyraInventoryManagerComponent::GetItemInstanceInList(UGyraInventoryItemInstance* InItemInstance)
{
	int32 Index = QuickEquipmentList.FindIndexByInstance(InItemInstance);
	if (Index != INDEX_NONE)
	{
		return EGyraInventoryListType::QuickEquipment;
	}


	Index = PassiveEquipmentList.FindIndexByInstance(InItemInstance);
	if (Index != INDEX_NONE)
	{
		return EGyraInventoryListType::PassiveEquipment;
	}


	Index = BackpackList.FindIndexByInstance(InItemInstance);
	if (Index != INDEX_NONE)
	{
		return EGyraInventoryListType::Backpack;
	}


	Index = WarehouseList.FindIndexByInstance(InItemInstance);
	if (Index != INDEX_NONE)
	{
		return EGyraInventoryListType::Warehouse;
	}

	check(false);

	return EGyraInventoryListType();
}

void UGyraInventoryManagerComponent::ReplaceItemByInstance(EGyraInventoryListType InListType,
                                                           UGyraInventoryItemInstance* InOldInstance,
                                                           UGyraInventoryItemInstance* InNewInstance,
                                                           int32 InNewStatCount)
{
	//这里不检测一致性 因为在替换的时候更改Type
	//bool  bRightA = GetItemInstanceInList(InOldInstance) == InListType;
	//if (bRightA)
	{
		FGyraInventoryList& OperatedInventoryList = GetOperatedInventroyListByEnum(InListType);

		OperatedInventoryList.ReplaceEntry(InOldInstance, InNewInstance, InNewStatCount);
	}
}

void UGyraInventoryManagerComponent::SwapEntryCross_Implementation(
	EGyraInventoryListType InListTypeA,
	UGyraInventoryItemInstance* InInstanceA,
	EGyraInventoryListType InListTypeB,
	UGyraInventoryItemInstance* InInstanceB)
{
	bool bRightA = GetItemInstanceInList(InInstanceA) == InListTypeA;
	bool bRightB = GetItemInstanceInList(InInstanceB) == InListTypeB;

	//这里如需要判断数据一致性 不一致则丢弃本操作
	if (bRightA && bRightB)
	{
		if (InListTypeA == InListTypeB)
		{
			SwapEntryInner(InListTypeA, InInstanceA, InInstanceB);
		}
		else
		{
			int32 StatckCountA = GetItemCountByInstance(InListTypeA, InInstanceA);
			int32 StatckCountB = GetItemCountByInstance(InListTypeB, InInstanceB);

			EGyraInventoryListType ReplaceListType1 = InListTypeA;

			ReplaceItemByInstance(ReplaceListType1, InInstanceA, InInstanceB, StatckCountB);

			EGyraInventoryListType ReplaceListType2 = InListTypeB;

			ReplaceItemByInstance(ReplaceListType2, InInstanceB, InInstanceA, StatckCountA);
		}
	}
}

#pragma endregion ListCrossOperation


void UGyraInventoryManagerComponent::BeginPlay()
{
	Super::BeginPlay();


	// Listen for the experience load to complete
	AGameStateBase* GameState = GetWorld()->GetGameStateChecked<AGameStateBase>();
	UGyraExperienceManagerComponent* ExperienceComponent = GameState->FindComponentByClass<
		UGyraExperienceManagerComponent>();

	check(ExperienceComponent);
	ExperienceComponent->CallOrRegister_OnExperienceLoaded_LowPriority(
		FOnGyraExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded));
}

void UGyraInventoryManagerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// 不能在这里调用自动存档,因为没办法判定玩家控制器

	Super::EndPlay(EndPlayReason);
}


#define REPLICATE_SUBOBJECTS_LIST(ListName) \
for (FGyraInventoryEntry& Entry : ListName.Entries) \
{ \
	UGyraInventoryItemInstance* Instance = Entry.Instance; \
	if (Instance && IsValid(Instance)) \
	{ \
		WroteSomething |= Channel->ReplicateSubobject(Instance, *Bunch, *RepFlags); \
	} \
}

bool UGyraInventoryManagerComponent::ReplicateSubobjects(UActorChannel* Channel, class FOutBunch* Bunch,
                                                         FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	REPLICATE_SUBOBJECTS_LIST(QuickEquipmentList);
	REPLICATE_SUBOBJECTS_LIST(PassiveEquipmentList);
	REPLICATE_SUBOBJECTS_LIST(BackpackList);
	REPLICATE_SUBOBJECTS_LIST(WarehouseList);

	return WroteSomething;
}

#define REPLICATE_READYFORREPLICATION_LIST(ListName) \
for (const FGyraInventoryEntry& Entry : ListName.Entries)\
{\
	UGyraInventoryItemInstance* Instance = Entry.Instance;\
	if (IsValid(Instance))\
	{\
		AddReplicatedSubObject(Instance);\
	}\
}

void UGyraInventoryManagerComponent::ReadyForReplication()
{
	Super::ReadyForReplication();

	// Register existing UGyraInventoryItemInstance
	// 注册现有的UGyraInventoryItemInstance
	if (IsUsingRegisteredSubObjectList())
	{
		REPLICATE_READYFORREPLICATION_LIST(QuickEquipmentList);
		REPLICATE_READYFORREPLICATION_LIST(PassiveEquipmentList);
		REPLICATE_READYFORREPLICATION_LIST(BackpackList);
		REPLICATE_READYFORREPLICATION_LIST(WarehouseList);
	}
}

void UGyraInventoryManagerComponent::NotifyAddReplicatedInventoryItemInstance(
	UGyraInventoryItemInstance* InItemInstance)
{
	if (IsUsingRegisteredSubObjectList() && IsReadyForReplication() && InItemInstance)
	{
		AddReplicatedSubObject(InItemInstance);
	}
}

void UGyraInventoryManagerComponent::NotifyRemoveReplicatedInventoryItemInstance(
	UGyraInventoryItemInstance* InItemInstance)
{
	if (InItemInstance && IsUsingRegisteredSubObjectList())
	{
		RemoveReplicatedSubObject(InItemInstance);
	}
}

void UGyraInventoryManagerComponent::OnUnregister()
{
	AutoWriteArchive();
	Super::OnUnregister();
}


void UGyraInventoryManagerComponent::OnExperienceLoaded(const UGyraExperienceDefinition* Experience)
{
	TmpExperience = const_cast<UGyraExperienceDefinition*>(Experience);

	//#if WITH_SERVER_CODE
	//
	//	//在服务器上进行背包的初始化操作
	//	AActor* OwningActor = GetOwner();
	//
	//	if (OwningActor && OwningActor->HasAuthority())
	//	{
	//		//这里的数量需要后期暴露为配置
	//		InitInventoryArchiveList(EGyraInventoryListType::QuickEquipment, 3);
	//		InitInventoryArchiveList(EGyraInventoryListType::Backpack, 20);
	//		InitInventoryArchiveList(EGyraInventoryListType::Warehouse, 40);
	//	}
	//
	//#endif

	// 当体验加载完毕后进行读取上传存档
	ReadArchive();
}


void UGyraInventoryManagerComponent::ReadArchive()
{
	if (GetWorld()->GetNetMode() != ENetMode::NM_DedicatedServer)
	{
		// 必须是本机玩家
		if (GetWorld()->GetFirstPlayerController() == Cast<APlayerController>(GetOwner()))
		{
			FGyraArchiveListInfomation QuickEquipmentListInfomation = GetArchiveListInfomationByEnum(
				EGyraInventoryListType::QuickEquipment);

			FGyraArchiveListInfomation PassiveEquipmentListInfomation = GetArchiveListInfomationByEnum(
				EGyraInventoryListType::PassiveEquipment);

			FGyraArchiveListInfomation BackpackListInfomation = GetArchiveListInfomationByEnum(
				EGyraInventoryListType::Backpack);

			FGyraArchiveListInfomation WarehouseListInfomation = GetArchiveListInfomationByEnum(
				EGyraInventoryListType::Warehouse);
			// 我们已经从本地读取了存档
			bReadArchive = true;
			// 需要知道是否收到了服务器下发的存档
			RepArchiveMap.Add(EGyraInventoryListType::QuickEquipment, false);
			RepArchiveMap.Add(EGyraInventoryListType::PassiveEquipment, false);
			RepArchiveMap.Add(EGyraInventoryListType::Backpack, false);
			RepArchiveMap.Add(EGyraInventoryListType::Warehouse, false);

			// 将存档上传到服务器上 必须通过网络初始化
			NetInitInventoryArchiveList(EGyraInventoryListType::QuickEquipment, QuickEquipmentListInfomation, 3);
			NetInitInventoryArchiveList(EGyraInventoryListType::PassiveEquipment, PassiveEquipmentListInfomation, 6);
			NetInitInventoryArchiveList(EGyraInventoryListType::Backpack, BackpackListInfomation, 20);
			NetInitInventoryArchiveList(EGyraInventoryListType::Warehouse, WarehouseListInfomation, 40);
		}
	}
}

void UGyraInventoryManagerComponent::WriteArchive()
{
	if (GetWorld()->GetNetMode() != ENetMode::NM_DedicatedServer)
	{
		// 这里是直接写入存档了
		// 必须是本机玩家

		check(GetOwner())
		APlayerController* Player = Cast<APlayerController>(GetOwner());


		if (Player->IsLocalController())
		{
			UGyraArchiveSubsystem* ArchiveSubsystem = UGyraArchiveSubsystem::GetArchiveSubsystem();

			ArchiveSubsystem->ActiveSlotInformation->QuickEquipmentList = QuickEquipmentList.ExportArchiveList();
			ArchiveSubsystem->ActiveSlotInformation->PassiveEquipmentList = PassiveEquipmentList.ExportArchiveList();
			ArchiveSubsystem->ActiveSlotInformation->BackpackList = BackpackList.ExportArchiveList();
			ArchiveSubsystem->PanelInformation->WarehouseList = WarehouseList.ExportArchiveList();

			ArchiveSubsystem->AutoSaveArchiveSlot();
		}
	}
}

void UGyraInventoryManagerComponent::AutoWriteArchive()
{
	// 上传了存档,但是存档没有下发
	if (bReadArchive)
	{
		for (auto Tmp : RepArchiveMap)
		{
			if (Tmp.Value == false)
			{
				// 存档没有收到下放,无法反写本地存档
				return;
			}
		}
	}

	WriteArchive();
}


//////////////////////////////////////////////////////////////////////
//XGTODO:过滤器 背包整理的时候加入!

// UCLASS(Abstract)
// class UGyraInventoryFilter : public UObject
// {
// public:
// 	virtual bool PassesFilter(UGyraInventoryItemInstance* Instance) const { return true; }
// };

// UCLASS()
// class UGyraInventoryFilter_HasTag : public UGyraInventoryFilter
// {
// public:
// 	virtual bool PassesFilter(UGyraInventoryItemInstance* Instance) const { return true; }
// };
