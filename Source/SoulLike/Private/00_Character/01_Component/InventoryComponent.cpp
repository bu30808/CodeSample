// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/01_Component/InventoryComponent.h"

#include "ComponentReregisterContext.h"
#include "00_Character/01_Component/AbilityComponent.h"
#include "02_Ability/AbilityBase.h"
#include "03_Widget/01_Menu/00_Inventory/ItemButtonWidget.h"
#include "04_Item/ItemActor.h"
#include "93_SaveGame/SoulLikeSaveGame.h"
#include "96_Library/AbilityHelperLibrary.h"
#include "96_Library/DataLayerHelperLibrary.h"
#include "96_Library/ItemHelperLibrary.h"
#include "98_GameInstance/SoulLikeInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Logging/StructuredLog.h"

DEFINE_LOG_CATEGORY(LogInventory)

AItemActor* FInventoryItem::SetItemActor(bool bLoad)
{
	if (SpawndItemActor == nullptr)
	{
		if (auto newActor =
			Cast<AItemActor>(
				UGameplayStatics::BeginDeferredActorSpawnFromClass(OwnerActor.Get(), ItemActorObjectClass,
				                                                   FTransform::Identity,
				                                                   ESpawnActorCollisionHandlingMethod::AlwaysSpawn,
				                                                   OwnerActor.Get())))
		{
			if (bLoad)
			{
				UE_LOGFMT(LogSave, Log, "게임 로드로 생성된 아이템 액터입니다 : {0}", GetNameSafe(newActor));
			}
			else
			{
				UE_LOGFMT(LogSave, Warning, "게임 로드로 생성된 아이템 액터가 아닙니다. : {0}", GetNameSafe(newActor));
			}

			/*newActor->AddDataLayer(UDataLayerHelperLibrary::GetAlwaysActivatedDataLayerAsset(newActor));*/
			newActor->SetActorEnableCollision(false);
			newActor->SetActorHiddenInGame(true);
			newActor->SpawnPreSetting();
			SpawndItemActor = Cast<AItemActor>(UGameplayStatics::FinishSpawningActor(newActor, FTransform::Identity));
		}
	}

	return SpawndItemActor;
}

FInventoryItem::FInventoryItem(class AActor* Owner, AItemActor* ItemActor, int32 Count, FGuid NewID)
{
	ensure(Owner);
	ensure(ItemActor);

	OwnerActor = Owner;
	ItemActorObjectClass = ItemActor->GetClass();
	/*SpawndItemActor = ItemActor;*/
	UniqueID = NewID;

	SetItemActor(false);

	ItemCount = Count;
}

FInventoryItem::FInventoryItem(AActor* Owner, TSubclassOf<AItemActor> ItemActorObject, int32 Count, FGuid NewID,
                               bool bLoad)
{
	ensure(Owner);
	ensure(ItemActorObject);

	OwnerActor = Owner;
	ItemActorObjectClass = ItemActorObject;
	UniqueID = NewID;

	SetItemActor(bLoad);

	ItemCount = Count;
}

const FItemInformation* FInventoryItem::GetItemInformation() const
{
	
	if (SpawndItemActor != nullptr)
	{
		return SpawndItemActor->GetItemInformation();
	}

	UE_LOGFMT(LogTemp, Error, "아이템 액터가 유효하지 않아 아이템 정보를 가져올 수 없습니다");

	return nullptr;
}

FText FInventoryItem::GetFormattedDescription() const
{
	//UE_LOGFMT(LogTemp,Log,"아이템 설명을 가져오려 시도합니다.");
	if (SpawndItemActor != nullptr)
	{
		//UE_LOGFMT(LogActorComponent,Log,"설명을 가져오는 클래스 타입 : {0}",SpawndItemActor->GetClass()->GetName());
		return SpawndItemActor->GetFormattedDescription();
	}

	return FText::FromString("NOT VALID");
}

bool FInventoryItem::Use(AActor* Owner) const
{
	if (SpawndItemActor != nullptr)
	{
		return SpawndItemActor->UseItem(Owner, UniqueID);
	}

	return false;
}

void FInventoryItem::UnEquip(AActor* Owner) const
{
	if (SpawndItemActor != nullptr)
	{
		if (GetItemInformation()->Item_Type == EItemType::EQUIPMENT)
		{
			SpawndItemActor->UnEquip(Owner, UniqueID);
		}
	}
}

