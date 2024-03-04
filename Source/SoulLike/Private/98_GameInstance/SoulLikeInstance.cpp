// Fill out your copyright notice in the Description page of Project Settings.


#include "98_GameInstance/SoulLikeInstance.h"

#include "MoviePlayer.h"
#include "00_Character/00_Player/PlayerCharacter.h"
#include "00_Character/01_Component/InventoryComponent.h"
#include "00_Character/03_Monster/BaseMonster.h"
#include "00_Character/04_NPC/99_Component/BonfireComponent.h"
#include "04_Item/ItemActor.h"
#include "04_Item/00_Consume/00_Potion/PotionItemActor.h"
#include "04_Item/01_Equipment/EquipmentItemActor.h"
#include "93_SaveGame/SoulLikeSaveGame.h"
#include "96_Library/ItemHelperLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Logging/StructuredLog.h"

USoulLikeInstance::USoulLikeInstance()
{
	{
		static ConstructorHelpers::FClassFinder<UUserWidget> widget(TEXT(
			"/Script/UMGEditor.WidgetBlueprint'/Game/Blueprints/02_Widget/Loading/UMG_Loading.UMG_Loading_C'"));
		if (widget.Succeeded())
		{
			LoadingWidgetObject = widget.Class;
		}
	}
}

void USoulLikeInstance::Init()
{
	Super::Init();

	FCoreUObjectDelegates::PreLoadMap.
		AddUObject(this, &USoulLikeInstance::PreLoadMapEvent);

	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &USoulLikeInstance::OnPostLoadMapWithWorldEvent);
	//PostLoadMapWithWorld
}

void USoulLikeInstance::PreLoadMapEvent(const FString& LevelName)
{
	if (LoadingWidgetObject != nullptr)
	{
		if (auto widget =
				CreateWidget<UUserWidget>(UGameplayStatics::GetPlayerController(GetWorld(), 0),
				                          LoadingWidgetObject.Get());
			widget != nullptr)
		{
			UKismetSystemLibrary::PrintString(this,TEXT("로딩 스크린 생성"));
			FLoadingScreenAttributes LoadingScreen;

			LoadingScreen.bAutoCompleteWhenLoadingCompletes = true;
			LoadingScreen.WidgetLoadingScreen = /*FLoadingScreenAttributes::NewTestLoadingScreenWidget()*/widget->
				TakeWidget();
			LoadingScreen.bWaitForManualStop = false;
			/*LoadingScreen.MinimumLoadingScreenDisplayTime = 50.f;
			LoadingScreen.MoviePaths.Add("Blurred Distance_1080p");*/
			GetMoviePlayer()->SetupLoadingScreen(LoadingScreen);
		}
	}
}

void USoulLikeInstance::OnPostLoadMapWithWorldEvent(UWorld* World)
{
	
}

bool USoulLikeInstance::IsUseGameSave() const
{
	//ASoulLikeGameMode게임모드를 사용중일때만 세이브 기능을 사용하도록 합니다.
	return UGameplayStatics::GetGameMode(GetWorld())->IsA<ASoulLikeGameMode>();
}

