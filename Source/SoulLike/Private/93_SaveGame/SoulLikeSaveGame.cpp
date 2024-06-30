// Fill out your copyright notice in the Description page of Project Settings.


#include "93_SaveGame/SoulLikeSaveGame.h"

#include "00_Character/00_Player/SoulTomb.h"
#include "00_Character/01_Component/AbilityComponent.h"
#include "00_Character/04_NPC/99_Component/BonfireComponent.h"
#include "03_Widget/QuickSlotWidget.h"
#include "03_Widget/01_Menu/00_Inventory/InventoryWidget.h"
#include "03_Widget/01_Menu/02_QuickSlot/QuickSlotButtonWidget.h"

#include "03_Widget/01_Menu/03_Equipment/EquipButtonWidget.h"
#include "03_Widget/01_Menu/03_Equipment/EquipWidget.h"
#include "03_Widget/01_Menu/05_Orb/OrbWidget.h"
#include "04_Item/ItemActor.h"
#include "04_Item/00_Consume/00_Potion/PotionItemActor.h"
#include "04_Item/01_Equipment/03_Orb/EquipmentItemActor_OrbFragment.h"
#include "91_Sky/DynamicSkyActor.h"
#include "96_Library/ItemHelperLibrary.h"
#include "98_GameInstance/SoulLikeInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Logging/StructuredLog.h"
#include "WorldPartition/DataLayer/DataLayerAsset.h"
#include "WorldPartition/DataLayer/DataLayerSubsystem.h"

DEFINE_LOG_CATEGORY(LogSave)

void FFieldItem::Add(const FString& LayerAssetFullPath, const FString& SafeName)
{
	if (!FiledItemList.Contains(LayerAssetFullPath))
	{
		FiledItemList.Add(LayerAssetFullPath);
	}

	FiledItemList[LayerAssetFullPath].ItemSafeNames.Emplace(SafeName);
}


/*
void UGameLoadHandler::LoadLevel(UWorld* World, const FString& MapName)
{
	if (MapName.IsEmpty())
	{
		return;
	}

	const auto levelName = UGameplayStatics::GetCurrentLevelName(World);
	if (levelName.Equals(MapName) == false)
	{
		UKismetSystemLibrary::PrintString(
			World,TEXT("저장된 레벨과 달라 레벨을 이동합니다, 기존 저장된 레벨 : ") + MapName + TEXT(" 현재 레벨 : ") + levelName);
		UGameplayStatics::OpenLevel(World, FName(*MapName));
	}
}
*/


FLastSavePoint& FLastSavePoint::operator=(const FBonfireInformation& TeleportedBonfire)
{
	LevelName = TeleportedBonfire.LevelName;
	SavedLocation = TeleportedBonfire.Location;
	SavedBonfireSafeName = TeleportedBonfire.OwnersSafeName;
	SkyTime = TeleportedBonfire.SkyTime;
	//NeedToActivateLayer = TeleportedBonfire.NeedToActivateLayer;
	
	return *this;
}

void UGameLoadHandler::LoadInventory(UInventoryComponent* InventoryComponent, const TArray<FItemSave>& SaveInventory)
{
	if (auto world = InventoryComponent->GetWorld())
	{
		InventoryComponent->Inventory.Empty();

		for (auto iter : SaveInventory)
		{
			TSubclassOf<AItemActor> subclass = LoadClass<AItemActor>(InventoryComponent->GetWorld(),
			                                                         *iter.ItemClassPath);
			const auto& item = FInventoryItem(InventoryComponent->GetOwner(), subclass, iter.ItemCount, iter.UniqueID,
			                                  true);
			InventoryComponent->Inventory.Emplace(iter.UniqueID, item);
			InventoryComponent->OnAddItem.Broadcast(InventoryComponent->GetOwner<ABaseCharacter>(), item, nullptr);
		}
	}
	else
	{
		UE_LOGFMT(LogSave, Error, "인벤토리를 복구하려 했으나, 월드를 가져올 수 없습니다!!");
	}
}

void UGameLoadHandler::LoadOrbMatrix(UInventoryComponent* InventoryComponent, const FGuid& CoreID,
                                     const FOrbMatrixSave& OrbMatrixSave)
{
	if (InventoryComponent->Inventory.Contains(CoreID))
	{
		if (const auto orbInformation = static_cast<const FOrbCoreInformation*>(InventoryComponent->Inventory[
			InventoryComponent->EquippedCore].GetItemInformation()))
		{
			UOrbMatrix* matrix = orbInformation->OrbMatrix;
			if (matrix != nullptr)
			{
				UKismetSystemLibrary::PrintString(InventoryComponent,TEXT("코어 메트릭스를 복구합니다."));
				matrix->LoadMatrix(OrbMatrixSave, InventoryComponent->GetOwner<APlayerCharacter>());

				//위젯 새로고침
				if (auto player = Cast<APlayerCharacter>(InventoryComponent->GetOwner()))
				{
					if (player->GetMainWidget())
					{
						player->GetMainWidget()->UMG_Orb->OnEquipCoreEvent(
							InventoryComponent->Inventory[InventoryComponent->EquippedCore]);
					}
				}
			}
		}
	}
	else
	{
		UE_LOGFMT(LogSave, Error, "코어를 불러올 수 없습니다. 메트릭스 불러오기에 실패했습니다.");
	}
}

void UGameLoadHandler::LoadFragment(UInventoryComponent* InventoryComponent, const TArray<FItemSave>& SaveFragments)
{
	InventoryComponent->Fragments.Empty();
	for (auto iter : SaveFragments)
	{
		const auto& uniqueID = iter.UniqueID;
		const auto& count = iter.ItemCount;

		TSubclassOf<AItemActor> subclass = LoadClass<AItemActor>(InventoryComponent->GetWorld(), *iter.ItemClassPath);
		UE_LOGFMT(LogSave, Log, "저장된 파편 목록입니다 : {0}", iter.ItemClassPath);
		if (subclass)
		{
			const FInventoryItem& fragment = FInventoryItem(InventoryComponent->GetOwner(), subclass, count, uniqueID);
			InventoryComponent->AddFragment(uniqueID, fragment, fragment.GetItemInformation());
			InventoryComponent->OnAddItem.Broadcast(InventoryComponent->GetOwner<ABaseCharacter>(), fragment, nullptr);
		}
		else
		{
			UE_LOGFMT(LogSave, Error, "클래스를 불러오는데 실패했습니다.");
		}
	}
}

void UGameLoadHandler::LoadCore(UInventoryComponent* InventoryComponent, const FGuid& UniqueID)
{
	UE_LOGFMT(LogSave, Warning, "코어 로드");
	if (InventoryComponent->Inventory.Contains(UniqueID))
	{
		UE_LOGFMT(LogSave, Log, "코어를 불러옵니다");
		InventoryComponent->EquippedCore = UniqueID;
		InventoryComponent->UseItem(UniqueID);
	}
	else
	{
		UE_LOGFMT(LogSave, Error, "인벤토리에서 코어를 찾을 수 없습니다 : {0}", UniqueID.ToString());
	}
}

void UGameLoadHandler::LoadWeapon(UInventoryComponent* InventoryComponent, const FGuid& UniqueID)
{
	if (InventoryComponent->Inventory.Contains(UniqueID))
	{
		InventoryComponent->EquippedWeapon = UniqueID;
		InventoryComponent->UseItem(UniqueID);

		if (auto mainWidget = InventoryComponent->GetOwner<APlayerCharacter>()->GetMainWidget())
		{
			mainWidget->ForceUpdateSpirit(InventoryComponent->GetInventoryItem(UniqueID));
		}
	}
}