bool FInventoryItem::CanStack(int32 AddCount) const
{
	auto maxStack = GetItemInformation()->MaxStack;
	if (maxStack == -1)
	{
		return true;
	}

	return maxStack >= ItemCount + AddCount;
}


// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;


	//OrbFragmentHandler = CreateDefaultSubobject<UOrbFragmentHandler>(TEXT("FragmentHandler"));

	// ...
}

// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();


	// ...
	if (auto instance = Cast<USoulLikeInstance>(UGameplayStatics::GetGameInstance(this)))
	{
		OnSaveFieldItemState.AddUniqueDynamic(instance, &USoulLikeInstance::OnSaveLevelItemPlacementStateEvent);
		OnAddItem.AddUniqueDynamic(instance, &USoulLikeInstance::OnAddItemEvent);
		OnRemoveItem.AddUniqueDynamic(instance, &USoulLikeInstance::OnRemoveItemEvent);
		OnUseItem.AddUniqueDynamic(instance, &USoulLikeInstance::OnUseItemEvent);
		OnUnEquipItem.AddUniqueDynamic(instance, &USoulLikeInstance::OnUnEquipItemEvent);
		OnUpdateMainAbilityQuickSlot.AddUniqueDynamic(instance, &USoulLikeInstance::OnUpdateMainAbilityQuickSlotEvent);
		OnUpdateMainConsumeQuickSlot.AddUniqueDynamic(instance, &USoulLikeInstance::OnUpdateMainConsumeQuickSlotEvent);
	}
}

void UInventoryComponent::PostInitProperties()
{
	Super::PostInitProperties();
}

void UInventoryComponent::AddFragment(FGuid Guid, const FInventoryItem& NewFragment, const FItemInformation* ItemInfo)
{
	UE_LOGFMT(LogInventory, Log, "AddFragment : 다음 인벤토리 파편을 추가합니다 : {0}", ItemInfo->Item_Name.ToString());
	Fragments.Add(Guid, NewFragment);
}

bool UInventoryComponent::IsFragmentContains(const FGuid& UniqueID) const
{
	return Fragments.Contains(UniqueID);
}

bool UInventoryComponent::IsItemContains(const FGuid& UniqueID) const
{
	return Inventory.Contains(UniqueID);
}


const FInventoryItem& UInventoryComponent::GetEquippedWeapon()
{
	return Inventory[EquippedWeapon];
}

const FInventoryItem& UInventoryComponent::GetEquippedCore()
{
	return Inventory[EquippedCore];
}


FGuid UInventoryComponent::AddItem(AItemActor* ItemActor, bool bShowPickUpWidget)
{
	if (ItemActor->IsValidLowLevel())
	{
		ItemActor->SetActorEnableCollision(false);
		ItemActor->SetActorHiddenInGame(true);

		if (const auto info = ItemActor->GetItemInformation())
		{
			FGuid guid;
			const int32& itemCount = ItemActor->GetItemCount();

			switch (info->Item_Type)
			{
			case EItemType::NONE:
				break;
			case EItemType::EQUIPMENT:
				guid = AddNewItemToInventory(info, ItemActor, itemCount);
				break;
			case EItemType::ABILITY:
				break;
			case EItemType::CONSUME:
			case EItemType::ENHANCEMENT:
			case EItemType::KEY:
			case EItemType::ETC:
				{
					if (info->bStackable)
					{
						const auto existItem = HasItem(info->Item_Tag);
						if (existItem && existItem->CanStack(itemCount))
						{
							existItem->ItemCount += itemCount;
							guid = existItem->UniqueID;
						}
						else
						{
							//TODO 보관함을 구현하려면 이 부분을 보관함으로 아이템을 전송하는 코드로 바꾸면 됩니다.
							guid = AddNewItemToInventory(info, ItemActor, itemCount);
						}
					}
				}
				break;
			}

			if (Inventory.Contains(guid))
			{
				OnAddItem.Broadcast(GetOwner<ABaseCharacter>(), Inventory[guid], ItemActor);
				if (bShowPickUpWidget)
				{
					OnGetItem.Broadcast(info->Item_Image, info->Item_Name, itemCount);
				}

				/*UE_LOGFMT(LogActorComponent, Log, "아이템 추가 : {0} {1}", guid.ToString(),
				          Inventory[guid].GetItemInformation()->Item_Tag.ToString());*/
				OnInventoryWidgetUpdate.Broadcast(guid, Inventory[guid].ItemCount);
				OnItemQuickSlotUpdate.Broadcast(guid, Inventory[guid].ItemCount);
			}
			else
			{
				auto& frag = GetFragment(guid);
				OnAddItem.Broadcast(GetOwner<ABaseCharacter>(), frag, ItemActor);
				if (bShowPickUpWidget)
				{
					OnGetItem.Broadcast(frag.GetItemInformation()->Item_Image, frag.GetItemInformation()->Item_Name,
					                    itemCount);
				}
			}
			OnSaveFieldItemState.Broadcast(GetOwner<APlayerCharacter>(), ItemActor);
			ItemActor->Destroy();

			return guid;
		}
		UE_LOGFMT(LogTemp, Error, "가져온 아이템 정보가 유효하지 않습니다.");
	}

	return FGuid();
}

