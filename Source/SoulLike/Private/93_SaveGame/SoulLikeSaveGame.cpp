// Fill out your copyright notice in the Description page of Project Settings.


#include "93_SaveGame/SoulLikeSaveGame.h"

#include "00_Character/00_Player/SoulTomb.h"
#include "00_Character/03_Monster/BaseMonster.h"
#include "00_Character/04_NPC/99_Component/BonfireComponent.h"
#include "03_Widget/01_Menu/00_Inventory/InventoryWidget.h"
#include "03_Widget/01_Menu/02_QuickSlot/QuickSlotButtonWidget.h"

#include "03_Widget/01_Menu/03_Equipment/EquipButtonWidget.h"
#include "03_Widget/01_Menu/03_Equipment/EquipWidget.h"
#include "03_Widget/01_Menu/05_Orb/OrbWidget.h"
#include "04_Item/ItemActor.h"
#include "04_Item/00_Consume/00_Potion/PotionItemActor.h"
#include "04_Item/01_Equipment/03_Orb/EquipmentItemActor_OrbFragment.h"
#include "96_Library/ItemHelperLibrary.h"
#include "98_GameInstance/SoulLikeInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Logging/StructuredLog.h"

DEFINE_LOG_CATEGORY(LogSave)

void FSafeNameSave::Add(const FString& SafeName)
{
	SafeNameList.AddUnique(SafeName);
}

FString FSafeNameSave::Get(const FString& SafeName)
{
	return SafeNameList[SafeNameList.Find(SafeName)];
}

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

void UGameLoadHandler::LoadInventory(UInventoryComponent* InventoryComponent, const TArray<FItemSave>& SaveInventory)
{
	if(auto world = InventoryComponent->GetWorld())
	{
		UE_LOGFMT(LogSave,Warning,"인벤토리 로드");
		InventoryComponent->Inventory.Empty();

		FActorSpawnParameters spawnParam;
		spawnParam.Owner = InventoryComponent->GetOwner();
		spawnParam.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		
		for (auto iter : SaveInventory)
		{			
			TSubclassOf<AItemActor> subclass = LoadClass<AItemActor>(InventoryComponent->GetWorld(), *iter.ItemClassPath);
			auto item = FInventoryItem(InventoryComponent->GetOwner(), subclass, iter.ItemCount, iter.UniqueID);
			UE_LOGFMT(LogSave,Warning,"인벤토리 아이템 복구 : {0} {1} {2}",item.GetItemInformation()->Item_Name, iter.ItemCount,item.UniqueID.ToString());
			
			InventoryComponent->Inventory.Add(iter.UniqueID, FInventoryItem(InventoryComponent->GetOwner(), subclass, iter.ItemCount, iter.UniqueID));
			InventoryComponent->OnAddItem.Broadcast(InventoryComponent->GetOwner<ABaseCharacter>(), item, nullptr);
		}
	}else
	{
		UE_LOGFMT(LogSave,Error,"인벤토리를 복구하려 했으나, 월드를 가져올 수 없습니다!!");
	}
}