void UGameLoadHandler::LoadEquippedItem(UInventoryComponent* InventoryComponent, TArray<FGuid> EquipUniqueID,
                                        TMap<FGuid, int32> EquipSlotIndexMap)
{
	for (auto id : EquipUniqueID)
	{
		if (EquipSlotIndexMap.Contains(id))
		{
			//무기는 다른 코드에서 미리 장착하기 때문에 장착할 필요가 없습니다. 따라서 EquipSlotIndexMap변수 내부에는 무기에 대한 슬롯정보가 없습니다만, EquipUniqueID에는 있으니 주의
			if (InventoryComponent->Inventory.Contains(id) && !UItemHelperLibrary::IsWeapon(
				InventoryComponent->Inventory[id]))
			{
				InventoryComponent->UseItem(id);

				//재 장착된 아이템을 장비 위젯에 반영합니다.
				if (auto pawn = InventoryComponent->GetOwner<APlayerCharacter>())
				{
					//1. 슬롯 인덱스 번호에 해당하는 위젯에 정보를 업데이트 합니다.
					if (auto mainWidget = pawn->GetMainWidget())
					{
						if (auto data = NewObject<UItemData>(pawn))
						{
							data->OnPlayerBuyItemFromNPC = mainWidget->UMG_Inventory->UMG_ItemList->
							                                           OnPlayerBuyItemFromNPC;
							data->InventoryItem = InventoryComponent->Inventory[id];

							mainWidget->UMG_Equipment->GetEquipButtonByIndex(EquipSlotIndexMap[id])->
							            SetButtonInfo(data);
							//2. 이 아이템 코드를 가진 아이템 버튼을 찾아서 장착 체크 합니다.
							mainWidget->UMG_Inventory->UMG_ItemList->RefreshFromInventoryItem(
								InventoryComponent->Inventory[id]);
						}
					}


					/*UItemData* data = NewObject<UItemData>();
					data->OnPlayerBuyItemFromNPC = pawn->GetMainMenuWidget()->UMG_Inventory->OnPlayerBuyItemFromNPC;
					data->InventoryItem = InventoryComponent->Inventory[id];

					auto mainMenu = pawn->GetMainMenuWidget();
					mainMenu->UMG_Equip->GetEquipButtonByIndex(EquipSlotIndexMap[id])->SetButtonInfo(data);
					//2. 이 아이템 코드를 가진 아이템 버튼을 찾아서 장착 체크 합니다.
					mainMenu->UMG_Inventory->RefreshFromInventoryItem(InventoryComponent->Inventory[id]);*/
				}
			}
		}
	}
}

void UGameLoadHandler::LoadAttribute(UAttributeComponent* AttributeComponent,
                                     const TMap<EAttributeType, FAttribute>& AttributesNotIncludeLevelUpPointMap,
                                     const TMap<EAttributeType, FAttribute>&
                                     LevelUpPointAttributesMap, bool bIsRespawn)
{
	AttributeComponent->LoadLevelUpPointAttributes(LevelUpPointAttributesMap);
	AttributeComponent->LoadAttributeNotIncludeLevelUpPoint(AttributesNotIncludeLevelUpPointMap, true, bIsRespawn);
}


void UGameLoadHandler::RestorePotionEnhancement(UInventoryComponent* InventoryComponent,
                                                const TMap<FGuid, FEnhancement>& PotionEnhancementMap)
{
	UE_LOGFMT(LogSave, Warning, "물약 강화수치 로드");
	if (InventoryComponent)
	{
		for (auto iter : PotionEnhancementMap)
		{
			if (InventoryComponent->GetInventory().Contains(iter.Key))
			{
				const auto& potion = InventoryComponent->GetInventoryItem(iter.Key);
				Cast<APotionItemActor>(potion.GetSpawndItemActor())->PotionItemInformation.Enhancement = iter.Value;
				InventoryComponent->OnUseItem.Broadcast(InventoryComponent->GetOwner<ABaseCharacter>(), potion);
			}
			else
			{
				UE_LOGFMT(LogSave, Error, "이 아이템 아이디를 가진 대상을 찾을 수 없습니다 : {0}", iter.Key.ToString());
			}
		}
	}
}

void UGameLoadHandler::RestoreQuickSlotState(APlayerCharacter* Player, const TMap<int32, FGuid>& ItemQuickSlotMap,
                                             const TMap<int32, FGameplayTag>& AbilityQuickSlotMap,
                                             int32 SelectedConsumeQuickSlot, int32 SelectedAbilityQuickSlot,
                                             const TSet<FGuid>& EquippedQuickSlotConsumeItems,
                                             const TSet<FGameplayTag>& EquippedQuickSlotAbilities)
{
	if (Player)
	{
		auto invenComp = Player->GetInventoryComponent();
		auto abComp = Player->GetAbilityComponent();

		if (invenComp != nullptr && abComp != nullptr)
		{
			invenComp->CurConsumeQuickSlotIndex = SelectedConsumeQuickSlot;
			invenComp->CurAbilityQuickSlotIndex = SelectedAbilityQuickSlot;


			if (auto mainWidget = Player->GetMainWidget())
			{
				mainWidget->UMG_Equipment->LoadConsumeQuickSlots(ItemQuickSlotMap);
				mainWidget->UMG_Equipment->LoadAbilityQuickSlots(AbilityQuickSlotMap);

				for (auto iter : EquippedQuickSlotConsumeItems)
				{
					//2. 이 아이템 코드를 가진 아이템 버튼을 찾아서 장착 체크 합니다.
					mainWidget->UMG_Inventory->UMG_ItemList->
					            RefreshFromInventoryItem(invenComp->GetInventoryItem(iter));
				}

				for (auto iter : EquippedQuickSlotAbilities)
				{
					//TODO - 추후 등록 가능한 어빌리티가 생기면 작성해야 합니다.
				}


				if (ItemQuickSlotMap.Contains(SelectedConsumeQuickSlot))
				{
					const auto& consumeGUID = ItemQuickSlotMap[SelectedConsumeQuickSlot];
					if (consumeGUID != FGuid())
					{
						mainWidget->UMG_QuickSlot->LoadSelectedConsumeQuickSlot(
							invenComp->GetInventoryItem(consumeGUID));
					}
				}

				if (AbilityQuickSlotMap.Contains(SelectedAbilityQuickSlot))
				{
					const auto& abilityTag = AbilityQuickSlotMap[SelectedAbilityQuickSlot];
					if (abilityTag.IsValid())
					{
						mainWidget->UMG_QuickSlot->LoadSelectedAbilityQuickSlot(abComp->GetAbilityByTag(abilityTag));
					}
				}
			}
		}
	}
}


void UGameLoadHandler::CreateSoulTomb(APlayerCharacter* Player, TSubclassOf<ASoulTomb> TombObject,
                                      const FSoulTombCreateContext& DeadState)
{
	if (!DeadState.bShouldCreateSoulTomb)
	{
		return;
	}

	if (Player)
	{
		FActorSpawnParameters spawnParam;
		spawnParam.Instigator = Player;
		spawnParam.Owner = Player;

		auto tomb = Player->GetWorld()->SpawnActor<ASoulTomb>(TombObject);
		tomb->Activate(DeadState.EXP, DeadState.DeadLocation);
	}
}

void UGameLoadHandler::RestoreBonfire(APlayerCharacter* Player, const TArray<FString>& SavedBonfire)
{
	TArray<AActor*> out;
	UGameplayStatics::GetAllActorsOfClass(Player, ABonfire::StaticClass(), out);

	for (auto iter : out)
	{
		const auto& safeName = GetNameSafe(iter);
		if (SavedBonfire.Contains(safeName))
		{
			Cast<ABonfire>(iter)->ActivateBonfire(Player);
		}
	}
}