void USoulLikeInstance::LoadGame(APlayerCharacter* Player)
{
	if (!IsUseGameSave())
	{
		return;
	}

	if (Player)
	{
		if (bBlockSave)
		{
			return;
		}

		if (const auto instance = GetSaveGameInstance())
		{
			if (const auto gameLoadHandler = NewObject<UGameLoadHandler>(GetWorld()))
			{
				bBlockSave = true;

				//이미 확인한 듀토리얼 액터를 제거합니다.
				gameLoadHandler->RestoreTutorial(Player, instance->ReadTutorialActors);

				switch (instance->GameLoadType)
				{
				case EGameLoadType::NORMAL:
					{
						//레벨 복구
						gameLoadHandler->LoadLevel(Player->GetWorld(), instance->SavedMapName);
					}
					break;
				case EGameLoadType::TELEPORT_TO_LAST_SAVEPOINT:
				case EGameLoadType::RESPAWN:
					{
						UE_LOGFMT(LogSave, Log, "리스폰이거나, 마지막 세이브 포인트로 이동");
						//레벨 복구
						if (instance->SavedMapName.IsEmpty() == false)
						{
							UE_LOGFMT(LogSave, Log, "리스폰이거나, 마지막 세이브 포인트로 이동, 레벨 복구");
							gameLoadHandler->LoadLevel(Player->GetWorld(), instance->SavedMapName);
							instance->GameLoadType = EGameLoadType::TELEPORT_TO_LAST_SAVEPOINT;
							if (instance->SavedLastSavePoint.SavedBonfireName.IsEmpty() == false)
							{
								UE_LOGFMT(LogSave, Log, "리스폰이거나, 마지막 세이브 포인트로 이동, 마지막 세이브 포인트 지점으로 이동");
								/*Player->SetActorTransform(instance->SavedLastSavePoint.SavedTransform, false, nullptr,
								                          ETeleportType::TeleportPhysics);*/
								Player->K2_TeleportTo(instance->SavedLastSavePoint.SavedTransform.GetLocation(),instance->SavedLastSavePoint.SavedTransform.Rotator());
							}
						}
					}
					break;
				case EGameLoadType::TELEPORT_TO_OTHER_BONFIRE:
					{
						UE_LOGFMT(LogSave, Log, "다음 화톳불로 이동합니다 : {0}", instance->TeleportBonfireInfo.LocationName);
						//레벨복구
						gameLoadHandler->LoadLevel(Player->GetWorld(), instance->TeleportBonfireInfo.LevelName);
						instance->GameLoadType = EGameLoadType::TELEPORT_TO_LAST_SAVEPOINT;
						gameLoadHandler->TeleportToBonfire(Player, instance->TeleportBonfireInfo);
					}
					break;
				default: ;
				}


				
				auto invenComp = Player->GetInventoryComponent();
				//저장된 인벤토리를 복구합니다.
				gameLoadHandler->LoadInventory(invenComp, instance->InventoryItem);
				//파편을 복구합니다.
				gameLoadHandler->LoadFragment(invenComp, instance->Fragments);
				//코어를 찾아 장착합니다.
				gameLoadHandler->LoadCore(invenComp, instance->CoreID);
				//메트릭스를 불러옵니다.
				gameLoadHandler->LoadOrbMatrix(invenComp, instance->CoreID, instance->OrbMatrix);
				//장비 강화수치를 복구합니다.
				gameLoadHandler->RestoreEquipmentEnhancement(invenComp, instance->EquipEnhancementMap);
				//물약 강화수치를 복구합니다.
				gameLoadHandler->RestorePotionEnhancement(invenComp, instance->PotionEnhancementMap);

				//무기를 찾아 장착합니다.
				gameLoadHandler->LoadWeapon(invenComp, instance->WeaponID);
				//장착중이던 다른 장비들을 장착합니다.
				gameLoadHandler->LoadEquippedItem(invenComp, instance->EquippedItemID,
				                                  instance->EquippedWidgetIndexMap);
				//어트리뷰트를 복구합니다.
				gameLoadHandler->LoadAttribute(Player->GetAttributeComponent(),
				                               instance->AttributesNotIncludeLevelUpPoint,
				                               instance->LevelUpPointAttributes);

				//필드 아이템 상태를 복구합니다.
				gameLoadHandler->RestoreFieldItemState(Player, instance->PickUppedItems);

				//퀵슬롯 상태를 되돌립니다.
				gameLoadHandler->RestoreQuickSlotState(Player, instance->ItemQuickSlotMap,
				                                       instance->AbilityQuickSlotMap);

				//사망 후 부활이라면, 영혼 무덤을 생성합니다.
				gameLoadHandler->CreateSoulTomb(Player, instance->DeadState);

				//화톳불 상태를 복구합니다.
				gameLoadHandler->RestoreBonfire(Player, instance->ActivateBonfire);

				//회복약 재충전
				UItemHelperLibrary::PotionReplenishment(Player);

				//사용이 끝난후 오브젝트를 제거합니다.
				gameLoadHandler->ConditionalBeginDestroy();
				bBlockSave = false;
			}

			//부활처리가 끝났으니, 다음 로드시 부활처리를 하지 않도록 저장합니다.
			instance->DeadState.bShouldCreateSoulTomb = false;

			SaveGameInstanceToSlot(instance);
		}
	}
}

void USoulLikeInstance::ClearSave()
{
	if (!IsUseGameSave())
	{
		return;
	}

	auto instance = GetSaveGameInstance();
	instance->Clear();
	SaveGameInstanceToSlot(instance);
}