FGuid UInventoryComponent::SpawnAndAddItem(TSubclassOf<AItemActor> ItemToSpawn, const int32& ItemCount)
{
	if (ItemToSpawn)
	{
		FActorSpawnParameters spawnParam;
		spawnParam.Owner = GetOwner();
		spawnParam.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		if (auto item = GetWorld()->SpawnActor<AItemActor>(ItemToSpawn, FTransform::Identity, spawnParam))
		{
			/*item->AddDataLayer(UDataLayerHelperLibrary::GetAlwaysActivatedDataLayerAsset(this));*/
			item->SetItemCount(ItemCount);
			return AddItem(item, false);
		}
	}

	return FGuid();
}

void UInventoryComponent::UseItem(FGuid ItemUniqueID)
{
	//빙결상태인 경우에는 아이템을 사용할 수 없도록 합니다.
	if (UAbilityHelperLibrary::IsStatusEffectActive(GetOwner<ABaseCharacter>(), EStatusEffect::FREEZE))
	{
		return;
	}

	if (Inventory.Contains(ItemUniqueID))
	{
		if (Inventory[ItemUniqueID].ItemCount > 0)
		{
			UE_LOGFMT(LogActorComponent, Log, "다음 아이템을 사용 시도 합니다 : {0} ,{1}",
			          Inventory[ItemUniqueID].GetItemInformation()->Item_Name.ToString(), ItemUniqueID);
			const auto& item = Inventory[ItemUniqueID];
			bool bSccUse = item.Use(GetOwner());

			if (bSccUse)
			{
				//사용 후 인벤토리에 남아있나 확인합니다.
				if (Inventory.Contains(ItemUniqueID))
				{
					if (item.GetItemInformation()->Item_Type == EItemType::EQUIPMENT)
					{
						//장비 아이템을 장착한 경우 따로 저장해 놓습니다.
						EquippedEquipments.AddUnique(ItemUniqueID);

						//무기인 경우 따로 저장합시다.
						if (UItemHelperLibrary::IsWeapon(Inventory[ItemUniqueID]))
						{
							EquippedWeapon = ItemUniqueID;
						}
					}
					//아이템 사용 후 해야할 행동을 합니다.

					OnUseItem.Broadcast(GetOwner<ABaseCharacter>(), item);
					OnItemQuickSlotUpdate.Broadcast(ItemUniqueID, item.ItemCount);
				}
				else
				{
					OnItemQuickSlotUpdate.Broadcast(ItemUniqueID, 0);
				}
			}
		}
		return;
	}

	if (IsFragmentContains(ItemUniqueID))
	{
		if (const auto frag = GetFragment(ItemUniqueID); frag.ItemCount > 0)
		{
			UE_LOGFMT(LogActorComponent, Log, "다음 아이템을 사용 시도 합니다 : {0} ,{1}",
			          frag.GetItemInformation()->Item_Name.ToString(),
			          ItemUniqueID);
			frag.Use(GetOwner());

			if (frag.GetItemInformation()->Item_Type == EItemType::EQUIPMENT)
			{
				//장비 아이템을 장착한 경우 따로 저장해 놓습니다.
				EquippedEquipments.AddUnique(ItemUniqueID);
			}

			//아이템 사용 후 해야할 행동을 합니다.
			OnUseItem.Broadcast(GetOwner<ABaseCharacter>(), frag);
		}
	}
}