void UGameLoadHandler::RestoreDataLayer(APlayerCharacter* Player,
	const TMap<FName, EDataLayerRuntimeState>& LayerStateMap, UDataTable* LayerTable)
{
	if (LayerTable == nullptr)
	{
		return;
	}

	if(auto subsystem =  UDataLayerManager::GetDataLayerManager(Player))
	{
		for(const auto& iter : LayerStateMap)
		{
			const auto& layerPath = iter.Key;
			const auto& layerState = iter.Value;
				
			UE_LOGFMT(LogSave,Log,"1 레이어 상태 복구 : {0} {1}",layerPath.ToString(),StaticEnum<EDataLayerRuntimeState>()->GetValueAsString( layerState));
			subsystem->SetDataLayerInstanceRuntimeState(subsystem->GetDataLayerInstance(layerPath),layerState);
		}
	}


	//OLD
	/*if (UDataLayerSubsystem* dataLayerSubsystem = UWorld::GetSubsystem<UDataLayerSubsystem>(Player->GetWorld()))
	{
		TArray<FDataLayerTable*> allLayers;
		LayerTable->GetAllRows<FDataLayerTable>("",allLayers);
		const auto& allRowNames = LayerTable->GetRowNames();
		
		
		for(const auto& iter : LayerStateMap)
		{
			const auto& layerPath = iter.Key;
			const auto& layerState = iter.Value;
			if(allRowNames.Contains(layerPath))
			{
				const int32 index =  allRowNames.IndexOfByKey(layerPath);
				if(index!=INDEX_NONE)
				{
					UE_LOGFMT(LogSave,Log,"레이어 상태 복구 : {0} {1}",layerPath.ToString(),StaticEnum<EDataLayerRuntimeState>()->GetValueAsString( layerState));
					dataLayerSubsystem->SetDataLayerInstanceRuntimeState(allLayers[index]->DataLayerAsset,layerState);
				}
			}
		}

		
	}
	else
	{
		UE_LOGFMT(LogSave, Error, "데이터 레이어 서브시스템을 가져올 수 없습니다.");
	}*/
}

void UGameLoadHandler::RestoreDataLayer(APlayerCharacter* Player, const FName& LayerToLoadRowName,
                                        UDataTable* LayerTable)
{
	if (LayerTable == nullptr)
	{
		return;
	}

	if(auto subsystem = UDataLayerManager::GetDataLayerManager(Player))
	{
		UE_LOGFMT(LogSave,Log,"2 레이어 상태 복구 : {0} 활성화",LayerToLoadRowName.ToString());
		subsystem->SetDataLayerInstanceRuntimeState(subsystem->GetDataLayerInstance(LayerToLoadRowName),EDataLayerRuntimeState::Activated);
	}

	//OLD
	/*
	if (UDataLayerSubsystem* dataLayerSubsystem = UWorld::GetSubsystem<UDataLayerSubsystem>(Player->GetWorld()))
	{
		if (auto find = LayerTable->FindRow<FDataLayerTable>(LayerToLoadRowName, ""))
		{
			if (auto instance = dataLayerSubsystem->GetDataLayerInstanceFromAsset(find->DataLayerAsset))
			{
				UE_LOGFMT(LogSave, Log, "레이어 복구 성공 : {0}", LayerToLoadRowName);
				dataLayerSubsystem->SetDataLayerRuntimeState(instance, EDataLayerRuntimeState::Activated);
			}
			else
			{
				UE_LOGFMT(LogSave, Error, "이 이름에 해당하는 레이어를 찾을 수 없습니다 : {0}", LayerToLoadRowName);
			}
		}
	}*/
}

void UGameLoadHandler::RestoreDataLayer(TObjectPtr<APlayerCharacter> Player,
                                        const FBonfireInformation& BonfireInformation, UDataTable* LayerTable)
{
	if (LayerTable == nullptr)
	{
		return;
	}

	if(auto subsystem = UDataLayerManager::GetDataLayerManager(Player.Get()))
	{
		//수동으로 설정된 경우만 정해진 레이어를 언로드 합니다.
		if(BonfireInformation.bUseLayerUnload && BonfireInformation.bUnloadLayerManually)
		{
			for(auto iter : BonfireInformation.NeedToUnloadLayer)
			{
				subsystem->SetDataLayerInstanceRuntimeState(subsystem->GetDataLayerInstanceFromAsset(iter.Key.LoadSynchronous()),EDataLayerRuntimeState::Unloaded,iter.Value);
				UE_LOGFMT(LogSave,Log,"다음 레이어를 언로드 합니다 : {0}",iter.Key->GetPathName());
			}
		}

		//이미 로드하거나, 활성화 된 레이어 목록
		TArray<FName> processedLayers;
		
		for(auto iter : BonfireInformation.NeedToLoadLayer)
		{
			processedLayers.Emplace(iter.Key->GetPathName());
			subsystem->SetDataLayerInstanceRuntimeState(subsystem->GetDataLayerInstanceFromAsset(iter.Key.LoadSynchronous()),EDataLayerRuntimeState::Loaded,iter.Value);
			UE_LOGFMT(LogSave,Log,"다음 레이어를 로드 합니다 : {0}",iter.Key->GetPathName());
		}
		
		for(auto iter : BonfireInformation.NeedToActivateLayer)
		{
			processedLayers.Emplace(iter.Key->GetPathName());
			subsystem->SetDataLayerInstanceRuntimeState(subsystem->GetDataLayerInstanceFromAsset(iter.Key.LoadSynchronous()),EDataLayerRuntimeState::Activated,iter.Value);
			UE_LOGFMT(LogSave,Log,"다음 레이어를 활성화 합니다 : {0}",iter.Key->GetPathName());
		}


		if(BonfireInformation.bUseLayerUnload && BonfireInformation.bUnloadLayerManually == false)
		{
			auto layerRows = LayerTable->GetRowNames();
			for(auto processedLayerName : processedLayers)
			{
				UE_LOGFMT(LogSave,Log,"다음 레이어는 이미 처리했으므로, 레이어 리스트에서 제거합니다. : {0}",processedLayerName);
				layerRows.Remove(processedLayerName);
			}

	
			for(auto iter : layerRows)
			{
				if(auto layerElement = LayerTable->FindRow<FDataLayerTable>(iter,""))
				{
					UE_LOGFMT(LogSave,Log,"다음 레이어를 비활성화 합니다 : {0}",iter);
					subsystem->SetDataLayerInstanceRuntimeState(subsystem->GetDataLayerInstanceFromAsset(layerElement->DataLayerAsset),EDataLayerRuntimeState::Unloaded,BonfireInformation.bUnloadRecursive);
				}
			}
		}
		
	}

	//OLD
	/*
	if (UDataLayerSubsystem* dataLayerSubsystem = UWorld::GetSubsystem<UDataLayerSubsystem>(Player->GetWorld()))
	{
		//수동으로 설정된 경우만 정해진 레이어를 언로드 합니다.
		if(BonfireInformation.bUnloadLayerManually)
		{
			for(auto iter : BonfireInformation.NeedToUnloadLayer)
			{
				dataLayerSubsystem->SetDataLayerInstanceRuntimeState(iter.LoadSynchronous(),EDataLayerRuntimeState::Unloaded);
				UE_LOGFMT(LogSave,Log,"다음 레이어를 언로드 합니다 : {0}",iter->GetPathName());
			}
		}


		//이미 로드하거나, 활성화 된 레이어 목록
		TArray<FName> processedLayers;
		
		for(auto iter : BonfireInformation.NeedToLoadLayer)
		{
			processedLayers.Emplace(iter->GetPathName());
			dataLayerSubsystem->SetDataLayerInstanceRuntimeState(iter.LoadSynchronous(),EDataLayerRuntimeState::Loaded);
			UE_LOGFMT(LogSave,Log,"다음 레이어를 로드 합니다 : {0}",iter->GetPathName());
		}
		
		for(auto iter : BonfireInformation.NeedToActivateLayer)
		{
			processedLayers.Emplace(iter->GetPathName());
			dataLayerSubsystem->SetDataLayerInstanceRuntimeState(iter.LoadSynchronous(),EDataLayerRuntimeState::Activated);
			UE_LOGFMT(LogSave,Log,"다음 레이어를 활성화 합니다 : {0}",iter->GetPathName());
		}

		if(BonfireInformation.bUnloadLayerManually == false)
		{
			auto layerRows = LayerTable->GetRowNames();
			for(auto processedLayerName : processedLayers)
			{
				UE_LOGFMT(LogSave,Log,"다음 레이어는 이미 처리했으므로, 레이어 리스트에서 제거합니다. : {0}",processedLayerName);
				layerRows.Remove(processedLayerName);
			}

	
			for(auto iter : layerRows)
			{
				if(auto layerElement = LayerTable->FindRow<FDataLayerTable>(iter,""))
				{
					UE_LOGFMT(LogSave,Log,"다음 레이어를 비활성화 합니다 : {0}",iter);
					dataLayerSubsystem->SetDataLayerInstanceRuntimeState(layerElement->DataLayerAsset,EDataLayerRuntimeState::Unloaded);
				}
			}
		}
				
	}*/
}