void USoulLikeInstance::TeleportToLastSavePoint(APlayerCharacter* Player)
{
	auto instance = GetSaveGameInstance();

	const auto& point = instance->SavedLastSavePoint;
	if (point.LevelName.IsEmpty())
	{
		//UKismetSystemLibrary::PrintString(this,TEXT("저장된 레벨 이름이 비어있는데요?"));
		UGameplayStatics::OpenLevel(Player->GetWorld(), *UGameplayStatics::GetCurrentLevelName(Player));
		return;
	}
	if (UGameplayStatics::GetCurrentLevelName(Player).Equals(point.LevelName) == false)
	{
		instance->GameLoadType = EGameLoadType::TELEPORT_TO_LAST_SAVEPOINT;
		SaveGameInstanceToSlot(instance);
		UGameplayStatics::OpenLevel(Player->GetWorld(), *point.LevelName);
	}
	else
	{
		Player->SetActorTransform(point.SavedTransform);
	}
}

void USoulLikeInstance::OnSaved(const FString& SlotName, const int32 UserIndex, bool bSucc)
{
	if (bSucc)
	{
		UKismetSystemLibrary::PrintString(this,TEXT("저장에 성공했습니다."));
	}
	else
	{
		UKismetSystemLibrary::PrintString(this,TEXT("저장에 실패했습니다."));
	}
}

/*
void USoulLikeInstance::ClearDeadMonster()
{
	if (auto instance = GetSaveGameInstance())
	{
		instance->ClearDeadMonster();
		SaveGameInstanceToSlot(instance);
	}
}
*/

USoulLikeSaveGame* USoulLikeInstance::GetSaveGameInstance(const FString& SlotName)
{
	if (SaveGameInstance == nullptr)
	{
		//이미 세이브 파일이 있는 경우
		if (UGameplayStatics::DoesSaveGameExist(SlotName, SaveIndex))
		{
			SaveGameInstance = Cast<USoulLikeSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, SaveIndex));
		}

		//세이브 파일이 없는 상태에서 새로 생성하는 경우
		if (SaveGameInstance == nullptr)
		{
			SaveGameInstance = Cast<USoulLikeSaveGame>(
				UGameplayStatics::CreateSaveGameObject(USoulLikeSaveGame::StaticClass()));
		}
	}

	return SaveGameInstance;
}

void USoulLikeInstance::SaveGameInstanceToSlot(USoulLikeSaveGame* SaveInstance,
                                               const FString& SlotName)
{
	if (bBlockSave)
	{
		return;
	}


	if (!IsUseGameSave())
	{
		return;
	}

	if (SaveInstance)
	{
		FAsyncSaveGameToSlotDelegate saveDelegate;
		saveDelegate.BindUObject(this, &USoulLikeInstance::OnSaved);

		UGameplayStatics::AsyncSaveGameToSlot(SaveInstance, SlotName, SaveIndex, saveDelegate);
	}
}

void USoulLikeInstance::OnSaveLevelItemPlacementStateEvent(ABaseCharacter* GetBy, AItemActor* GotItemActor)
{
	if (bBlockSave)
	{
		return;
	}

	if (GotItemActor == nullptr)
	{
		return;
	}
	auto player = Cast<APlayerCharacter>(GetBy);

	auto instance = GetSaveGameInstance();
	instance->SaveTransform(player);
	instance->SavePlacementItemState(UGameplayStatics::GetCurrentLevelName(GetWorld()),
	                                 GetNameSafe(GotItemActor));
	SaveGameInstanceToSlot(instance);
}

void USoulLikeInstance::OnAddItemEvent(ABaseCharacter* UsedBy, const FInventoryItem& ItemInfo,
                                       class AItemActor* GotItemActor)
{
	if (bBlockSave)
	{
		return;
	}

	if (GotItemActor == nullptr)
	{
		return;
	}

	auto player = Cast<APlayerCharacter>(UsedBy);
	auto instance = GetSaveGameInstance();
	instance->SaveTransform(player);
	instance->SaveAddedItem(ItemInfo);
	SaveGameInstanceToSlot(instance);
}