void UInventoryComponent::RemoveItem(FGuid ItemUniqueID)
{
	if (Inventory.Contains(ItemUniqueID))
	{
		if (Inventory[ItemUniqueID].GetItemInformation()->bRemovable)
		{
			Inventory.Remove(ItemUniqueID);
			OnRemoveItem.Broadcast(GetOwner<ABaseCharacter>(), ItemUniqueID);
		}
	}
}

bool UInventoryComponent::DecreaseItemCount(FGuid ItemUniqueID, int32 Count)
{
	if (Inventory.Contains(ItemUniqueID))
	{
		if (Inventory[ItemUniqueID].ItemCount < Count)
		{
			UE_LOGFMT(LogActorComponent, Error, "아이템 수량보다 제거하려는 갯수가 더 많습니다");
			return false;
		}

		UE_LOGFMT(LogActorComponent, Log, "{0}의 기존 수량 : {1}",
		          Inventory[ItemUniqueID].GetItemInformation()->Item_Tag.ToString(), Inventory[ItemUniqueID].ItemCount);
		UE_LOGFMT(LogActorComponent, Log, "제거할 수량 : {0}", Count);
		Inventory[ItemUniqueID].ItemCount = FMath::Clamp(Inventory[ItemUniqueID].ItemCount - Count, 0,INT_MAX);

		const auto& itemTag = Inventory[ItemUniqueID].GetItemInformation()->Item_Tag;


		UE_LOGFMT(LogActorComponent, Log, "{0}의 남은 수량 : {1}",
		          Inventory[ItemUniqueID].GetItemInformation()->Item_Tag.ToString(), Inventory[ItemUniqueID].ItemCount);


		const auto& updatedCount = Inventory[ItemUniqueID].ItemCount;
		if (updatedCount <= 0)
		{
			if (Inventory[ItemUniqueID].GetItemInformation()->bRemoveWhenCountZero)
			{
				UE_LOGFMT(LogActorComponent, Log, "아이템 갯수가 0 이하이고, 지워도 된다고 마크되어 있으니 인벤토리에서 제거합니다 : {0}",
				          Inventory[ItemUniqueID].GetItemInformation()->Item_Name.ToString());

				Inventory.Remove(ItemUniqueID);
				OnRemoveItem.Broadcast(GetOwner<ABaseCharacter>(), ItemUniqueID);
				//OnItemQuickSlotUpdate.Broadcast(ItemUniqueID, updatedCount);
				return true;
			}
		}
		//OnItemQuickSlotUpdate.Broadcast(ItemUniqueID, updatedCount);
		OnInventoryWidgetUpdate.Broadcast(ItemUniqueID, updatedCount);

		return true;
	}

	return false;
}

void UInventoryComponent::UnEquip(const FGuid& ItemUniqueID)
{
	if (Inventory.Contains(ItemUniqueID))
	{
		UE_LOGFMT(LogTemp, Log, "다음 아이템을 장착 해제 시도합니다 : {0}",
		          Inventory[ItemUniqueID].GetItemInformation()->Item_Name.ToString());
		Inventory[ItemUniqueID].UnEquip(GetOwner());
		if (Inventory[ItemUniqueID].GetItemInformation()->Item_Type == EItemType::EQUIPMENT)
		{
			EquippedEquipments.Remove(ItemUniqueID);

			//무기인 경우 저장된 값을 날립니다.
			if (UItemHelperLibrary::IsWeapon(Inventory[ItemUniqueID]))
			{
				EquippedWeapon = FGuid();
			}
		}

		OnUnEquipItem.Broadcast(GetOwner<ABaseCharacter>(), Inventory[ItemUniqueID]);
		return;
	}

	if (IsFragmentContains(ItemUniqueID))
	{
		const auto& frag = GetFragment(ItemUniqueID);
		frag.UnEquip(GetOwner());
		if (frag.GetItemInformation()->Item_Type == EItemType::EQUIPMENT)
		{
			EquippedEquipments.Remove(ItemUniqueID);
		}

		OnUnEquipItem.Broadcast(GetOwner<ABaseCharacter>(), frag);
	}
}