void UGameLoadHandler::RestoreSky(APlayerCharacter* Player, float CurrentSkyTime)
{
	if (Player)
	{
		if (auto sky = Player->GetDynamicSky())
		{
			sky->TimeOfDay = CurrentSkyTime;
		}
	}
}

USoulLikeSaveGame::USoulLikeSaveGame()
{
}


void UGameLoadHandler::RestoreEquipmentEnhancement(UInventoryComponent* InventoryComponent,
                                                   const TMap<FGuid, FEnhancement>& EquipmentEnhancedMap)
{
	UE_LOGFMT(LogSave, Warning, "강화수치 로드");
	if (InventoryComponent)
	{
		for (auto iter : EquipmentEnhancedMap)
		{
			if (InventoryComponent->GetInventory().Contains(iter.Key))
			{
				const auto& equip = InventoryComponent->GetInventoryItem(iter.Key);
				Cast<AEquipmentItemActor>(equip.GetSpawndItemActor())->Enhancement = iter.Value;

				InventoryComponent->OnUseItem.Broadcast(InventoryComponent->GetOwner<ABaseCharacter>(), equip);
#if WITH_EDITOR
				auto info = Cast<AEquipmentItemActor>(equip.GetSpawndItemActor())->GetEnhancementInfo();

				UE_LOGFMT(LogSave, Warning, "----------------강화복구----------------");

				UE_LOGFMT(LogSave, Warning, "{0} {1}의 강화 상태 복구 : {2}강", GetNameSafe(equip.GetSpawndItemActor()),
				          equip.GetItemInformation()->Item_Name.ToString(), info.CurEnhancement);
				for (auto e : info.EnhancementMap)
				{
					UE_LOGFMT(LogSave, Warning, "{0}를 {1}번 강화함",
					          StaticEnum<EAttackType>()->GetValueAsString(e.Key), e.Value);
				}

				UE_LOGFMT(LogSave, Warning, "--------------------------------------");
#endif
			}
			else if (InventoryComponent->GetFragments().Contains(iter.Key))
			{
				const auto& equip = InventoryComponent->GetInventoryItem(iter.Key);
				Cast<AEquipmentItemActor>(equip.GetSpawndItemActor())->Enhancement = iter.Value;

				InventoryComponent->OnUseItem.Broadcast(InventoryComponent->GetOwner<ABaseCharacter>(), equip);
#if WITH_EDITOR
				auto info = Cast<AEquipmentItemActor>(equip.GetSpawndItemActor())->GetEnhancementInfo();

				UE_LOGFMT(LogSave, Warning, "----------------강화복구----------------");

				UE_LOGFMT(LogSave, Warning, "{0} {1}의 강화 상태 복구 : {2}강", GetNameSafe(equip.GetSpawndItemActor()),
				          equip.GetItemInformation()->Item_Name.ToString(), info.CurEnhancement);
				for (auto e : info.EnhancementMap)
				{
					UE_LOGFMT(LogSave, Warning, "{0}를 {1}번 강화함",
					          StaticEnum<EAttackType>()->GetValueAsString(e.Key), e.Value);
				}

				UE_LOGFMT(LogSave, Warning, "--------------------------------------");
#endif
			}
		}
	}
}

void USoulLikeSaveGame::SaveLevelName(APlayerCharacter* Player)
{
	SavedMapName = UGameplayStatics::GetCurrentLevelName(Player->GetWorld());
}

void USoulLikeSaveGame::SaveTransform(APlayerCharacter* Player)
{
	if (Player == nullptr)
	{
		return;
	}

	//위치 및 회전정보 저장
	Transform = Player->GetTransform();

	SaveLevelName(Player);
}

void USoulLikeSaveGame::SaveAttribute(APlayerCharacter* Player)
{
	if (Player == nullptr)
	{
		return;
	}

	//어트리뷰트 저장
	if (auto attComp = Player->GetAttributeComponent())
	{
		AttributesNotIncludeLevelUpPoint.Empty();
		{
			const auto& att = attComp->GetAllAttributeNotIncludeLevelUpPoint();
			for (auto iter : att)
			{
				AttributesNotIncludeLevelUpPoint.Add(iter.Key, *iter.Value);
			}

			for (auto iter : AttributesNotIncludeLevelUpPoint)
			{
				UE_LOGFMT(LogTemp, Log, "어트리뷰트 저장 : {0} , {1}",
				          StaticEnum<EAttributeType>()->GetValueAsString(iter.Key), iter.Value.GetCurrent());
			}
		}

		LevelUpPointAttributes.Empty();
		{
			const auto& att = attComp->GetAllLevelUpAttribute();
			for (auto iter : att)
			{
				LevelUpPointAttributes.Add(iter.Key, *iter.Value);
			}
		}
	}

	SaveLevelName(Player);
}

void USoulLikeSaveGame::SaveExp(const FAttribute& ExpAttribute)
{
	if (!AttributesNotIncludeLevelUpPoint.Contains(EAttributeType::EXP))
	{
		AttributesNotIncludeLevelUpPoint.Add(EAttributeType::EXP);
	}

	AttributesNotIncludeLevelUpPoint[EAttributeType::EXP] = ExpAttribute;
}

void USoulLikeSaveGame::SaveFragment(APlayerCharacter* Player)
{
	if (Player == nullptr)
	{
		return;
	}


	if (auto invenComp = Player->GetInventoryComponent())
	{
		Fragments.Empty();
		//신력 파편 저장
		const auto& fragments = invenComp->GetFragments();
		for (auto iter : fragments)
		{
			const auto& uniqueID = iter.Key;
			const auto& classPath = iter.Value.GetItemInformation()->ItemActorObject->GetPathName();
			const auto& count = iter.Value.ItemCount;

			Fragments.Add(FItemSave(uniqueID, classPath, count));
		}
	}

	SaveLevelName(Player);
}

void USoulLikeSaveGame::SaveEquipSlot(APlayerCharacter* Player)
{
	if (Player == nullptr)
	{
		return;
	}

	if (auto invenComp = Player->GetInventoryComponent())
	{
		EquippedWidgetIndexMap.Empty();

		//장비중이던 아이템 아이디 저장
		EquippedItemID = invenComp->GetEquippedItemsID();
		//장착중인 장비가 장비슬롯 몇번에 장착되었는가 저장합니다.
		for (auto id : EquippedItemID)
		{
			if (Player->GetMainWidget())
			{
				auto index = Player->GetMainWidget()->UMG_Equipment->FindIndexByID(id);
				if (index != INDEX_NONE)
				{
					/*UKismetSystemLibrary::PrintString(
						this, invenComp->GetInventoryItem(id).GetItemInformation()->Item_Name + TEXT("는 ") +
						FString::FormatAsNumber(index) + TEXT("번 슬롯에 장착된 상태입니다."));*/
					EquippedWidgetIndexMap.Add(id, index);
				}
			}
		}

		/*
		//장비중인 무기 및 코어 아이디 저장
		if (invenComp->GetEquippedWeaponID() != FGuid())
		{
			WeaponID = invenComp->GetEquippedWeaponID();
		}
		if (invenComp->GetEquippedCoreID() != FGuid())
		{
			CoreID = invenComp->GetEquippedCoreID();
		}*/
	}
}

