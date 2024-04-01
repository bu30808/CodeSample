// Fill out your copyright notice in the Description page of Project Settings.


#include "98_GameInstance/SoulLikeInstance.h"

#include "MoviePlayer.h"
#include "Kismet/GameplayStatics.h"
#include "Logging/StructuredLog.h"
#include "WorldPartition/DataLayer/DataLayerAsset.h"

#include "00_Character/00_Player/PlayerCharacter.h"
#include "00_Character/00_Player/SoulTomb.h"

#include "00_Character/01_Component/InventoryComponent.h"
#include "00_Character/03_Monster/BaseMonster.h"

#include "00_Character/04_NPC/99_Component/BonfireComponent.h"

#include "01_GameMode/SoulLikeGameMode.h"
#include "02_Ability/01_Task/GameplayTask_LaunchEvent.h"
#include "03_Widget/01_Menu/00_Inventory/InventoryWidget.h"

#include "04_Item/ItemActor.h"
#include "04_Item/00_Consume/00_Potion/PotionItemActor.h"
#include "04_Item/01_Equipment/EquipmentItemActor.h"
#include "92_Tools/WorldStreamingSourceActor.h"
#include "93_SaveGame/SoulLikeSaveGame.h"
#include "96_Library/DataLayerHelperLibrary.h"
#include "96_Library/ItemHelperLibrary.h"
#include "WorldPartition/WorldPartitionRuntimeCell.h"
#include "WorldPartition/WorldPartitionSubsystem.h"

class UDataLayerSubsystem;
DEFINE_LOG_CATEGORY(LogInstance);

USoulLikeInstance::USoulLikeInstance()
{
	{
		static ConstructorHelpers::FClassFinder<UUserWidget> widget(TEXT(
			"/Script/UMGEditor.WidgetBlueprint'/Game/Blueprints/02_Widget/Loading/UMG_Loading.UMG_Loading_C'"));
		if (widget.Succeeded())
		{
			LoadingWidgetClass = widget.Class;
		}
	}

	{
		static ConstructorHelpers::FObjectFinder<UDataLayerAsset> layer(TEXT(
			"/Script/Engine.DataLayerAsset'/Game/DataLayer/Runtime/AlwaysLoadedLayer.AlwaysLoadedLayer'"));
		if (layer.Succeeded())
		{
			AlwaysActivatedLayer = layer.Object;
		}
	}

	{
		static ConstructorHelpers::FClassFinder<ASoulTomb> tomb(TEXT(
			"/Script/Engine.Blueprint'/Game/Blueprints/00_Character/06_SoulTomb/BP_SoulTomb.BP_SoulTomb_C'"));
		if (tomb.Succeeded())
		{
			SoulTombClass = tomb.Class;
		}
	}

	{
		static ConstructorHelpers::FClassFinder<AWorldStreamingSourceActor> streamingActor(TEXT(
			"/Script/Engine.Blueprint'/Game/Blueprints/08_Tool/BP_WorldStreamingActor.BP_WorldStreamingActor_C'"));
		if (streamingActor.Succeeded())
		{
			WorldStreamingSourceActorClass = streamingActor.Class;
		}
	}

	{
		static ConstructorHelpers::FObjectFinder<UDataTable> datalayer(TEXT(
			"/Script/Engine.DataTable'/Game/DataLayer/DT_DataLayer.DT_DataLayer'"));
		if (datalayer.Succeeded())
		{
			DataLayerTable = datalayer.Object;
		}
	}
}

void USoulLikeInstance::Init()
{
	Super::Init();

	FCoreUObjectDelegates::PreLoadMap.
		AddUObject(this, &USoulLikeInstance::PreLoadMapEvent);

	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &USoulLikeInstance::OnPostLoadMapWithWorldEvent);
}