const FInventoryItem& UInventoryComponent::GetInventoryItem(const FGuid& ItemUniqueID)
{
	//일단 인벤토리에 있는가 확인합니다.
	if (Inventory.Contains(ItemUniqueID))
	{
		return Inventory[ItemUniqueID];
	}

	//없다면 파편에서 가져옵니다. 이 부분에서 실패하면 게임이 터지니 주의하세요.
	return Fragments[ItemUniqueID];
}

TArray<AItemActor*> UInventoryComponent::GetEquipmentItems()
{
	TArray<AItemActor*> items;
	for (auto code : EquippedEquipments)
	{
		if (Inventory.Contains(code))
		{
			items.Add(Inventory[code].GetSpawndItemActor());
		}
	}

	return items;
}

bool UInventoryComponent::IsEquippedEquipment(const FGuid& ItemUniqueID)
{
	return EquippedEquipments.Contains(ItemUniqueID);
}

bool UInventoryComponent::K2_HasItem(const FGameplayTag& ItemTag)
{
	for (auto& iter : Inventory)
	{
		if (iter.Value.GetItemInformation()->Item_Tag.MatchesTagExact(ItemTag))
		{
			return true;
		}
	}

	return false;
}

bool UInventoryComponent::K2_HasItemByID(const FGuid& UniqueID) const
{
	return Inventory.Contains(UniqueID) || Fragments.Contains(UniqueID);
}

const FInventoryItem* UInventoryComponent::GetItemByTag(const FGameplayTag& ItemTag)
{
	for (auto& iter : Inventory)
	{
		if (iter.Value.GetItemInformation()->Item_Tag.MatchesTagExact(ItemTag))
		{
			return &iter.Value;
		}
	}

	return nullptr;
}

AItemActor* UInventoryComponent::GetItemActorByTag(const FGameplayTag& ItemTag)
{
	return GetItemByTag(ItemTag)->GetSpawndItemActor();
}

FInventoryItem* UInventoryComponent::HasItem(FGameplayTag ItemTag)
{
	//이미 있는가 확인함
	for (auto& iter : Inventory)
	{
		UE_LOGFMT(LogInventory,Log,"HasItem : {0}",iter.Value.GetItemInformation()->Item_Tag.ToString());
		//이미 있는 경우
		if (iter.Value.GetItemInformation()->Item_Tag.MatchesTagExact(ItemTag))
		{
			return &iter.Value;
		}
	}

	return nullptr;
}

const FGuid UInventoryComponent::AddNewItemToInventory(const FItemInformation* ItemInfo, AItemActor* ItemActor,
                                                       int32 ItemCount)
{
	const FGuid& newID = FGuid::NewGuid();
	const FInventoryItem& NewItem = FInventoryItem(GetOwner(), ItemActor, ItemCount, newID);

	if (ItemInfo->Item_Type == EItemType::EQUIPMENT)
	{
		if (static_cast<const FEquipmentInformation*>(ItemInfo)->EquipType == EEquipType::ORB)
		{
			if (static_cast<const FOrbInformation*>(ItemInfo)->OrbType == EOrbType::FRAGMENT)
			{
				UE_LOGFMT(LogInventory, Log, "새 파편 추가 : {0}:{1} {2}개",
				          NewItem.GetItemInformation()->Item_Name.ToString(), newID, NewItem.ItemCount);
				AddFragment(newID, NewItem, ItemInfo);
				return newID;
			}
		}
	}

	UE_LOGFMT(LogInventory, Log, "새 아이템 추가 : {0}:{1} {2}개", NewItem.GetItemInformation()->Item_Name.ToString(), newID,
	          NewItem.ItemCount);
	Inventory.Add(newID, NewItem);
	return newID;
}