void USoulLikeSaveGame::SaveEquipEnhancement(APlayerCharacter* Player)
{
	if (Player == nullptr)
	{
		return;
	}

	if (auto invenComp = Player->GetInventoryComponent())
	{
		EquipEnhancementMap.Empty();
		const auto& inventory = invenComp->GetInventory();
		for (auto iter : inventory)
		{
			if (UItemHelperLibrary::IsEquipment(iter.Value))
			{
				EquipEnhancementMap.Add(
					iter.Key, Cast<AEquipmentItemActor>(iter.Value.GetSpawndItemActor())->GetEnhancementInfo());
				UE_LOGFMT(LogSave, Log, "{0} 저장", iter.Key.ToString());

				auto info = Cast<AEquipmentItemActor>(iter.Value.GetSpawndItemActor())->GetEnhancementInfo();
				UE_LOGFMT(LogSave, Log, "{0}의 강화 상태 저장 : {1}강", iter.Value.GetItemInformation()->Item_Name.ToString(),
				          info.CurEnhancement);
				for (auto e : info.EnhancementMap)
				{
					UE_LOGFMT(LogSave, Log, "상태 : {0} 능력치를 {1}만큼 강화",
					          StaticEnum<EAttackType>()->GetValueAsString(e.Key), e.Value);
				}
			}
		}
	}

	SaveLevelName(Player);
}

void USoulLikeSaveGame::SavePotionEnhancement(APlayerCharacter* Player)
{
	if (Player == nullptr)
	{
		return;
	}


	if (auto invenComp = Player->GetInventoryComponent())
	{
		PotionEnhancementMap.Empty();
		const auto& inventory = invenComp->GetInventory();
		for (auto iter : inventory)
		{
			if (UItemHelperLibrary::IsHPPotion(iter.Value) || UItemHelperLibrary::IsMPPotion(iter.Value))
			{
				PotionEnhancementMap.Add(
					iter.Key, Cast<APotionItemActor>(iter.Value.GetSpawndItemActor())->GetPotionEnhancement());

				auto info = Cast<APotionItemActor>(iter.Value.GetSpawndItemActor())->GetPotionEnhancement();
				UE_LOGFMT(LogSave, Log, "{0}의 강화 상태 저장 : {1}강", iter.Value.GetItemInformation()->Item_Name.ToString(),
				          info.CurEnhancement);
			}
		}
	}

	SaveLevelName(Player);
}

void USoulLikeSaveGame::SaveOrbMatrix(APlayerCharacter* Player)
{
	if (Player == nullptr)
	{
		return;
	}

	if (auto invenComp = Player->GetInventoryComponent())
	{
		if (invenComp->GetEquippedCoreID() == FGuid())
		{
			return;
		}
		//라인 상태 저장
		if (const auto coreInfo = static_cast<const FOrbCoreInformation*>(invenComp->GetEquippedCore().
			GetItemInformation()))
		{
			SaveOrbMatrix(coreInfo->OrbMatrix);
		}
		else
		{
			UE_LOGFMT(LogSave, Log, "코어 정보를 불러올 수 없습니다.");
		}
	}

	SaveLevelName(Player);
}

void USoulLikeSaveGame::SaveOrbMatrix(UOrbMatrix* Matrix)
{
	UE_LOGFMT(LogSave, Warning, "코어 매트릭스를 저장합니다.");
	OrbMatrix.Clear();

	OrbMatrix.WholeMatrix = Matrix->OrbMatrix;
	OrbMatrix.PhysicalLine = Matrix->PhysicalLine;
	OrbMatrix.DefenceLine = Matrix->DefenceLine;
	OrbMatrix.MagicalLine = Matrix->MagicalLine;
	OrbMatrix.FreeLine = Matrix->FreeLine;
}

/*
void USoulLikeSaveGame::SaveDeadMonster(APlayerCharacter* Player)
{
	if (Player == nullptr)
	{
		return;
	}

	const auto& mapName = UGameplayStatics::GetCurrentLevelName(Player->GetWorld());
	if (auto gameMode = Cast<ASoulLikeGameMode>(UGameplayStatics::GetGameMode(Player)))
	{
		if (!DeadMonsters.Contains(mapName))
		{
			DeadMonsters.Add(mapName, FMonsterRespawnInfos());
		}

		const auto& respawnInfo = gameMode->GetRespawnInfos();
		if (respawnInfo.Contains(mapName))
		{
			const auto& monstersInfo = respawnInfo[mapName];
			//스폰되어 저장된 친구들중에, 사망상태이거나 지워져서 유효하지 않은 친구들의 정보를 가져옵니다.
			for (auto i = monstersInfo.Num() - 1; i >= 0; i--)
			{
				const auto& info = monstersInfo[i];
				//유효하지 않을 때
				if (info.Monster->IsValidLowLevel() == false)
				{
					//저장
					DeadMonsters[mapName].DeadInfo.AddUnique(info);
				}
				else
				{
					//사망상태인가 체크합니다
					if (info.Monster->IsDead())
					{
						//저장
						DeadMonsters[mapName].DeadInfo.AddUnique(info);
					}
				}
			}
		}
	}
}

void USoulLikeSaveGame::ClearDeadMonster()
{
	UKismetSystemLibrary::PrintString(this,TEXT("저장 사망 몬스터 초기화"));
	DeadMonsters.Empty();
}
*/

void USoulLikeSaveGame::SaveLastSavePoint(const FString& LevelName, const FVector& LastLocation,
                                          UBonfireComponent* BonfireComponent)
{
	SavedLastSavePoint.LevelName = LevelName;
	SavedLastSavePoint.SavedLocation = LastLocation;
	SavedLastSavePoint.SavedBonfireSafeName = GetNameSafe(BonfireComponent->GetOwner());
	if (BonfireComponent != nullptr)
	{
		SavedLastSavePoint.SkyTime = BonfireComponent->GetOwner<ABonfire>()->GetBonfireInformation().SkyTime;

		const auto& info = BonfireComponent->GetOwner<ABonfire>()->GetBonfireInformation();
		UE_LOGFMT(LogSave,Log,"SaveLastSavePoint : {0} , {1}, {2}",info.LocationName.ToString(),info.OwnersSafeName,SavedLastSavePoint.SkyTime);
	}
	
	/*SavedLastSavePoint.NeedToActivateLayer.Empty();

	if (UDataLayerSubsystem* dataLayerSubsystem = UWorld::GetSubsystem<UDataLayerSubsystem>(GetWorld()))
	{
		
		auto activateLayers = dataLayerSubsystem->GetEffectiveActiveDataLayerNames();
		for (auto iter : activateLayers)
		{
			if (const auto instance = dataLayerSubsystem->GetDataLayerInstance<FName>(
				iter, BonfireComponent->GetOwner()->GetLevel()))
			{
				SavedLastSavePoint.ActivateLayersPath.Emplace(instance->GetDataLayerFullName());
			}
		}
	}*/


	UE_LOGFMT(LogSave, Log, "저장된 마지막 세이브 포인트 정보 : {0} {1} {2}", SavedLastSavePoint.LevelName,
	          SavedLastSavePoint.SavedLocation.ToString(), SavedLastSavePoint.SavedBonfireSafeName);
}