void USoulLikeInstance::PreLoadMapEvent(const FString& LevelName)
{
	if (LoadingWidgetClass != nullptr)
	{
		if (auto widget =
				CreateWidget<UUserWidget>(UGameplayStatics::GetPlayerController(GetWorld(), 0),
				                          LoadingWidgetClass.Get());
			widget != nullptr)
		{
			UKismetSystemLibrary::PrintString(this,TEXT("로딩 스크린 생성"));
			FLoadingScreenAttributes LoadingScreen;
			LoadingScreen.bAutoCompleteWhenLoadingCompletes = false;
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

bool USoulLikeInstance::IsRespawn()
{
	if (auto instance = GetSaveGameInstance())
	{
		return instance->GameLoadType == EGameLoadType::RESPAWN || instance->GameLoadType ==
			EGameLoadType::TELEPORT_TO_LAST_SAVEPOINT;
	}

	return false;
}

void USoulLikeInstance::DeleteSaveFile(const FOnDeletedSaveFile& OnDeletedSaveFile,FString SlotName)
{
	if (UGameplayStatics::DoesSaveGameExist(SlotName, SaveIndex))
	{
		UGameplayStatics::DeleteGameInSlot(SlotName, SaveIndex);
		OnDeletedSaveFile.ExecuteIfBound();
		UE_LOG(LogTemp, Warning, TEXT("세이브 파일이 삭제되었습니다."));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("해당 세이브 파일이 존재하지 않습니다."));
	}
}

void USoulLikeInstance::LoadGame()
{
	if (!IsUseGameSave())
	{
		return;
	}

	if (CurrentPlayer)
	{
		if (bBlockSave)
		{
			return;
		}
		if (const auto instance = GetSaveGameInstance())
		{
			bBlockSave = true;
			/*
			LoadLayer();
			*/

			switch (instance->GameLoadType)
			{
			case EGameLoadType::NORMAL:
				NormalLoad();
				break;
			case EGameLoadType::RESPAWN:
			case EGameLoadType::TELEPORT_TO_LAST_SAVEPOINT:
				RespawnOrLastSavePointLoad();
				break;
			case EGameLoadType::TELEPORT_TO_OTHER_BONFIRE:
				TeleportToOtherBonfireLoad();
				break;
			}
		}
	}
}

void USoulLikeInstance::LoadCommon()
{
	LoadInventory();
	LoadAttribute(IsRespawn());
	LoadQuickSlotState();
}

void USoulLikeInstance::LoadLevel(const FString& LevelName)
{
	if (auto instance = GetSaveGameInstance())
	{
		const auto levelName = UGameplayStatics::GetCurrentLevelName(CurrentPlayer->GetWorld());
		if (levelName.Equals(LevelName) == false)
		{
			UGameplayStatics::OpenLevel(CurrentPlayer, FName(*LevelName));
		}
	}
}

void USoulLikeInstance::NormalLoad()
{
	LoadLevel(UGameplayStatics::GetCurrentLevelName(CurrentPlayer->GetWorld()));
	LoadCommon();
	MarkLoadFinish();
}

void USoulLikeInstance::RespawnOrLastSavePointLoad()
{
	if (auto instance = GetSaveGameInstance())
	{
		//레벨 복구
		if (instance->SavedMapName.IsEmpty() == false)
		{
			LoadLevel(instance->SavedMapName);
			instance->GameLoadType = EGameLoadType::TELEPORT_TO_LAST_SAVEPOINT;

			if (instance->SavedLastSavePoint.SavedBonfireName.IsEmpty() == false)
			{
				UE_LOGFMT(LogSave, Log, "불러온 마지막 세이브 포인트 정보 : {0} {1} {2}", instance->SavedLastSavePoint.LevelName,
				          instance->SavedLastSavePoint.SavedTransform.GetLocation().ToString(),
				          instance->SavedLastSavePoint.SavedBonfireName);
				CurrentPlayer->SetActorLocation(instance->SavedLastSavePoint.SavedTransform.GetLocation());
				LoadLayer();
				if (auto streamingActor = UDataLayerHelperLibrary::SpawnWorldStreamingSourceActor(CurrentPlayer))
				{
					streamingActor->OnStreamingComplete.AddUniqueDynamic(this, &USoulLikeInstance::LoadCommon);
					streamingActor->OnStreamingComplete.AddUniqueDynamic(this, &USoulLikeInstance::LoadSky);
					streamingActor->OnAfterStreamingComplete.AddUniqueDynamic(this, &USoulLikeInstance::MarkLoadFinish);
					streamingActor->OnAfterStreamingComplete.AddUniqueDynamic(this,&USoulLikeInstance::PotionReplenishment);
					streamingActor->StreamingStart(CurrentPlayer->GetActorLocation());
				}
			}
			else
			{
				LoadCommon();
				LoadSky();
				MarkLoadFinish();
			}
		}
	}
}

void USoulLikeInstance::LoadSkyFromBonfire()
{
	if (const auto gameLoadHandler = NewObject<UGameLoadHandler>(CurrentPlayer))
	{
		if (auto instance = GetSaveGameInstance())
		{
			gameLoadHandler->RestoreSky(CurrentPlayer, instance->TeleportBonfireInfo.SkyTime);
		}
		gameLoadHandler->MarkAsGarbage();
	}
}

void USoulLikeInstance::TeleportToOtherBonfireLoad()
{
	if (auto instance = GetSaveGameInstance())
	{
		UE_LOGFMT(LogSave, Log, "다음 화톳불로 이동합니다 : {0}", instance->TeleportBonfireInfo.LocationName.ToString());
		//레벨복구
		LoadLevel(instance->TeleportBonfireInfo.LevelName);
		if (auto streamingActor = UDataLayerHelperLibrary::SpawnWorldStreamingSourceActor(CurrentPlayer))
		{
			LoadBonfireLayer(instance->TeleportBonfireInfo.NeedToLoadLayer);
			streamingActor->OnStreamingComplete.AddUniqueDynamic(this, &USoulLikeInstance::LoadCommon);
			streamingActor->OnStreamingComplete.AddUniqueDynamic(this, &USoulLikeInstance::LoadSkyFromBonfire);
			streamingActor->OnAfterStreamingComplete.AddUniqueDynamic(this, &USoulLikeInstance::MarkLoadFinish);

			streamingActor->StreamingStart(instance->TeleportBonfireInfo.Location);
			instance->GameLoadType = EGameLoadType::TELEPORT_TO_LAST_SAVEPOINT;
		}
	}
}

void USoulLikeInstance::PotionReplenishment()
{
	UItemHelperLibrary::PotionReplenishment(CurrentPlayer);
}

void USoulLikeInstance::LoadBonfireLayer(const TSet<FName>& Layers)
{
	if (const auto gameLoadHandler = NewObject<UGameLoadHandler>(CurrentPlayer))
	{
		UE_LOGFMT(LogSave, Log, "화톳불 레이어 복구시도");
		for(auto iter : Layers)
		{
			UE_LOGFMT(LogSave,Log,"저장된 화톳불 레이어 : {0}",iter.ToString());
		}
		gameLoadHandler->RestoreDataLayer(CurrentPlayer, Layers, DataLayerTable);
		gameLoadHandler->MarkAsGarbage();
	}
}

void USoulLikeInstance::LoadBonfireLayer(const TArray<TSoftObjectPtr<UDataLayerAsset>>& Layers)
{
	if (const auto gameLoadHandler = NewObject<UGameLoadHandler>(CurrentPlayer))
	{
		UE_LOGFMT(LogSave, Log, "화톳불 레이어 복구시도");
		for(auto iter : Layers)
		{
			UE_LOGFMT(LogSave,Log,"저장된 화톳불 레이어 : {0}",iter.ToSoftObjectPath().ToString());
		}
		gameLoadHandler->RestoreDataLayer(CurrentPlayer, Layers);
		gameLoadHandler->MarkAsGarbage();
	}
}

void USoulLikeInstance::LoadLayer()
{
	if (const auto gameLoadHandler = NewObject<UGameLoadHandler>(CurrentPlayer))
	{
		if (auto instance = GetSaveGameInstance())
		{
			gameLoadHandler->RestoreDataLayer(CurrentPlayer, instance->ActivateLayersPath, DataLayerTable);
		}
		gameLoadHandler->MarkAsGarbage();
	}
}

void USoulLikeInstance::LoadLayers(const TArray<FName>& LayerRowNames)
{
	for(const auto& iter : LayerRowNames)
	{
		LoadLayer(iter);
	}
}

void USoulLikeInstance::LoadLayer(const FName& LayerRowName)
{
	if (const auto gameLoadHandler = NewObject<UGameLoadHandler>(CurrentPlayer))
	{
		gameLoadHandler->RestoreDataLayer(CurrentPlayer, LayerRowName, DataLayerTable);
		gameLoadHandler->MarkAsGarbage();
	}
}

void USoulLikeInstance::LoadSky()
{
	if (const auto gameLoadHandler = NewObject<UGameLoadHandler>(CurrentPlayer))
	{
		if (auto instance = GetSaveGameInstance())
		{
			gameLoadHandler->RestoreSky(CurrentPlayer, instance->CurrentSkyTime);
		}
		gameLoadHandler->MarkAsGarbage();
	}
}

AWorldStreamingSourceActor* USoulLikeInstance::CreateWorldStreamingActor()
{
	check(CurrentPlayer);

	if (WorldStreamingSourceActorClass)
	{
		FActorSpawnParameters spawnParam;
		spawnParam.Owner = CurrentPlayer;
		spawnParam.Instigator = CurrentPlayer;

		if (const auto streamingSource = GetWorld()->SpawnActor<AWorldStreamingSourceActor>(
			WorldStreamingSourceActorClass, spawnParam))
		{
			streamingSource->bShouldDestroy = true;
			return streamingSource;
		}
	}
	else
	{
		UE_LOGFMT(LogSave, Error, "스폰할 스트리밍 액터 클래스가 비었습니다.");
	}

	return nullptr;
}

void USoulLikeInstance::LoadInventory()
{
	if (const auto gameLoadHandler = NewObject<UGameLoadHandler>(CurrentPlayer))
	{
		if (auto instance = GetSaveGameInstance())
		{
			auto invenComp = CurrentPlayer->GetInventoryComponent();
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
		}
		gameLoadHandler->MarkAsGarbage();
	}
}

void USoulLikeInstance::LoadAttribute(bool bIsRespawn)
{
	if (const auto gameLoadHandler = NewObject<UGameLoadHandler>(CurrentPlayer))
	{
		if (auto instance = GetSaveGameInstance())
		{
			//어트리뷰트를 복구합니다.
			gameLoadHandler->LoadAttribute(CurrentPlayer->GetAttributeComponent(),
			                               instance->AttributesNotIncludeLevelUpPoint,
			                               instance->LevelUpPointAttributes, bIsRespawn);
		}
		gameLoadHandler->MarkAsGarbage();
	}
}

void USoulLikeInstance::LoadQuickSlotState()
{
	if (const auto gameLoadHandler = NewObject<UGameLoadHandler>(CurrentPlayer))
	{
		if (auto instance = GetSaveGameInstance())
		{
			//퀵슬롯 상태를 되돌립니다.
			gameLoadHandler->RestoreQuickSlotState(CurrentPlayer, instance->ItemQuickSlotMap,
			                                       instance->AbilityQuickSlotMap, instance->SelectedConsumeQuickSlot,
			                                       instance->SelectedAbilityQuickSlot, instance->EquippedQuickSlotItems,
			                                       instance->EquippedQuickSlotAbilities);
		}
		gameLoadHandler->MarkAsGarbage();
	}
}

void USoulLikeInstance::CreateSoulTomb(APlayerCharacter* Player)
{
	if (auto instance = GetSaveGameInstance())
	{
		if(instance->DeadState.bShouldCreateSoulTomb)
		{
			if (const auto gameLoadHandler = NewObject<UGameLoadHandler>(Player))
			{
				//사망 후 부활이라면, 영혼 무덤을 생성합니다.
				gameLoadHandler->CreateSoulTomb(Player, SoulTombClass, instance->DeadState);
				//부활처리가 끝났으니, 다음 로드시 부활처리를 하지 않도록 설정합니다.
				instance->DeadState.bShouldCreateSoulTomb = false;
				SaveGameInstanceToSlot(instance);
				gameLoadHandler->MarkAsGarbage();
			}
		}
	}
}

void USoulLikeInstance::MarkLoadFinish()
{
	bBlockSave = false;

	OnFinishLoadGame.Broadcast();
}


void USoulLikeInstance::OnUpdateMainAbilityQuickSlotEvent(const FGameplayTag& Tag, bool bRemove, int32 SlotIndex)
{
	if (!IsUseGameSave())
	{
		return;
	}

	auto instance = GetSaveGameInstance();
	instance->SaveSelectedAbilityQuickSlotIndex(SlotIndex);
	SaveGameInstanceToSlot(instance);
}

void USoulLikeInstance::OnUpdateMainConsumeQuickSlotEvent(const FInventoryItem& Item, bool bRemove, int32 SlotIndex)
{
	if (!IsUseGameSave())
	{
		return;
	}

	auto instance = GetSaveGameInstance();
	instance->SaveSelectedConsumeQuickSlotIndex(SlotIndex);
	SaveGameInstanceToSlot(instance);
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
		UE_LOGFMT(LogSave, Error, "다음 필드 아이템이 널이라 저장이 불가능합니다.");
		return;
	}

	if (auto instance = GetSaveGameInstance())
	{
		instance->SavePlacementItemState(Cast<APlayerCharacter>(GetBy), GotItemActor);
		SaveGameInstanceToSlot(instance);
	}
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
	instance->SaveSkyTime(player);
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

	if (auto player = Cast<APlayerCharacter>(UsedBy))
	{
		auto instance = GetSaveGameInstance();
		instance->SaveTransform(player);
		instance->SaveAttribute(player);
		instance->SaveUsedItem(player, ItemInfo);

		//장비아이템이라면,
		if (UItemHelperLibrary::IsEquipment(ItemInfo))
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
	instance->SaveDataLayer(Player);
	instance->SaveSkyTime(Player);

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
		if (Data->IsA<UItemData>())
		{
			instance->SaveRegisterItemQuickSlotState(Data, Index,
			                                         Player->GetInventoryComponent()->
			                                                 GetRegisteredConsumeQuickSlotItems());
		}

		if (Data->IsA<UAbilityData>())
		{
			instance->SaveRegisterAbilityQuickSlotState(Data, Index,
			                                            Player->GetInventoryComponent()->
			                                                    GetRegisteredQuickSlotAbilities());
		}

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
		if (Data->IsA<UItemData>())
		{
			instance->SaveRegisterItemQuickSlotState(Data, Index,
			                                         Player->GetInventoryComponent()->
			                                                 GetRegisteredConsumeQuickSlotItems());
		}

		if (Data->IsA<UAbilityData>())
		{
			instance->SaveRegisterAbilityQuickSlotState(Data, Index,
			                                            Player->GetInventoryComponent()->
			                                                    GetRegisteredQuickSlotAbilities());
		}
		SaveGameInstanceToSlot(instance);
	}
}

void USoulLikeInstance::OnDeadPlayer(APlayerCharacter* PlayerCharacter)
{
	auto instance = GetSaveGameInstance();
	instance->GameLoadType = EGameLoadType::RESPAWN;
	instance->SaveDeadState(PlayerCharacter);
	PlayerCharacter->GetAttributeComponent()->SetEXP(0);
	instance->SaveAttribute(PlayerCharacter);
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

bool USoulLikeInstance::IsActivatedBonfire(const FString& LevelName, const FString& SafeName)
{
	auto instance = GetSaveGameInstance();
	const FName& safeName = FName(SafeName);
	const auto& activatedBonfire = instance->ActivateBonfire;
	if (activatedBonfire.Contains(LevelName))
	{
		for (const auto& iter : activatedBonfire[LevelName].Information)
		{
			if (iter.ActorSafeName.IsEqual(safeName))
			{
				return true;
			}
		}
	}

	return false;
}

void USoulLikeInstance::SaveTeleportBonfire(const FBonfireInformation& BonfireInfo)
{
	UKismetSystemLibrary::PrintString(this,TEXT("다른 화톳불로 이동하기 위해서 저장합니다. : ") + BonfireInfo.LocationName.ToString());
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

void USoulLikeInstance::SaveKilledBoss(ABaseMonster* BaseMonster)
{
	if (!IsUseGameSave())
	{
		return;
	}

	if (const auto instance = GetSaveGameInstance())
	{
		const auto& bossTag = BaseMonster->GetMonsterTag();
		if (bossTag.IsValid())
		{
			instance->SaveKilledBoss(bossTag);
			SaveGameInstanceToSlot(instance);
		}
	}
}

void USoulLikeInstance::SaveSky(APlayerCharacter* Player)
{
	if (!IsUseGameSave())
	{
		return;
	}


	if (const auto instance = GetSaveGameInstance())
	{
		instance->SaveSkyTime(Player);
		SaveGameInstanceToSlot(instance);
	}
}

void USoulLikeInstance::SaveDataLayer(APlayerCharacter* Player)
{
	if (!IsUseGameSave())
	{
		return;
	}
	
	if (const auto instance = GetSaveGameInstance())
	{
		instance->SaveDataLayer(Player);
		SaveGameInstanceToSlot(instance);
	}
}

bool USoulLikeInstance::IsActivatedBonfire(const ABonfire* Bonfire)
{
	if (!IsUseGameSave())
	{
		return false;
	}

	if (const auto instance = GetSaveGameInstance())
	{
		const auto& levelName = UGameplayStatics::GetCurrentLevelName(Bonfire);
		const auto& safeName = GetNameSafe(Bonfire);

		const auto& bonfireList = instance->ActivateBonfire;
		if (bonfireList.Contains(levelName))
		{
			const auto& arr = bonfireList[levelName].Information.Array();
			return arr.Contains(safeName);
		}
	}

	return false;
}

bool USoulLikeInstance::IsAlreadyPickUppedItem(const AItemActor* Item)
{
	if (!IsUseGameSave())
	{
		return false;
	}

	if (const auto instance = GetSaveGameInstance())
	{
		const auto& levelName = UGameplayStatics::GetCurrentLevelName(Item);
		const auto& safeName = GetNameSafe(Item);

		const auto& pickUppedList = instance->PickUppedItems;
		if (pickUppedList.Contains(levelName))
		{
			const auto& arr = pickUppedList[levelName].Information.Array();
			if (!arr.Contains(safeName))
			{
				UE_LOGFMT(LogSave, Error, "다음 필드 아이템은 해당 레벨 아이템 목록에 저장된 적이 없습니다. : {0}", GetNameSafe(Item));
			}

			return arr.Contains(safeName);
		}
		UE_LOGFMT(LogSave, Error, "다음 필드 아이템은 해당 레벨에 저장된적이 없습니다 : {0}", GetNameSafe(Item));
	}

	return false;
}

bool USoulLikeInstance::IsAlreadyReadTutorial(const ATutorialActor* TutorialActor)
{
	if (!IsUseGameSave())
	{
		return false;
	}

	if (const auto instance = GetSaveGameInstance())
	{
		const auto& safeName = FName(GetNameSafe(TutorialActor));
		return instance->ReadTutorialActors.Contains(safeName);
	}

	return false;
}

bool USoulLikeInstance::IsBossKilled(const FGameplayTag& BossMonsterTag)
{
	if (!IsUseGameSave())
	{
		return false;
	}

	if (const auto instance = GetSaveGameInstance())
	{
		return instance->KilledBossMonstersTag.Contains(BossMonsterTag);
	}

	return false;
}