void UInventoryComponent::AddQuickSlotItem(UInventoryData* Data, int32 Index)
{
	if (Data != nullptr)
	{
		if (Data->IsA<UItemData>())
		{
			ConsumeQuickSlotUniqueIDs[Index] = Cast<UItemData>(Data)->InventoryItem.UniqueID;
			CurConsumeQuickSlotIndex = Index;
			OnUpdateMainConsumeQuickSlot.Broadcast(GetInventoryItem(ConsumeQuickSlotUniqueIDs[Index]), false, Index);
		}
	}
	else
	{
		UKismetSystemLibrary::PrintString(this,TEXT("퀵슬롯 데이터가 유효하지 않아 처리할 수 없습니다."));
	}
}

void UInventoryComponent::AddQuickSlotAbility(UInventoryData* Data, int32 Index)
{
	if (Data->IsA<UAbilityData>())
	{
		AbilityQuickSlotTags[Index] = Cast<UAbilityData>(Data)->AbilityInformation.AbilityTag;
		CurAbilityQuickSlotIndex = Index;
		OnUpdateMainAbilityQuickSlot.Broadcast(Cast<UAbilityData>(Data)->AbilityInformation.AbilityTag, false, Index);
	}
}

void UInventoryComponent::RemoveQuickSlotItem(UInventoryData* Data, int32 Index)
{
	if (Data->IsA<UItemData>())
	{
		if (Index == CurConsumeQuickSlotIndex)
		{
			if (K2_HasItemByID(ConsumeQuickSlotUniqueIDs[Index]))
			{
				OnUpdateMainConsumeQuickSlot.Broadcast(GetInventoryItem(ConsumeQuickSlotUniqueIDs[Index]), true, Index);
			}
			else
			{
				UKismetSystemLibrary::PrintString(
					this,TEXT("이 아이템 정보를 찾을 수 없습니다 : ") + ConsumeQuickSlotUniqueIDs[Index].ToString());
			}
		}

		if (ConsumeQuickSlotUniqueIDs.IsValidIndex(Index))
		{
			ConsumeQuickSlotUniqueIDs[Index] = FGuid();
		}
	}
}

void UInventoryComponent::RemoveQuickSlotAbility(UInventoryData* Data, int32 Index)
{
	if (Data->IsA<UAbilityData>())
	{
		if (Index == CurAbilityQuickSlotIndex)
		{
			OnUpdateMainAbilityQuickSlot.
				Broadcast(Cast<UAbilityData>(Data)->AbilityInformation.AbilityTag, true, Index);
		}
		AbilityQuickSlotTags[Index] = FGameplayTag::EmptyTag;
	}
}

void UInventoryComponent::NextConsumeQuickSlot()
{
	//UKismetSystemLibrary::PrintString(this,TEXT("NextConsumeQuickSlot"));
	UE_LOGFMT(LogInventory, Log, "다음 퀵슬롯으로 전환시도");


	//빈 슬롯이 아닐 때까지 인덱스를 늘립니다.
	for (auto i = CurConsumeQuickSlotIndex + 1; i < 10; i++)
	{
		if (ConsumeQuickSlotUniqueIDs[i] != FGuid())
		{
			CurConsumeQuickSlotIndex = i;
			if (K2_HasItemByID(ConsumeQuickSlotUniqueIDs[CurConsumeQuickSlotIndex]))
			{
				const auto& item = GetInventoryItem(ConsumeQuickSlotUniqueIDs[CurConsumeQuickSlotIndex]);
				UE_LOGFMT(LogInventory, Log, "다음 퀵슬롯 타겟 발견 : {0}", item.GetItemInformation()->Item_Name.ToString());
				OnUpdateMainConsumeQuickSlot.Broadcast(item, false, CurConsumeQuickSlotIndex);
			}
			return;
		}
	}

	for (auto i = 0; i < CurConsumeQuickSlotIndex; i++)
	{
		if (ConsumeQuickSlotUniqueIDs[i] != FGuid())
		{
			CurConsumeQuickSlotIndex = i;
			if (K2_HasItemByID(ConsumeQuickSlotUniqueIDs[CurConsumeQuickSlotIndex]))
			{
				const auto& item = GetInventoryItem(ConsumeQuickSlotUniqueIDs[CurConsumeQuickSlotIndex]);
				UE_LOGFMT(LogInventory, Log, "다음 퀵슬롯 타겟 발견 : {0}", item.GetItemInformation()->Item_Name.ToString());
				OnUpdateMainConsumeQuickSlot.Broadcast(item, false, CurConsumeQuickSlotIndex);
			}
			return;
		}
	}
}