void USoulLikeSaveGame::SavePlacementItemState(class APlayerCharacter* Player,
                                               AItemActor* ItemActor)
{
	//레이어가 유효하지 않거나, 오너가 플레이어인경우 레벨에 배치되어있는 아이템이 아니라고 간주합니다.
	if (ItemActor->GetOwner() == Player)
	{
		UE_LOGFMT(LogSave, Warning, "다음 아이템은 오너가 플레이어라 제거할 필요가 없습니다 : {0}", GetNameSafe(ItemActor));
		return;
	}


	const auto& levelName = UGameplayStatics::GetCurrentLevelName(ItemActor);
	const auto& safeName = FName(GetNameSafe(ItemActor));

	if (levelName.IsEmpty())
	{
		UE_LOGFMT(LogSave, Warning, "다음 아이템은 레벨 이름이 유효하지 않아 아이템 획득 상태를 저장할 수 없습니다 : {0}", GetNameSafe(ItemActor));
		return;
	}

	if (!PickUppedItems.Contains(levelName))
	{
		PickUppedItems.Add(levelName);
	}

	PickUppedItems[levelName].Information.Emplace(FActorSave(ItemActor, safeName, ItemActor->GetClass(),
	                                                         ItemActor->GetActorTransform()));
	for (auto iter : PickUppedItems[levelName].Information)
	{
		UE_LOGFMT(LogSave, Log, "저장된 다음 필드 아이템 : {0}", iter.ActorSafeName);
	}
	for (auto iter : PickUppedItems[levelName].Information)
	{
		if (iter == safeName)
		{
			UE_LOGFMT(LogSave, Log, "다음 필드 아이템을 획득하여 상태를 저장합니다 : {0}", safeName);
			break;
		}
	}
}

void USoulLikeSaveGame::SaveAddedItem(const FInventoryItem& ItemInfo)
{
	//공통부분
	const auto& uniqueID = ItemInfo.UniqueID;

	//아이템 종류가 파편일 경우 따로 처리해야 합니다.
	if (UItemHelperLibrary::IsOrbFragment(ItemInfo))
	{
		const auto& classPath = ItemInfo.GetItemInformation()->ItemActorObject->GetPathName();
		Fragments.Add(FItemSave(uniqueID, classPath, ItemInfo.ItemCount));
		UE_LOGFMT(LogSave, Log, "SaveAddedItem : 다음 인벤토리 파편을 저장합니다 : {0}",
		          ItemInfo.GetItemInformation()->Item_Name.ToString());
		//변경된 메트릭스 저장
		SaveOrbMatrix(ItemInfo.GetSpawndItemActor()->GetOwner<APlayerCharacter>());
		return;
	}

	if (InventoryItem.Contains(uniqueID))
	{
		InventoryItem.FindByPredicate([uniqueID](const FItemSave& Element)
		{
			return Element.UniqueID == uniqueID;
		})->ItemCount = ItemInfo.ItemCount;
	}
	else
	{
		const auto& classPath = ItemInfo.GetItemInformation()->ItemActorObject->GetPathName();
		const auto& count = ItemInfo.ItemCount;
		UE_LOGFMT(LogSave, Log, "SaveAddedItem : 다음 인벤토리 아이템을 저장합니다 : {0}",
		          ItemInfo.GetItemInformation()->Item_Name.ToString());
		InventoryItem.Emplace(FItemSave(uniqueID, classPath, count));
	}
	/*UE_LOGFMT(LogSave, Log, "인벤토리에 추가된 아이템을 저장합니다 : {0}, {1} {2}", ItemInfo.GetItemInformation()->Item_Name,
	          ItemInfo.ItemCount, uniqueID);

	UE_LOGFMT(LogSave, Log, "------------------저장된 배열--------------------------");
	for (auto iter : InventoryItem)
	{
		UE_LOGFMT(LogSave, Log, "인벤토리 아이템 리스트 : {0}, {1}", iter.UniqueID, iter.ItemCount);
	}
	UE_LOGFMT(LogSave, Log, "-------------------------------------------------------");*/
}

void USoulLikeSaveGame::SaveUsedItem(APlayerCharacter* UsedBy, const FInventoryItem& ItemInfo)
{
	//공통부분
	const auto& uniqueID = ItemInfo.UniqueID;
	//아이템 종류가 파편일 경우 따로 처리해야 합니다.
	if (UItemHelperLibrary::IsOrbFragment(ItemInfo))
	{
		//변경된 메트릭스 저장
		SaveOrbMatrix(ItemInfo.GetSpawndItemActor()->GetOwner<APlayerCharacter>());
		return;
	}


	if (InventoryItem.Contains(uniqueID))
	{
		int32 findIndex = InventoryItem.IndexOfByPredicate([uniqueID](const FItemSave& Element)
		{
			return Element.UniqueID == uniqueID;
		});

		if (findIndex != INDEX_NONE)
		{
			InventoryItem[findIndex].ItemCount = ItemInfo.ItemCount;
			UE_LOGFMT(LogSave, Log, "사용한 아이템을 저장합니다 : {0} ,남은 갯수 : {1}개",
			          UsedBy->GetInventoryComponent()->GetInventoryItem(InventoryItem[findIndex].UniqueID).
			          GetItemInformation()->Item_Name.ToString(), InventoryItem[findIndex].ItemCount);
		}
		else
		{
			UE_LOGFMT(LogSave, Error, "저장할 아이템을 가져올 수 없습니다!!");
		}
		//장비 아이템인 경우
		if (UItemHelperLibrary::IsEquipment(ItemInfo))
		{
			EquippedItemID.AddUnique(uniqueID);

			if (UItemHelperLibrary::IsOrbCore(ItemInfo))
			{
				UE_LOGFMT(LogSave, Log, "인벤토리에 사용된 코어 아이템을 저장합니다 : {0}, {1} {2}",
				          ItemInfo.GetItemInformation()->Item_Name.ToString(), ItemInfo.ItemCount, uniqueID);
				CoreID = uniqueID;
				SaveOrbMatrix(ItemInfo.GetSpawndItemActor()->GetOwner<APlayerCharacter>());
				return;
			}

			if (UItemHelperLibrary::IsWeapon(ItemInfo))
			{
				if (ItemInfo.GetSpawndItemActor()->GetOwner<APlayerCharacter>()->GetInventoryComponent()->
				             IsEquippedEquipment(
					             ItemInfo.UniqueID))
				{
					UE_LOGFMT(LogSave, Log, "인벤토리에 사용된 무기 아이템을 저장합니다 : {0}, {1}",
					          ItemInfo.GetItemInformation()->Item_Name.ToString(), ItemInfo.ItemCount);
					WeaponID = uniqueID;
				}
			}
		}
	}
}

void USoulLikeSaveGame::SaveRemovedItem(const FGuid& RemoveItemUniqueID)
{
	if (InventoryItem.Contains(RemoveItemUniqueID))
	{
		auto index = InventoryItem.IndexOfByPredicate([RemoveItemUniqueID](const FItemSave& Element)
		{
			return Element.UniqueID == RemoveItemUniqueID;
		});

		if (index != INDEX_NONE)
		{
			InventoryItem.RemoveAt(index);
		}
	}
}

void USoulLikeSaveGame::SaveRegisterItemQuickSlotState(UInventoryData* Data, int32 Index,
                                                       const TSet<FGuid>& QuickSlotItems)
{
	if (Data->IsA<UItemData>())
	{
		EquippedQuickSlotItems = QuickSlotItems;

		auto data = Cast<UItemData>(Data);
		const auto& id = data->InventoryItem.UniqueID;
		//이미 등록된 아이디라면 제거하고 다시 등록합시다.
		for (auto iter = ItemQuickSlotMap.CreateIterator(); iter; ++iter)
		{
			if (iter.Value() == id)
			{
				iter.RemoveCurrent();
				break;
			}
		}

		ItemQuickSlotMap.Add(Index, id);
	}
}