void USoulLikeInstance::OnUseItemEvent(ABaseCharacter* UsedBy, const FInventoryItem& ItemInfo)
{
	if (bBlockSave)
	{
		return;
	}

	if(auto player = Cast<APlayerCharacter>(UsedBy))
	{
		auto instance = GetSaveGameInstance();
		/*instance->SaveTransform(player);*/
		instance->SaveAttribute(player);
		instance->SaveUsedItem(ItemInfo);

		//장비아이템이라면,
		if(UItemHelperLibrary::IsEquipment(ItemInfo))
		{
			instance->SaveEquipSlot(player);
		}
	
		SaveGameInstanceToSlot(instance);
	}
}

void USoulLikeInstance::OnUpgradeEquipmentEvent(const FGuid& ID, AEquipmentItemActor* EquipmentItemActor)
{
	if (bBlockSave)
	{
		return;
	}
	if (EquipmentItemActor == nullptr)
	{
		return;
	}

	/*
	#if WITH_EDITOR
		auto player = Cast<APlayerCharacter>(EquipmentItemActor->GetOwner());
		const auto item = player->GetInventoryComponent()->GetInventoryItem(ID);
		UE_LOGFMT(LogSave,Log,"---------------------------------------");
		UE_LOGFMT(LogSave,Log,"강화 성공한 아이템을 저장합니다 : {0}, {1}",item.GetItemInformation()->Item_Name, item.UniqueID);
		for(auto iter : Cast<AEquipmentItemActor>(item.GetSpawndItemActor())->GetEnhancementInfo().EnhancementMap)
		{
			UE_LOGFMT(LogSave,Log,"{0} : {1}",StaticEnum<EAttributeType>()->GetValueAsString(iter.Key),iter.Value);
		}
		UE_LOGFMT(LogSave,Log,"---------------------------------------");
	#endif*/
	auto instance = GetSaveGameInstance();
	instance->SaveUpgradeEquipmentItem(ID, EquipmentItemActor->GetEnhancementInfo());
	SaveGameInstanceToSlot(instance);
}

void USoulLikeInstance::OnUpgradePotionEvent(const FGuid& ID, class APotionItemActor* PotionItemActor)
{
	if (bBlockSave)
	{
		return;
	}
	if (PotionItemActor == nullptr)
	{
		return;
	}

	auto player = Cast<APlayerCharacter>(PotionItemActor->GetOwner());

	auto instance = GetSaveGameInstance();
	instance->SaveTransform(player);
	instance->SaveUpgradePotionItem(ID, PotionItemActor->GetPotionEnhancement());
	SaveGameInstanceToSlot(instance);
}

void USoulLikeInstance::OnSlotOpenEvent(APlayerCharacter* Player, UOrbMatrix* Matrix)
{
	if (bBlockSave)
	{
		return;
	}
	if (Matrix == nullptr)
	{
		return;
	}

	auto instance = GetSaveGameInstance();
	instance->SaveTransform(Player);
	instance->SaveOrbMatrix(Matrix);
	SaveGameInstanceToSlot(instance);
}

void USoulLikeInstance::OnUnEquipItemEvent(ABaseCharacter* UnEquippedBy, const FInventoryItem& ItemInfo)
{
	if (bBlockSave)
	{
		return;
	}
	auto player = Cast<APlayerCharacter>(UnEquippedBy);
	auto instance = GetSaveGameInstance();
	instance->SaveTransform(player);
	instance->SaveAttribute(player);
	instance->SaveUnEquipItem(ItemInfo);
	SaveGameInstanceToSlot(instance);
}

void USoulLikeInstance::OnNPCBuyItemFromPlayerEvent(APlayerCharacter* InteractPlayer, const FGuid& ItemID)
{
	if (bBlockSave)
	{
		return;
	}
	auto player = Cast<APlayerCharacter>(InteractPlayer);
	auto instance = GetSaveGameInstance();

	instance->SaveTransform(player);
	instance->SaveAttribute(player);
	instance->UpdateSoldItemInSaveFile(player->GetInventoryComponent(), ItemID);
	SaveGameInstanceToSlot(instance);
}

void USoulLikeInstance::SaveWithLastSavePoint(APlayerCharacter* Player, UBonfireComponent* BonfireComponent)
{
	if (bBlockSave)
	{
		return;
	}

	auto instance = GetSaveGameInstance();
	instance->GameLoadType = EGameLoadType::TELEPORT_TO_LAST_SAVEPOINT;
	instance->SaveLastSavePoint(UGameplayStatics::GetCurrentLevelName(Player), Player->GetActorTransform(),
	                            BonfireComponent);
	instance->SaveInventory(Player);
	instance->SaveAttribute(Player);
	SaveGameInstanceToSlot(instance);
}