void UGameLoadHandler::LoadOrbMatrix(UInventoryComponent* InventoryComponent, const FGuid& CoreID, const FOrbMatrixSave& OrbMatrixSave)
{
	if (InventoryComponent->Inventory.Contains(CoreID))
	{
		if (const auto orbInformation = static_cast<const FOrbCoreInformation*>(InventoryComponent->Inventory[
			InventoryComponent->EquippedCore].GetItemInformation()))
		{
			UOrbMatrix* matrix = orbInformation->OrbMatrix;
			if (matrix!=nullptr)
			{
				UKismetSystemLibrary::PrintString(InventoryComponent,TEXT("코어 메트릭스를 복구합니다."));
				matrix->LoadMatrix(OrbMatrixSave, InventoryComponent->GetOwner<APlayerCharacter>());

				//위젯 새로고침
				if (auto player = Cast<APlayerCharacter>(InventoryComponent->GetOwner()))
				{
					if(player->GetMainWidget()){
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
	UE_LOGFMT(LogSave,Warning,"파편 로드");
	for (auto iter : SaveFragments)
	{
		const auto& uniqueID = iter.UniqueID;
		const auto& count = iter.ItemCount;

		TSubclassOf<AItemActor> subclass = LoadClass<AItemActor>(InventoryComponent->GetWorld(), *iter.ItemClassPath);
		if (subclass)
		{
			UKismetSystemLibrary::PrintString(InventoryComponent->GetWorld(), iter.ItemClassPath);
			const FInventoryItem& fragment = FInventoryItem(InventoryComponent->GetOwner(), subclass, count, uniqueID);
			InventoryComponent->AddFragment(uniqueID, fragment, fragment.GetItemInformation());
			InventoryComponent->OnAddItem.Broadcast(InventoryComponent->GetOwner<ABaseCharacter>(), fragment, nullptr);
		}
		else
		{
			UE_LOGFMT(LogSave,Error,"클래스를 불러오는데 실패했습니다.");
		}
	}
}

void UGameLoadHandler::LoadCore(UInventoryComponent* InventoryComponent, const FGuid& UniqueID)
{
	UE_LOGFMT(LogSave,Warning,"코어 로드");
	if (InventoryComponent->Inventory.Contains(UniqueID))
	{
		UE_LOGFMT(LogSave,Log,"코어를 불러옵니다");
		InventoryComponent->EquippedCore = UniqueID;
		InventoryComponent->UseItem(UniqueID);
	}else
	{
		UE_LOGFMT(LogSave,Error,"인벤토리에서 코어를 찾을 수 없습니다 : {0}",UniqueID.ToString());
	}
}

void UGameLoadHandler::LoadWeapon(UInventoryComponent* InventoryComponent, const FGuid& UniqueID)
{
	if (InventoryComponent->Inventory.Contains(UniqueID))
	{
		InventoryComponent->EquippedWeapon = UniqueID;
		InventoryComponent->UseItem(UniqueID);
		
		/*InventoryComponent->Inventory[UniqueID].Use(InventoryComponent->GetOwner());
		*/

		if(auto mainWidget = InventoryComponent->GetOwner<APlayerCharacter>()->GetMainWidget()){
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
					if(auto mainWidget = pawn->GetMainWidget())
					{
						if(auto data = NewObject<UItemData>(pawn))
						{
							data->OnPlayerBuyItemFromNPC = mainWidget->UMG_Inventory->UMG_ItemList->OnPlayerBuyItemFromNPC;
							data->InventoryItem = InventoryComponent->Inventory[id];

							mainWidget->UMG_Equipment->GetEquipButtonByIndex(EquipSlotIndexMap[id])->SetButtonInfo(data);
							//2. 이 아이템 코드를 가진 아이템 버튼을 찾아서 장착 체크 합니다.
							mainWidget->UMG_Inventory->UMG_ItemList->RefreshFromInventoryItem(InventoryComponent->Inventory[id]);
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
                                     const TMap<EAttributeType, FAttribute>& AttributesNotIncludeLevelUpPointMap, const TMap<EAttributeType, FAttribute>&
                                     LevelUpPointAttributesMap)
{
	for (auto iter : AttributesNotIncludeLevelUpPointMap)
	{
		UE_LOGFMT(LogSave,Log,"어트리뷰트 로드 : {0} , {1}",StaticEnum<EAttributeType>()->GetValueAsString(iter.Key),iter.Value.GetCurrent());
		AttributeComponent->AttributesNotIncludeLevelUpPoint[iter.Key]->Load(iter.Value);
	}
	
	for (auto iter : LevelUpPointAttributesMap)
	{
		AttributeComponent->LevelUpPointAttributes[iter.Key]->Load(iter.Value);
	}


	AttributeComponent->InitProgressWidget();
	AttributeComponent->OnCharacterInformationUpdate.Broadcast();
}

void UGameLoadHandler::RestoreMonsterState(UWorld* World, const TMap<FString, FMonsterRespawnInfos>& DeadMonsters)
{
	const auto& mapName = UGameplayStatics::GetCurrentLevelName(World);

	if (DeadMonsters.Contains(mapName))
	{
		const auto& deadInfo = DeadMonsters[mapName].DeadInfo;
		TArray<AActor*> out;
		UGameplayStatics::GetAllActorsOfClass(World, ABaseMonster::StaticClass(), out);
		for (auto i = 0; i < out.Num(); i++)
		{
			const FString saveName = GetNameSafe(out[i]);
			//사망 배열에서 필드 몬스터들중 같은 대상을 찾습니다.
			auto find = deadInfo.FindByPredicate([saveName](const FMonsterRespawnInfo& Info)
			{
				return Info.MonsterSafeName.Equals(saveName);
			});

			//찾았다면 제거합니다.
			if (find != nullptr)
			{
				//이름 바꾸고 지워야 나중에 문제 안 생깁니다.
				const auto& tempName = out[i]->GetName() + TEXT("Destroyed");
				out[i]->Rename(*tempName);
				out[i]->Destroy();
			}
		}
		/*
		const auto& deadMonstersUAID = DeadMonsters[mapName].;

		TArray<AActor*> out;
		UGameplayStatics::GetAllActorsOfClass(World, ABaseMonster::StaticClass(), out);
		for (auto i = 0; i < out.Num(); i++)
		{
			const FString fieldMonsterUAID = USoulLikeSaveGame::UAIDParser(out[i]);
			if (deadMonstersUAID.Contains(fieldMonsterUAID))
			{
				out[i]->Destroy();
			}
		}*/
	}
}

void UGameLoadHandler::RestoreFieldItemState(APlayerCharacter* Player, const TMap<FString, FSafeNameSave>& PickuppedItems)
{
	const FString& MapName = UGameplayStatics::GetCurrentLevelName(Player->GetWorld());
	UKismetSystemLibrary::PrintString(this,TEXT("필드 아이템 상황을 복구합니다 : ") + MapName);
	if (PickuppedItems.Contains(MapName))
	{
		const auto& itemUAID = PickuppedItems[MapName].SafeNameList;

		for (auto iter : itemUAID)
		{
			UKismetSystemLibrary::PrintString(this,TEXT("저장된 필드 아이템 : ") + iter);
		}

		TArray<AActor*> out;
		UGameplayStatics::GetAllActorsOfClass(Player, AItemActor::StaticClass(), out);
		for (auto i = 0; i < out.Num(); i++)
		{
			if (out[i]->GetOwner() != Player)
			{
				const FString saveName = GetNameSafe(out[i]);
				if (itemUAID.Contains(saveName))
				{
					UKismetSystemLibrary::PrintString(this,TEXT("다음 필드 아이템 제거 : ") + saveName);
					out[i]->Destroy();
				}
			}
		}

		for (auto iter : itemUAID)
		{
			UKismetSystemLibrary::PrintString(this,TEXT("복구 후, 저장된 필드 아이템 : ") + iter);
		}

	}
	else
	{
		UKismetSystemLibrary::PrintString(this,TEXT("해당 필드 정보가 없습니다 : ") + MapName);
	}
}

void UGameLoadHandler::RestorePotionEnhancement(UInventoryComponent* InventoryComponent,
                                                const TMap<FGuid, FEnhancement>& PotionEnhancementMap)
{
	UE_LOGFMT(LogSave,Warning,"물약 강화수치 로드");
	if (InventoryComponent)
	{
		for (auto iter : PotionEnhancementMap)
		{
			if (InventoryComponent->GetInventory().Contains(iter.Key))
			{
				const auto& potion = InventoryComponent->GetInventoryItem(iter.Key);
				Cast<APotionItemActor>(potion.GetSpawndItemActor())->PotionItemInformation.Enhancement = iter.Value;
				InventoryComponent->OnUseItem.Broadcast(InventoryComponent->GetOwner<ABaseCharacter>(),potion);
			}
			else
			{
				UE_LOGFMT(LogSave, Error, "이 아이템 아이디를 가진 대상을 찾을 수 없습니다 : {0}", iter.Key.ToString());
			}
		}
	}
}

void UGameLoadHandler::RestoreQuickSlotState(APlayerCharacter* Player, const TMap<int32, FGuid>& ItemQuickSlotMap,
	const TMap<int32, FGameplayTag>& AbilityQuickSlotMap)
{
	if(Player)
	{
		if(auto mainWidget = Player->GetMainWidget())
		{
			mainWidget->UMG_Equipment->LoadConsumeQuickSlots(ItemQuickSlotMap);
			mainWidget->UMG_Equipment->LoadAbilityQuickSlots(AbilityQuickSlotMap);
		}
	}
}

void UGameLoadHandler::CreateSoulTomb(APlayerCharacter* Player, const FDeadState& DeadState)
{
	if(!DeadState.bShouldCreateSoulTomb)
	{
		return;
	}
	
	if(Player->GetWorld())
	{
		FActorSpawnParameters spawnParams;
		spawnParams.Instigator = Player;
		spawnParams.Owner = Player;
		spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		if(auto tomb = Player->GetWorld()->SpawnActor<ASoulTomb>(ASoulTomb::StaticClass(),DeadState.DeadLocation,FRotator::ZeroRotator,spawnParams))
		{
			tomb->SetEXP(DeadState.EXP);
		}
	}
}

void UGameLoadHandler::RestoreBonfire(APlayerCharacter* Player, const TArray<FString>& SavedBonfire)
{
	TArray<AActor*> out;
	UGameplayStatics::GetAllActorsOfClass(Player,ABonfire::StaticClass(),out);

	for(auto iter :out)
	{
		const auto& safeName = GetNameSafe(iter);
		if(SavedBonfire.Contains(safeName))
		{
			Cast<ABonfire>(iter)->ActivateBonfire(Player);
		}
		
	}
}

void UGameLoadHandler::TeleportToBonfire(APlayerCharacter* Player, const FBonfireInformation& BonfireInfo)
{
	//Player->SetActorLocation(BonfireInfo.Location,false,nullptr,ETeleportType::TeleportPhysics);
	Player->K2_TeleportTo(BonfireInfo.Location,Player->GetActorRotation());
	/*TArray<AActor*> out;
	UGameplayStatics::GetAllActorsOfClass(Player,ABonfire::StaticClass(),out);

	for(auto iter :out)
	{
		const auto& safeName = GetNameSafe(iter);
		if(BonfireInfo.OwnersSafeName.Equals(safeName))
		{
			Player->SetActorLocation(iter->GetActorLocation(),false,nullptr,ETeleportType::TeleportPhysics);
			return;
		}
	}*/
}

void UGameLoadHandler::RestoreTutorial(APlayerCharacter* Player, const TArray<FString>& Tutorials)
{
	TArray<AActor*> out;
	UGameplayStatics::GetAllActorsOfClass(Player,ATutorialActor::StaticClass(),out);
	
	for(auto iter :out)
	{
		const auto& safeName = GetNameSafe(iter);
		if(Tutorials.Contains(safeName)){
			iter->Destroy();
		}
	}
}


void UGameLoadHandler::RestoreEquipmentEnhancement(UInventoryComponent* InventoryComponent,
                                                   const TMap<FGuid, FEnhancement>& EquipmentEnhancedMap)
{
	UE_LOGFMT(LogSave,Warning,"강화수치 로드");
	if (InventoryComponent)
	{
		for (auto iter : EquipmentEnhancedMap)
		{
			if (InventoryComponent->GetInventory().Contains(iter.Key))
			{
				const auto& equip = InventoryComponent->GetInventoryItem(iter.Key);
				Cast<AEquipmentItemActor>(equip.GetSpawndItemActor())->Enhancement = iter.Value;

				InventoryComponent->OnUseItem.Broadcast(InventoryComponent->GetOwner<ABaseCharacter>(),equip);
#if WITH_EDITOR
				auto info = Cast<AEquipmentItemActor>(equip.GetSpawndItemActor())->GetEnhancementInfo();

				UE_LOGFMT(LogSave,Warning,"----------------강화복구----------------");

				UE_LOGFMT(LogSave, Warning, "{0} {1}의 강화 상태 복구 : {2}강", GetNameSafe(equip.GetSpawndItemActor()),
						  equip.GetItemInformation()->Item_Name, info.CurEnhancement);
				for (auto e : info.EnhancementMap)
				{
					UE_LOGFMT(LogSave, Warning, "{0}를 {1}번 강화함",
							  StaticEnum<EAttackType>()->GetValueAsString(e.Key), e.Value);
				}
				
				UE_LOGFMT(LogSave,Warning,"--------------------------------------");
#endif
			}
			else if(InventoryComponent->GetFragments().Contains(iter.Key))
			{
				const auto& equip = InventoryComponent->GetInventoryItem(iter.Key);
				Cast<AEquipmentItemActor>(equip.GetSpawndItemActor())->Enhancement = iter.Value;

				InventoryComponent->OnUseItem.Broadcast(InventoryComponent->GetOwner<ABaseCharacter>(),equip);
#if WITH_EDITOR
				auto info = Cast<AEquipmentItemActor>(equip.GetSpawndItemActor())->GetEnhancementInfo();

				UE_LOGFMT(LogSave,Warning,"----------------강화복구----------------");

				UE_LOGFMT(LogSave, Warning, "{0} {1}의 강화 상태 복구 : {2}강", GetNameSafe(equip.GetSpawndItemActor()),
						  equip.GetItemInformation()->Item_Name, info.CurEnhancement);
				for (auto e : info.EnhancementMap)
				{
					UE_LOGFMT(LogSave, Warning, "{0}를 {1}번 강화함",
							  StaticEnum<EAttackType>()->GetValueAsString(e.Key), e.Value);
				}
				
				UE_LOGFMT(LogSave,Warning,"--------------------------------------");
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

		UKismetSystemLibrary::PrintString(this,TEXT("어트리뷰트 상태 저장됨"));
		{
			const auto& att = attComp->GetAllAttributeNotIncludeLevelUpPoint();
			for (auto iter : att)
			{
				AttributesNotIncludeLevelUpPoint.Add(iter.Key, *iter.Value);
			}
			
			for (auto iter : AttributesNotIncludeLevelUpPoint)
			{
				UE_LOGFMT(LogTemp,Log,"어트리뷰트 저장 : {0} , {1}",StaticEnum<EAttributeType>()->GetValueAsString(iter.Key),iter.Value.GetCurrent());
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

			UKismetSystemLibrary::PrintString(Player->GetWorld(), classPath);

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
			if( Player->GetMainWidget())
			{
				auto index = Player->GetMainWidget()->UMG_Equipment->FindIndexByID(id);
				if (index != INDEX_NONE)
				{
					UKismetSystemLibrary::PrintString(
						this, invenComp->GetInventoryItem(id).GetItemInformation()->Item_Name + TEXT("는 ") + FString::FormatAsNumber(index) + TEXT("번 슬롯에 장착된 상태입니다."));
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
				UE_LOGFMT(LogSave, Log, "{0}의 강화 상태 저장 : {1}강", iter.Value.GetItemInformation()->Item_Name,
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
				UE_LOGFMT(LogSave, Log, "{0}의 강화 상태 저장 : {1}강", iter.Value.GetItemInformation()->Item_Name,
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
	UE_LOGFMT(LogSave,Warning,"코어 매트릭스를 저장합니다.");
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

void USoulLikeSaveGame::SaveLastSavePoint(const FString& LevelName, const FTransform& LastTransform, UBonfireComponent* BonfireComponent)
{
	SavedLastSavePoint.LevelName = LevelName;
	SavedLastSavePoint.SavedTransform = LastTransform;
	SavedLastSavePoint.SavedBonfireName = GetNameSafe(BonfireComponent->GetOwner());

	UKismetSystemLibrary::PrintString(this,TEXT("마지막으로 체크한 포인트를 저장합니다 : ")+SavedLastSavePoint.LevelName);
}

/*
void USoulLikeSaveGame::SaveAll(APlayerCharacter* Player, bool bSaveLevelName)
{
	if (Player == nullptr)
	{
		return;
	}

	UKismetSystemLibrary::PrintString(this,TEXT("전부 저장"));
	/*GameLoadType = EGameLoadType::RESPAWN;#1#
	
	if (bSaveLevelName)
	{
		SaveLevelName(Player);
	}
	
	//SaveTransform(Player);

	SaveAttribute(Player);

	SaveInventoryItem(Player);

	SaveFragment(Player);

	SaveEquipment(Player);

	SaveOrbMatrix(Player);

	SaveEquipEnhancement(Player);

	SavePotionEnhancement(Player);

	//SaveDeadMonster(Player);

	SaveAllQuickSlotState(Player);
}*/

void USoulLikeSaveGame::SavePlacementItemState(const FString& LevelName, const FString& SafeName)
{
	if (LevelName.IsEmpty())
	{
		return;
	}

	if (SafeName.IsEmpty())
	{
		return;
	}

	UKismetSystemLibrary::PrintString(this,LevelName + TEXT("레벨의 필드 아이템 획득 상황을 저장합니다 : ") + SafeName);

	if (!PickUppedItems.Contains(LevelName))
	{
		PickUppedItems.Add(LevelName);
	}

	PickUppedItems[LevelName].Add(SafeName);

	
	for (auto iter : PickUppedItems[LevelName].SafeNameList)
	{
		UKismetSystemLibrary::PrintString(this,TEXT("저장된 필드 아이템 : ") + iter);
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
		InventoryItem.Emplace(FItemSave(uniqueID, classPath, count));
	}
	UE_LOGFMT(LogSave,Log,"인벤토리에 추가된 아이템을 저장합니다 : {0}, {1} {2}",ItemInfo.GetItemInformation()->Item_Name,ItemInfo.ItemCount,uniqueID);

	UE_LOGFMT(LogSave,Log,"------------------저장된 배열--------------------------");
	for(auto iter : InventoryItem)
	{
		UE_LOGFMT(LogSave,Log,"{0}, {1}",iter.UniqueID,iter.ItemCount);
	}
	UE_LOGFMT(LogSave,Log,"-------------------------------------------------------");
}

void USoulLikeSaveGame::SaveUsedItem(const FInventoryItem& ItemInfo)
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
		UE_LOGFMT(LogSave,Log,"인벤토리에 사용된 아이템을 저장합니다 : {0}, {1} {2}",ItemInfo.GetItemInformation()->Item_Name,ItemInfo.ItemCount,uniqueID);
		
		InventoryItem.FindByPredicate([uniqueID](const FItemSave& Element)
		{
			return Element.UniqueID == uniqueID;
		})->ItemCount = ItemInfo.ItemCount;
		//장비 아이템인 경우
		if (UItemHelperLibrary::IsEquipment(ItemInfo))
		{
			EquippedItemID.AddUnique(uniqueID);

			if (UItemHelperLibrary::IsOrbCore(ItemInfo))
			{
				UE_LOGFMT(LogSave,Log,"인벤토리에 사용된 코어 아이템을 저장합니다 : {0}, {1} {2}",ItemInfo.GetItemInformation()->Item_Name,ItemInfo.ItemCount,uniqueID);
				CoreID = uniqueID;
				SaveOrbMatrix(ItemInfo.GetSpawndItemActor()->GetOwner<APlayerCharacter>());
				return;
			}

			if (UItemHelperLibrary::IsWeapon(ItemInfo))
			{
				if (ItemInfo.GetSpawndItemActor()->GetOwner<APlayerCharacter>()->GetInventoryComponent()->IsEquipped(
					ItemInfo.UniqueID))
				{
					UE_LOGFMT(LogSave,Log,"인벤토리에 사용된 무기 아이템을 저장합니다 : {0}, {1}",ItemInfo.GetItemInformation()->Item_Name,ItemInfo.ItemCount);
					WeaponID = uniqueID;
					return;
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

		if(index!=INDEX_NONE)
		{
			InventoryItem.RemoveAt(index);
		}
	}
}

void USoulLikeSaveGame::SaveRegisterQuickSlotState(UInventoryData* Data, int32 Index)
{
	if(Data->IsA<UItemData>())
	{
		auto data = Cast<UItemData>(Data);
		const auto& id = data->InventoryItem.UniqueID;
		//이미 등록된 아이디라면 제거하고 다시 등록합시다.
		for(auto iter = ItemQuickSlotMap.CreateIterator();iter;++iter)
		{
			if(iter.Value()==id)
			{
				iter.RemoveCurrent();
				break;
			}
		}

		ItemQuickSlotMap.Add(Index,id);
		return;
	}

	if(Data->IsA<UAbilityData>())
	{
		auto data = Cast<UAbilityData>(Data);
		const auto& tag = data->AbilityInformation.AbilityTag;

		//이미 등록된 아이디라면 제거하고 다시 등록합시다.
		for(auto iter = AbilityQuickSlotMap.CreateIterator();iter;++iter)
		{
			if(iter.Value().MatchesTagExact(tag))
			{
				iter.RemoveCurrent();
				break;
			}
		}

		AbilityQuickSlotMap.Add(Index,tag);
		return;
	}
}

void USoulLikeSaveGame::SaveUnRegisterQuickSlotState(UInventoryData* Data, int32 Index)
{
	if(Data==nullptr)
	{
		return;
	}
	
	if(Data->IsA<UItemData>())
	{
		auto data = Cast<UItemData>(Data);
		const auto& id = data->InventoryItem.UniqueID;

		//이미 등록된 아이디라면 제거
		for(auto iter = ItemQuickSlotMap.CreateIterator();iter;++iter)
		{
			if(iter.Value()==id)
			{
				iter.RemoveCurrent();
				break;
			}
		}
		
		return;
	}

	if(Data->IsA<UAbilityData>())
	{
		auto data = Cast<UAbilityData>(Data);
		const auto& tag = data->AbilityInformation.AbilityTag;

		//이미 등록된 아이디라면 제거
		for(auto iter = AbilityQuickSlotMap.CreateIterator();iter;++iter)
		{
			if(iter.Value().MatchesTagExact(tag))
			{
				iter.RemoveCurrent();
				break;
			}
		}

		return;
	}
}

void USoulLikeSaveGame::SaveAllQuickSlotState(APlayerCharacter* Player)
{
	if(Player)
	{
		if(Player->GetMainWidget())
		{
			const auto& consumeQuick = Player->GetMainWidget()->UMG_Equipment->GetAllConsumeQuickSlots();
			ItemQuickSlotMap.Empty();
			for(auto iter : consumeQuick)
			{
				if(auto widget = Cast<UQuickSlotButtonWidget>(iter))
				{
					const auto& id = widget->GetSlotItemUniqueID();
					const auto& index = widget->GetSlotIndex();
					if(id != FGuid())
					{
						ItemQuickSlotMap.Add(index,	id);
					}
				}
			}
		
		
			const auto& abilityQuick = Player->GetMainWidget()->UMG_Equipment->GetAllAbilityQuickSlots();
			for(auto iter : abilityQuick)
			{
				if(auto widget = Cast<UQuickSlotButtonWidget>(iter))
				{
					const auto& id = widget->GetSlotItemUniqueID();
					const auto& index = widget->GetSlotIndex();
					if(id != FGuid())
					{
						ItemQuickSlotMap.Add(index,	id);
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
	UE_LOGFMT(LogSave,Log,"장착 해제 아이템 저장 : {0} {1}",ItemInfo.GetItemInformation()->Item_Name,ItemInfo.UniqueID);
	
	//공통부분
	const auto& uniqueID = ItemInfo.UniqueID;
	EquippedItemID.Remove(uniqueID);

	//아이템 종류가 파편일 경우 따로 처리해야 합니다.
	if (UItemHelperLibrary::IsOrbFragment(ItemInfo))
	{
		//변경된 메트릭스 저장
		//UKismetSystemLibrary::PrintString(ItemInfo.GetSpawndItemActor(),TEXT("파편 장착 해제"));
		SaveOrbMatrix(ItemInfo.GetSpawndItemActor()->GetOwner<APlayerCharacter>());
		return;
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
	DeadState.DeadLocation = Player->GetActorLocation();
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
	if(Bonfire)
	{
		const auto& safeName = GetNameSafe(Bonfire);
		UE_LOGFMT(LogSave,Log,"화톳불 / 저장 : {0}",safeName);
		ActivateBonfire.AddUnique(safeName);
	}
}

void USoulLikeSaveGame::SaveTeleportBonfireInfo(const FBonfireInformation& BonfireInfo)
{
	TeleportBonfireInfo = BonfireInfo;
}

void USoulLikeSaveGame::SaveInventory(APlayerCharacter* Player)
{
	if(auto invenComp = Player->GetInventoryComponent())
	{
		const auto& items = invenComp->GetInventory();
		for(auto iter :items)
		{
			if(!InventoryItem.Contains(iter.Key))
			{
				//Fragments.Add(FItemSave(uniqueID, classPath, ItemInfo.ItemCount));
				const auto& classPath = iter.Value.GetItemInformation()->ItemActorObject->GetPathName();
				InventoryItem.Emplace(FItemSave(iter.Key,classPath,iter.Value.ItemCount));
			}else
			{
				InventoryItem.FindByKey(iter.Key)->ItemCount = iter.Value.ItemCount;
			}
			
		}
		
		const auto& frags = invenComp->GetFragments();
		for(auto iter :items)
		{
			if(!Fragments.Contains(iter.Key))
			{
				//Fragments.Add(FItemSave(uniqueID, classPath, ItemInfo.ItemCount));
				const auto& classPath = iter.Value.GetItemInformation()->ItemActorObject->GetPathName();
				Fragments.Emplace(FItemSave(iter.Key,classPath,iter.Value.ItemCount));
			}else
			{
				Fragments.FindByKey(iter.Key)->ItemCount = iter.Value.ItemCount;
			}
			
		}
	}
}

void USoulLikeSaveGame::SaveReadTutorialActor(ATutorialActor* TutorialActor)
{
	const auto& name = GetNameSafe(TutorialActor);
	if(!ReadTutorialActors.Contains(name))
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