void USoulLikeSaveGame::SaveRegisterAbilityQuickSlotState(UInventoryData* Data, int32 Index,
                                                          const TSet<FGameplayTag>& QuickSlotAbilities)
{
	if (Data->IsA<UAbilityData>())
	{
		EquippedQuickSlotAbilities = QuickSlotAbilities;
		auto data = Cast<UAbilityData>(Data);
		const auto& tag = data->AbilityInformation.AbilityTag;

		//이미 등록된 아이디라면 제거하고 다시 등록합시다.
		for (auto iter = AbilityQuickSlotMap.CreateIterator(); iter; ++iter)
		{
			if (iter.Value().MatchesTagExact(tag))
			{
				iter.RemoveCurrent();
				break;
			}
		}

		AbilityQuickSlotMap.Add(Index, tag);
	}
}

void USoulLikeSaveGame::SaveAllQuickSlotState(APlayerCharacter* Player)
{
	if (Player)
	{
		if (Player->GetMainWidget())
		{
			const auto& consumeQuick = Player->GetMainWidget()->UMG_Equipment->GetAllConsumeQuickSlots();
			ItemQuickSlotMap.Empty();
			for (auto iter : consumeQuick)
			{
				if (auto widget = Cast<UQuickSlotButtonWidget>(iter))
				{
					const auto& id = widget->GetSlotItemUniqueID();
					const auto& index = widget->GetSlotIndex();
					if (id != FGuid())
					{
						ItemQuickSlotMap.Add(index, id);
					}
				}
			}


			const auto& abilityQuick = Player->GetMainWidget()->UMG_Equipment->GetAllAbilityQuickSlots();
			for (auto iter : abilityQuick)
			{
				if (auto widget = Cast<UQuickSlotButtonWidget>(iter))
				{
					const auto& id = widget->GetSlotItemUniqueID();
					const auto& index = widget->GetSlotIndex();
					if (id != FGuid())
					{
						ItemQuickSlotMap.Add(index, id);
					}
				}
			}
		}
		SaveLevelName(Player);
	}
}

void USoulLikeSaveGame::SaveUpgradeEquipmentItem(const FGuid& ID, const FEnhancement& Enhancement)
{
	EquipEnhancementMap.Add(ID, Enhancement);
}

void USoulLikeSaveGame::SaveUpgradePotionItem(const FGuid& ID, const FEnhancement& PotionEnhancement)
{
	PotionEnhancementMap.Add(ID, PotionEnhancement);
}

void USoulLikeSaveGame::SaveUnEquipItem(const FInventoryItem& ItemInfo)
{
	UE_LOGFMT(LogSave, Log, "장착 해제 아이템 저장 : {0} {1}", ItemInfo.GetItemInformation()->Item_Name.ToString(),
	          ItemInfo.UniqueID);

	//공통부분
	const auto& uniqueID = ItemInfo.UniqueID;
	EquippedItemID.Remove(uniqueID);

	//아이템 종류가 파편일 경우 따로 처리해야 합니다.
	if (UItemHelperLibrary::IsOrbFragment(ItemInfo))
	{
		//변경된 메트릭스 저장
		//UKismetSystemLibrary::PrintString(ItemInfo.GetSpawndItemActor(),TEXT("파편 장착 해제"));
		SaveOrbMatrix(ItemInfo.GetSpawndItemActor()->GetOwner<APlayerCharacter>());
	}

	//장착해제랑 인벤토리 저장이랑 뭔상관이지? 이거 왜 있는거야?
	/*if (InventoryItem.Contains(uniqueID))
	{
		auto index = InventoryItem.IndexOfByPredicate([uniqueID](const FItemSave& Element)
		{
			return Element.UniqueID == uniqueID;
		});

		if (index != INDEX_NONE)
		{
			InventoryItem.RemoveAt(index);
		}
	}*/
}

void USoulLikeSaveGame::UpdateSoldItemInSaveFile(UInventoryComponent* InventoryComponent, const FGuid& ItemID)
{
	if (InventoryItem.Contains(ItemID))
	{
		auto index = InventoryItem.IndexOfByPredicate([ItemID](const FItemSave& Element)
		{
			return Element.UniqueID == ItemID;
		});

		//이미 아이템이 인벤토리에서 삭제되었나 확인합니다
		bool bExist = InventoryComponent->IsItemContains(ItemID);

		if (index != INDEX_NONE)
		{
			if (bExist)
			{
				InventoryItem[index].ItemCount = InventoryComponent->GetInventoryItem(ItemID).ItemCount;
			}
			else
			{
				InventoryItem.RemoveAt(index);
			}
		}
	}
}

void USoulLikeSaveGame::SaveDeadState(APlayerCharacter* Player)
{
	DeadState.DeadLocation = Player->GetLastGroundedLocation();
	DeadState.EXP = Player->GetAttributeComponent()->GetEXP();
	DeadState.bShouldCreateSoulTomb = true;

	SaveLevelName(Player);
}

void USoulLikeSaveGame::Clear()
{
	Transform = FTransform::Identity;

	AttributesNotIncludeLevelUpPoint.Empty();

	InventoryItem.Empty();

	Fragments.Empty();

	EquippedItemID.Empty();
	EquippedWidgetIndexMap.Empty();
	EquipEnhancementMap.Empty();
	PotionEnhancementMap.Empty();

	WeaponID = FGuid();
	CoreID = FGuid();
	OrbMatrix = FOrbMatrixSave();

	//DeadMonsters.Empty();

	PickUppedItems.Empty();

	ItemQuickSlotMap.Empty();
	AbilityQuickSlotMap.Empty();
}

void USoulLikeSaveGame::SaveActivateBonfire(ABonfire* Bonfire)
{
	if (Bonfire)
	{
		const auto& safeName = FName(GetNameSafe(Bonfire));
		const auto& levelName = UGameplayStatics::GetCurrentLevelName(Bonfire);
		if (!ActivateBonfire.Contains(levelName))
		{
			ActivateBonfire.Add(levelName);
		}
		UE_LOGFMT(LogSave, Log, "화톳불 / 저장 : {0}", safeName);

		if (!ActivateBonfire[levelName].Information.Array().Contains(safeName))
		{
			UE_LOGFMT(LogSave, Log, "이 화톳불을 저장합니다 : {0}", safeName);
			ActivateBonfire[levelName].Information.Add(
				FActorSave(Bonfire, safeName, Bonfire->GetClass(), Bonfire->GetActorTransform()));
		}
	}
}

void USoulLikeSaveGame::SaveTeleportBonfireInfo(const FBonfireInformation& BonfireInfo)
{
	TeleportBonfireInfo = BonfireInfo;
}

void USoulLikeSaveGame::SaveInventory(APlayerCharacter* Player)
{
	if (auto invenComp = Player->GetInventoryComponent())
	{
		const auto& items = invenComp->GetInventory();
		for (auto iter : items)
		{
			if (!InventoryItem.Contains(iter.Key))
			{
				UE_LOGFMT(LogSave, Log, "SaveInventory : 처음으로 다음 인벤토리 아이템을 저장합니다 : {0}",
				          iter.Value.GetItemInformation()->Item_Name.ToString());
				const auto& classPath = iter.Value.GetItemInformation()->ItemActorObject->GetPathName();
				InventoryItem.Emplace(FItemSave(iter.Key, classPath, iter.Value.ItemCount));
			}
			else
			{
				UE_LOGFMT(LogSave, Log, "SaveInventory : 이미 저장되었으니 업데이트. 다음 인벤토리 아이템을 저장합니다 : {0}",
				          iter.Value.GetItemInformation()->Item_Name.ToString());
				InventoryItem.FindByKey(iter.Key)->ItemCount = iter.Value.ItemCount;
			}
		}

		const auto& frags = invenComp->GetFragments();
		for (auto iter : frags)
		{
			if (!Fragments.Contains(iter.Key))
			{
				UE_LOGFMT(LogSave, Log, "SaveInventory : 다음 인벤토리 파편을 저장합니다 : {0}",
				          iter.Value.GetItemInformation()->Item_Name.ToString());
				const auto& classPath = iter.Value.GetItemInformation()->ItemActorObject->GetPathName();
				Fragments.Emplace(FItemSave(iter.Key, classPath, iter.Value.ItemCount));
			}
			else
			{
				UE_LOGFMT(LogSave, Log, "SaveInventory : 이미 저장되었으니 업데이트. 다음 인벤토리 파편을 저장합니다 : {0}",
				          iter.Value.GetItemInformation()->Item_Name.ToString());
				Fragments.FindByKey(iter.Key)->ItemCount = iter.Value.ItemCount;
			}
		}
	}
}