void USoulLikeInstance::OnRemoveItemEvent(ABaseCharacter* UsedBy, const FGuid& RemoveItemUniqueID)
{
	if (bBlockSave)
	{
		return;
	}
	auto player = Cast<APlayerCharacter>(UsedBy);

	auto instance = GetSaveGameInstance();
	instance->SaveTransform(player);
	instance->SaveRemovedItem(RemoveItemUniqueID);
	SaveGameInstanceToSlot(instance);
}

void USoulLikeInstance::OnRegisterQuickSlotItemOrAbility(APlayerCharacter* Player, UInventoryData* Data, int32 Index)
{
	if (bBlockSave)
	{
		return;
	}
	if (Index != INDEX_NONE)
	{
		auto instance = GetSaveGameInstance();
		instance->SaveRegisterQuickSlotState(Data, Index);
		SaveGameInstanceToSlot(instance);
	}
}

void USoulLikeInstance::OnUnRegisterQuickSlotItemOrAbility(APlayerCharacter* Player, UInventoryData* Data, int32 Index)
{
	if (bBlockSave)
	{
		return;
	}
	if (Index != INDEX_NONE)
	{
		auto instance = GetSaveGameInstance();
		instance->SaveUnRegisterQuickSlotState(Data, Index);
		SaveGameInstanceToSlot(instance);
	}
}

void USoulLikeInstance::OnDeadPlayer(APlayerCharacter* PlayerCharacter)
{
	auto instance = GetSaveGameInstance();
	instance->GameLoadType = EGameLoadType::RESPAWN;
	instance->SaveDeadState(PlayerCharacter);
	PlayerCharacter->GetAttributeComponent()->SetEXP(0);
	SaveGameInstanceToSlot(instance);
}

bool USoulLikeInstance::ExistSaveFile()
{
	return UGameplayStatics::DoesSaveGameExist("SaveFile", SaveIndex);
}

void USoulLikeInstance::SaveBonfire(ABonfire* Bonfire)
{
	auto instance = GetSaveGameInstance();
	instance->SaveActivateBonfire(Bonfire);
	SaveGameInstanceToSlot(instance);
}

bool USoulLikeInstance::IsActivatedBonfire(const FString& SafeName)
{
	auto instance = GetSaveGameInstance();
	UE_LOGFMT(LogTemp, Log, "화톳불 / 확인 : {0}, {1}", SafeName, instance->ActivateBonfire.Contains(SafeName));
	return instance->ActivateBonfire.Contains(SafeName);
}

void USoulLikeInstance::SaveTeleportBonfire(const FBonfireInformation& BonfireInfo)
{
	UKismetSystemLibrary::PrintString(this,TEXT("다른 화톳불로 이동하기 위해서 저장합니다. : ") + BonfireInfo.LocationName);
	auto instance = GetSaveGameInstance();
	instance->GameLoadType = EGameLoadType::TELEPORT_TO_OTHER_BONFIRE;
	instance->SaveTeleportBonfireInfo(BonfireInfo);
	SaveGameInstanceToSlot(instance);
}

void USoulLikeInstance::SaveTutorial(ATutorialActor* TutorialActor)
{
	auto instance = GetSaveGameInstance();
	instance->SaveReadTutorialActor(TutorialActor);
	SaveGameInstanceToSlot(instance);
}

void USoulLikeInstance::SaveTutorial(FGameplayTag TutorialTag)
{
	auto instance = GetSaveGameInstance();
	instance->SaveReadTutorial(TutorialTag);
	SaveGameInstanceToSlot(instance);
}

bool USoulLikeInstance::IsSkippedTutorial(FGameplayTag TutorialTag)
{
	auto instance = GetSaveGameInstance();
	return instance->IsSkippedTutorial(TutorialTag);
}

void USoulLikeInstance::SaveAttribute(APlayerCharacter* PlayerCharacter)
{
	auto instance = GetSaveGameInstance();
	instance->SaveAttribute(PlayerCharacter);
	SaveGameInstanceToSlot(instance);
}