void UInventoryComponent::NextAbilityQuickSlot()
{
	//빈 슬롯이 아닐 때까지 인덱스를 늘립니다.
	for (auto i = CurAbilityQuickSlotIndex + 1; i < 10; i++)
	{
		if (AbilityQuickSlotTags[i].IsValid())
		{
			UKismetSystemLibrary::PrintString(this,TEXT("발견 : ") + FString::FormatAsNumber(i));
			CurAbilityQuickSlotIndex = i;
			OnUpdateMainAbilityQuickSlot.Broadcast(
				AbilityQuickSlotTags[CurAbilityQuickSlotIndex], false, CurAbilityQuickSlotIndex);
			return;
		}
	}

	for (auto i = 0; i < CurAbilityQuickSlotIndex; i++)
	{
		if (AbilityQuickSlotTags[i].IsValid())
		{
			CurAbilityQuickSlotIndex = i;
			UKismetSystemLibrary::PrintString(this,TEXT("발견 : ") + FString::FormatAsNumber(i));
			OnUpdateMainAbilityQuickSlot.Broadcast(
				AbilityQuickSlotTags[CurAbilityQuickSlotIndex], false, CurAbilityQuickSlotIndex);
			return;
		}
	}
}

bool UInventoryComponent::IsRegistered(const FGuid& ID) const
{
	return ConsumeQuickSlotUniqueIDs.Contains(ID);
}

bool UInventoryComponent::IsRegistered(const FGameplayTag& Tag) const
{
	return AbilityQuickSlotTags.Contains(Tag);
}

void UInventoryComponent::UseConsumeQuickSlot()
{
	UseItem(ConsumeQuickSlotUniqueIDs[CurConsumeQuickSlotIndex]);
}

void UInventoryComponent::UseAbilityQuickSlot()
{
	GetOwner<ABaseCharacter>()->GetAbilityComponent()->ActivateAbility(AbilityQuickSlotTags[CurAbilityQuickSlotIndex],
	                                                                   GetOwner());
}

TSet<FGuid> UInventoryComponent::GetRegisteredConsumeQuickSlotItems()
{
	TSet<FGuid> out;
	for (auto iter : ConsumeQuickSlotUniqueIDs)
	{
		if (iter != FGuid())
		{
			out.Emplace(iter);
		}
	}

	return out;
}

TSet<FGameplayTag> UInventoryComponent::GetRegisteredQuickSlotAbilities()
{
	TSet<FGameplayTag> out;
	for (auto iter : AbilityQuickSlotTags)
	{
		if (iter.IsValid())
		{
			out.Emplace(iter);
		}
	}

	return out;
}

void UInventoryComponent::OnItemButtonWidgetGeneratedEvent(UUserWidget* UserWidget)
{
	UE_LOGFMT(LogInventory, Log, "OnItemButtonWidgetGeneratedEvent");
	if (UserWidget->IsA<UItemButtonWidget>())
	{
		auto button = Cast<UItemButtonWidget>(UserWidget);
		if (auto data = button->GetInventoryData())
		{
			if (data->IsA<UItemData>())
			{
				const auto& itemData = Cast<UItemData>(data)->InventoryItem;
				if (UItemHelperLibrary::IsConsume(itemData))
				{
					UE_LOGFMT(LogInventory, Log, "등록된 아이템입니다 : {0}",
					          itemData.GetItemInformation()->Item_Name.ToString());
					button->SetEquipped(IsRegistered(itemData.UniqueID));
					return;
				}

				if (UItemHelperLibrary::IsEquipment(itemData))
				{
					UE_LOGFMT(LogInventory, Log, "등록된 장비 아이템입니다 : {0}",
					          itemData.GetItemInformation()->Item_Name.ToString());
					button->SetEquipped(IsEquippedEquipment(itemData.UniqueID));
					return;
				}
				return;
			}


			if (data->IsA<UAbilityData>())
			{
				const auto& abilityData = Cast<UAbilityData>(data)->AbilityInformation;
				button->SetEquipped(IsRegistered(abilityData.AbilityTag));
			}
		}
	}
}

bool UInventoryComponent::HasKey(const FGameplayTag KeyTag)
{
	return K2_HasItem(KeyTag);
}