void USoulLikeSaveGame::SaveDataLayer(APlayerCharacter* Player, UDataTable* LayerTable)
{
	if (LayerTable)
	{
		/*TArray<FDataLayerTable*> layers;
		LayerTable->GetAllRows<FDataLayerTable>("", layers);
		const auto& rowNames = LayerTable->GetRowNames();
		*/

		if (auto manager = UDataLayerManager::GetDataLayerManager(Player))
		{
			LastSavedLayerState.Empty();

			
			for(auto iter : manager->GetEffectiveActiveDataLayerNames())
			{
				
				UE_LOGFMT(LogSave,Log,"SaveDataLayerState : {0} {1}",iter,StaticEnum<EDataLayerRuntimeState>()->GetValueAsString(manager->GetDataLayerInstanceRuntimeState(manager->GetDataLayerInstanceFromName(iter))));
				LastSavedLayerState.Emplace(iter,EDataLayerRuntimeState::Activated);
			}
			for(auto iter :manager->GetEffectiveLoadedDataLayerNames())
			{
				UE_LOGFMT(LogSave,Log,"SaveDataLayerState : {0} {1}",iter,StaticEnum<EDataLayerRuntimeState>()->GetValueAsString(manager->GetDataLayerInstanceRuntimeState(manager->GetDataLayerInstanceFromName(iter))));
				LastSavedLayerState.Emplace(iter,EDataLayerRuntimeState::Loaded);
			}
			

			
			/*for (int32 i = 0; i < layers.Num(); i++)
			{
				UE_LOGFMT(LogSave,Log,"SaveDataLayerState : {0} {1}",rowNames[i],StaticEnum<EDataLayerRuntimeState>()->GetValueAsString(manager->GetDataLayerInstanceRuntimeState(manager->GetDataLayerInstanceFromAsset(layers[i]->DataLayerAsset))));
				LastSavedLayerState.Emplace(rowNames[i],manager->GetDataLayerInstanceRuntimeState(manager->GetDataLayerInstanceFromAsset(layers[i]->DataLayerAsset)));
			}*/
			
		}else
		{
			UE_LOGFMT(LogSave,Log,"레이어 메니저를 가져올 수 없습니다.");
		}
	}
}

void USoulLikeSaveGame::SaveSkyTime(APlayerCharacter* Player)
{
	if (auto sky = Player->GetDynamicSky())
	{
		CurrentSkyTime = sky->TimeOfDay;
	}
}

void USoulLikeSaveGame::SaveKilledBoss(const FGameplayTag& BossTag)
{
	UE_LOGFMT(LogSave, Log, "처치한 우두머리 정보를 기록합니다. : {0}", BossTag.ToString());
	KilledBossMonstersTag.Emplace(BossTag);
}

void USoulLikeSaveGame::SaveSelectedConsumeQuickSlotIndex(int32 SelectedIndex)
{
	SelectedConsumeQuickSlot = SelectedIndex;
}

void USoulLikeSaveGame::SaveSelectedAbilityQuickSlotIndex(int32 SelectedIndex)
{
	SelectedAbilityQuickSlot = SelectedIndex;
}

void USoulLikeSaveGame::SaveChest(AChest* Chest, bool bEaredChestItem)
{

	if(Chest){
		const auto& safeName = FName(GetNameSafe(Chest));
		const auto& levelName = UGameplayStatics::GetCurrentLevelName(Chest);
		if(OpenedChests.Contains(levelName)==false)
		{
			OpenedChests.Emplace(levelName);
		}

		if(!OpenedChests[levelName].Information.Array().Contains(safeName))
		{
			//뭐 열림 여부만 저장되면 되는거라 모든 정보를 다 저장할 필요는 없지만, 혹시 몰루니깐
			OpenedChests[levelName].Information.Add(FActorSave(Chest,safeName,Chest->GetClass(),Chest->GetTransform()));
		}

		if(bEaredChestItem)
		{
			if(EarnedChestItems.Contains(levelName)==false)
			{
				EarnedChestItems.Emplace(levelName);
			}

			if(!EarnedChestItems[levelName].Information.Array().Contains(safeName))
			{
				//뭐 열림 여부만 저장되면 되는거라 모든 정보를 다 저장할 필요는 없지만, 혹시 몰루니깐
				EarnedChestItems[levelName].Information.Add(FActorSave(Chest,safeName,Chest->GetClass(),Chest->GetTransform()));
			}
		}
	}
}

void USoulLikeSaveGame::SaveNPC(ANPCBase* NPC, bool bIsDestroyed)
{
	const auto& tag = NPC->GetNPCTag();
	if(tag.IsValid())
	{
		if(!NPCState.Contains(tag))
		{
			NPCState.Add(tag,FNPCState(NPC,bIsDestroyed));
			return;
		}

		NPCState[tag].Update(NPC);

		UE_LOGFMT(LogSave,Log,"NPC 상태 저장 : {0} {1} {2} {3}",NPCState[tag].bHasMet,NPCState[tag].bJoinBasement,NPCState[tag].SafeName,NPCState[tag].NPCLocation.ToString());
	}
}

void USoulLikeSaveGame::LoadNPC(ANPCBase* NPC)
{
	const auto& tag = NPC->GetNPCTag();
	if(tag.IsValid())
	{
		if(NPCState.Contains(tag))
		{	
			if(NPCState[tag].bHasMet)
			{
				NPC->SetMetNPC(false);
			}
		
			if(NPCState[tag].bJoinBasement)
			{
				NPC->SetJoinNPCAtBasement(false);
			}
			
			if(NPCState[tag].bIsDestroyed)
			{
				UE_LOGFMT(LogSave,Warning,"{0} NPC는 제거된 상태로 저장되었습니다. Destroy합니다.",tag.ToString());
				NPC->Destroyed();
				return;
			}
			

			UE_LOGFMT(LogSave,Log,"NPC 상태 로드 : {0} {1} {2} {3}",NPCState[tag].bHasMet,NPCState[tag].bJoinBasement,NPCState[tag].SafeName,NPCState[tag].NPCLocation.ToString());
			NPC->OnLoadNPCState.Broadcast(NPCState[tag]);
		}
	}
}

void USoulLikeSaveGame::SaveReadTutorialActor(ATutorialActor* TutorialActor)
{
	const auto& name = FName(GetNameSafe(TutorialActor));
	if (!ReadTutorialActors.Contains(name))
	{
		ReadTutorialActors.Emplace(name);
	}
}

void USoulLikeSaveGame::SaveReadTutorial(FGameplayTag TutorialTag)
{
	ReadTutorialTags.Emplace(TutorialTag);
}

bool USoulLikeSaveGame::IsSkippedTutorial(FGameplayTag TutorialTag)
{
	return ReadTutorialTags.Contains(TutorialTag);
}

/*
FString USoulLikeSaveGame::UAIDParser(UObject* Object)
{
	if (Object)
	{
		auto safeName = GetNameSafe(Object);
		return UAIDParser(safeName);
	}

	return FString();
}


FString USoulLikeSaveGame::UAIDParser(const FString& SafeName)
{
	FString key = "UAID_";
	int32 startIndex = SafeName.Find(key);

	if (startIndex != INDEX_NONE)
	{
		FString UAID = SafeName.Mid(startIndex + key.Len());
		return UAID;
	}
	return SafeName;
}
*/
APlayerCharacter* USoulLikeSaveGame::GetPlayerCharacter()
{
	return Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
}
