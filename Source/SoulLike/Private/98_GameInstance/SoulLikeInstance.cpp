// Fill out your copyright notice in the Description page of Project Settings.


#include "98_GameInstance/SoulLikeInstance.h"

#include "MoviePlayer.h"
#include "Kismet/GameplayStatics.h"
#include "Logging/StructuredLog.h"
#include "WorldPartition/DataLayer/DataLayerAsset.h"

#include "00_Character/00_Player/PlayerCharacter.h"
#include "00_Character/00_Player/SoulTomb.h"
#include "00_Character/00_Player/01_Component/BossHelperComponent.h"

#include "00_Character/01_Component/InventoryComponent.h"
#include "00_Character/03_Monster/BaseMonster.h"

#include "00_Character/04_NPC/99_Component/BonfireComponent.h"

#include "01_GameMode/SoulLikeGameMode.h"
#include "03_Widget/01_Menu/00_Inventory/InventoryWidget.h"

#include "04_Item/ItemActor.h"
#include "04_Item/00_Consume/00_Potion/PotionItemActor.h"
#include "04_Item/01_Equipment/EquipmentItemActor.h"
#include "92_Tools/WorldStreamingSourceActor.h"
#include "93_SaveGame/SoulLikeSaveGame.h"
#include "96_Library/DataLayerHelperLibrary.h"
#include "96_Library/ItemHelperLibrary.h"
#include "Components/CapsuleComponent.h"
#include "Sound/SoundClass.h"
#include "WorldPartition/DataLayer/DataLayerManager.h"

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

float USoulLikeInstance::GetBGMVolume()
{
	if(BGMClass){
		return BGMClass->Properties.Volume;
	}

	return 0.5f;
}

float USoulLikeInstance::GetSFXVolume()
{
	if(SFXClass){
		return SFXClass->Properties.Volume;
	}
	return 0.5f;
}

void USoulLikeInstance::SetBGMVolume(float vol)
{
	BGMClass->Properties.Volume = vol;
	SaveBGMVolume(vol);
}

void USoulLikeInstance::SetSFXVolume(float vol)
{
	SFXClass->Properties.Volume = vol;
	SaveSFXVolume(vol);
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
				CreateWidget<UUserWidget>(GetWorld(),LoadingWidgetClass.Get()))
		{
			UKismetSystemLibrary::PrintString(this,TEXT("로딩 스크린 생성"));
			FLoadingScreenAttributes LoadingScreen;
			LoadingScreen.bAutoCompleteWhenLoadingCompletes = false;
			LoadingScreen.WidgetLoadingScreen = /*FLoadingScreenAttributes::NewTestLoadingScreenWidget()*/widget->
				TakeWidget();
			LoadingScreen.bWaitForManualStop = true;
			LoadingScreen.bAllowEngineTick = true;
			LoadingScreen.MinimumLoadingScreenDisplayTime = 1.f;
			GetMoviePlayer()->SetupLoadingScreen(LoadingScreen);
		}else
		{
			UKismetSystemLibrary::PrintString(this,TEXT("위젯 포인터가 널입니다. 1 "));
		}
	}else
	{
		UKismetSystemLibrary::PrintString(this,TEXT("위젯 포인터가 널입니다.2 "));
	}
}

void USoulLikeInstance::OnPostLoadMapWithWorldEvent(UWorld* World)
{
	GetMoviePlayer()->StopMovie();
	UKismetSystemLibrary::PrintString(this,TEXT("로드 완료됨"));
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
		return instance->GameLoadType == EGameLoadType::DEAD_RESPAWN;
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
			CurrentPlayer->SetGravityAroundPlayer(false);
			UE_LOGFMT(LogSave,Warning,"세이브 불가능 상태로 전환합니다.");
			
			switch (instance->GameLoadType)
			{
			case EGameLoadType::NORMAL:
				NormalLoad();
				break;
			case EGameLoadType::DEAD_RESPAWN:
				DeadRespawn();
				break;
			case EGameLoadType::TELEPORT_TO_OTHER_BONFIRE:
				TeleportToOtherBonfireLoad();
				break;
			case EGameLoadType::RESTART:
				Restart();
				break;
			}
		}
	}
}

void USoulLikeInstance::RestoreFieldItemState()
{
	UE_LOGFMT(LogSave,Log,"필드 아이템 복구를 시도합니다.");
	if(CurrentPlayer!=nullptr){
		TArray<AActor*> items;
		UGameplayStatics::GetAllActorsOfClass(CurrentPlayer,AItemActor::StaticClass(),items);
		for(auto iter : items)
		{
			if(IsAlreadyPickUppedItem(Cast<AItemActor>(iter)))
			{
				iter->Destroy();
			}
		}
	}else{
		UE_LOGFMT(LogSave,Error,"필드 아이템 복구를 시도합니다. 플레이어가 널포인터라 불가능합니다.");
	}
}

void USoulLikeInstance::LoadCommon()
{
	RestoreFieldItemState();
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
			UE_LOGFMT(LogSave,Log,"저장된 레벨과 현재 레벨이 다릅니다. 저장된 레벨을 불러옵니다.");
			UGameplayStatics::OpenLevel(CurrentPlayer, FName(*LevelName));
		}
	}
}

void USoulLikeInstance::NormalLoad()
{
	if(auto gameMode = UGameplayStatics::GetGameMode(CurrentPlayer))
	{
		if (auto streamingActor = UDataLayerHelperLibrary::SpawnWorldStreamingSourceActor(CurrentPlayer))
		{
			LoadLayer();
			LoadInventory();
			LoadAttribute(IsRespawn());
			auto playerStart = gameMode->K2_FindPlayerStart(CurrentPlayer->GetController());
			DrawDebugPoint(CurrentPlayer->GetWorld(),playerStart->GetActorLocation(),50.f,FColor::Red,true);
			
			streamingActor->StreamingStart(playerStart->GetActorLocation());
			streamingActor->OnAfterStreamingComplete.AddUniqueDynamic(this, &USoulLikeInstance::TryLoadFinish);
		}
	}
	
}

void USoulLikeInstance::RestoreBeforeDead()
{
	//카메라 되돌림
	CurrentPlayer->ResetCamera();
	//디졸브 제거
	CurrentPlayer->StopDeadDissolve();
	//상태이상 제거
	CurrentPlayer->GetAttributeComponent()->ClearStatusEffect();
	//상태 되돌림
	CurrentPlayer->SetCharacterState(ECharacterState::NORMAL);
	CurrentPlayer->GetBossHelperComponent()->ResetTrigger();
	CurrentPlayer->GetBossHelperComponent()->ResetMistBlock();
	//체력 마력 스테 되돌림
	if(auto attComp = CurrentPlayer->GetAttributeComponent())
	{
		attComp->SetHP(attComp->GetMaxHP());
		attComp->SetMP(attComp->GetMaxMP());
		attComp->SetSP(attComp->GetMaxSP());
	}
	//콜리전 되돌림
	CurrentPlayer->GetMesh()->SetCollisionProfileName("PlayerMesh");
	CurrentPlayer->GetCapsuleComponent()->SetCollisionProfileName("Pawn");
	//애니메이션 모드 되돌림
	CurrentPlayer->GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
}

void USoulLikeInstance::DeadRespawn()
{
	UE_LOGFMT(LogSave,Warning,"DeadRespawn");
	if (auto instance = GetSaveGameInstance())
	{
		//레벨 복구
		if (instance->SavedMapName.IsEmpty() == false)
		{
			if (!FindObject<UPackage>(nullptr, *instance->SavedMapName))
			{
				LoadLevel(instance->SavedMapName);

				RestoreBeforeDead();
				if (instance->LastCheckpoint.OwnersSafeName.IsEmpty() == false)
				{
					if (auto streamingActor = UDataLayerHelperLibrary::SpawnWorldStreamingSourceActor(CurrentPlayer))
					{
						LoadSkyFromBonfire(instance->LastCheckpoint);
						LoadLayer();
						
						streamingActor->StreamingStart(instance->LastCheckpoint.Location);
						streamingActor->OnAfterStreamingComplete.AddUniqueDynamic(this, &USoulLikeInstance::TryLoadFinish);
					}
					
				}else{
					UE_LOGFMT(LogSave,Warning,"저장된 마지막 세이브 포인트가 없습니다. 기본 로드 합니다.");
					NormalLoad();
				}
			}
		}
	}
}

void USoulLikeInstance::Restart()
{
	UE_LOGFMT(LogSave,Log,"게임 재시작을 위한 데이터를 로드합니다.");
	if (auto instance = GetSaveGameInstance())
	{
		if (instance->SavedMapName.IsEmpty() == false)
		{
			//복구해야 할 것.
			//0.레벨
			LoadLevel(instance->SavedMapName);
			if (auto streamingActor = UDataLayerHelperLibrary::SpawnWorldStreamingSourceActor(CurrentPlayer))
			{
				LoadCommon();
				//2.하늘
				LoadSky();
				//1.레이어
				LoadLayer();
				streamingActor->StreamingStart(instance->PlayerData.Transform.GetLocation());
				streamingActor->OnAfterStreamingComplete.AddUniqueDynamic(this, &USoulLikeInstance::OnTryRestartLoadFinish);
								
			}
		}
	}
	
}

void USoulLikeInstance::LoadSkyFromBonfire(const FBonfireInformation& BonfireInformation) const
{
	if (const auto gameLoadHandler = NewObject<UGameLoadHandler>(CurrentPlayer))
	{
		gameLoadHandler->RestoreSky(CurrentPlayer, BonfireInformation);
		gameLoadHandler->MarkAsGarbage();
	}
}

void USoulLikeInstance::LoadSky()
{
	if (const auto gameLoadHandler = NewObject<UGameLoadHandler>(CurrentPlayer))
	{
		if (auto instance = GetSaveGameInstance())
		{
			gameLoadHandler->RestoreSky(CurrentPlayer,instance->WorldData);
		}
		gameLoadHandler->MarkAsGarbage();
	}
}

void USoulLikeInstance::LoadTransform()
{
	if (auto instance = GetSaveGameInstance())
	{
		CurrentPlayer->SetActorTransform(instance->PlayerData.Transform);
	}
}

void USoulLikeInstance::LoadLastMonsterState()
{
	if (const auto gameLoadHandler = NewObject<UGameLoadHandler>(CurrentPlayer))
	{
		if (auto instance = GetSaveGameInstance())
		{
			gameLoadHandler->RestoreMonsterState(CurrentPlayer,instance->WorldData.LastMonsterState);
		}
		gameLoadHandler->MarkAsGarbage();
	}
}


void USoulLikeInstance::TeleportToOtherBonfireLoad()
{
	UE_LOGFMT(LogSave,Warning,"TeleportToOtherBonfireLoad");
	if (auto instance = GetSaveGameInstance())
	{
		const auto& teleportBonfire = instance->WorldData.TeleportBonfireInfo;
		
		UE_LOGFMT(LogSave, Log, "다음 화톳불로 이동합니다 : {0}", teleportBonfire.LocationName.ToString());
		//레벨복구
		LoadLevel(teleportBonfire.LevelName);
		if (auto streamingActor = UDataLayerHelperLibrary::SpawnWorldStreamingSourceActor(CurrentPlayer))
		{
			LoadBonfireSetting(teleportBonfire);
			LoadCommon();
			
			streamingActor->OnAfterStreamingComplete.AddUniqueDynamic(this, &USoulLikeInstance::TryLoadFinish);
			streamingActor->StreamingStart(teleportBonfire.Location);

			instance->LastCheckpoint = teleportBonfire;
		}
	}
}

void USoulLikeInstance::LoadBonfireSetting(const FBonfireInformation& BonfireInformation)
{
	LoadBonfireLayer(BonfireInformation);
	LoadSkyFromBonfire(BonfireInformation);
	LoadCommon();
}

void USoulLikeInstance::LoadBonfireLayer(const FBonfireInformation& BonfireInformation)
{
	if (const auto gameLoadHandler = NewObject<UGameLoadHandler>(CurrentPlayer))
	{
		gameLoadHandler->RestoreDataLayer(CurrentPlayer, BonfireInformation,DataLayerTable);
		gameLoadHandler->MarkAsGarbage();
	}
}

void USoulLikeInstance::LoadLayer()
{
	if (const auto gameLoadHandler = NewObject<UGameLoadHandler>(CurrentPlayer))
	{
		if (auto instance = GetSaveGameInstance())
		{
			gameLoadHandler->RestoreDataLayer(CurrentPlayer, instance->WorldData.LastSavedLayerState, DataLayerTable);
		}
		gameLoadHandler->MarkAsGarbage();
	}
}

void USoulLikeInstance::LoadLayers(const TArray<TSoftObjectPtr<UDataLayerAsset>>& Layers) const
{
	if(auto subsystem =  UDataLayerManager::GetDataLayerManager<UWorld>(GetWorld()))
	{
		for(const auto& iter : Layers)
		{
			subsystem->SetDataLayerInstanceRuntimeState(subsystem->GetDataLayerInstance(iter.LoadSynchronous()),EDataLayerRuntimeState::Activated);
		}
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
			gameLoadHandler->LoadInventory(invenComp,instance->PlayerData.InventoryData);
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
			gameLoadHandler->LoadAttribute(CurrentPlayer->GetAttributeComponent(),instance->PlayerData,bIsRespawn);
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
			gameLoadHandler->RestoreQuickSlotState(CurrentPlayer,instance->PlayerData.InventoryData);
		}
		gameLoadHandler->MarkAsGarbage();
	}
}

void USoulLikeInstance::CreateSoulTomb(APlayerCharacter* Player)
{
	if (auto instance = GetSaveGameInstance())
	{
		if(instance->PlayerData.DeadState.bShouldCreateSoulTomb)
		{
			if (const auto gameLoadHandler = NewObject<UGameLoadHandler>(Player))
			{
				if(CreatedTomb!=nullptr)
				{
					UE_LOGFMT(LogSave,Log,"기존 무덤을 제거합니다.");
					CreatedTomb->Destroy();
				}
				
				UE_LOGFMT(LogSave,Log,"영혼 무덤을 생성합니다.");
				//사망 후 부활이라면, 영혼 무덤을 생성합니다.
				CreatedTomb = gameLoadHandler->CreateSoulTomb(Player, SoulTombClass, instance->PlayerData.DeadState);
				//부활처리가 끝났으니, 다음 로드시 부활처리를 하지 않도록 설정합니다.
				instance->PlayerData.DeadState.bShouldCreateSoulTomb = false;
				SaveGameInstanceToSlot(instance);
				gameLoadHandler->MarkAsGarbage();
			}
		}
	}
}

void USoulLikeInstance::OnTryRestartLoadFinish()
{
	//위치정보
	LoadTransform();
	//몬스터 상태정보
	LoadLastMonsterState();

	TryLoadFinish();
}

void USoulLikeInstance::TryLoadFinish()
{
	UE_LOGFMT(LogSave,Warning,"USoulLikeInstance::TryLoadFinish");
	
	if(GetWorld()->GetTimerManager().TimerExists(LoadFinishTimerHandle))
	{
		UE_LOGFMT(LogSave,Warning,"이미 로드 완료 타이머가 실행중입니다.");
		GetWorld()->GetTimerManager().ClearTimer(LoadFinishTimerHandle);
	}
	//n초있다가 실행하도록 하자. 바닥으로 꺼지는 문제 있다.
	GetWorld()->GetTimerManager().SetTimer(LoadFinishTimerHandle,this,&USoulLikeInstance::LoadFinish,1.f,false);
}

void USoulLikeInstance::LoadFinish()
{
	UE_LOGFMT(LogSave,Warning,"USoulLikeInstance::LoadFinish");
	UE_LOGFMT(LogSave,Warning,"세이브 가능 상태로 전환합니다.");
	bBlockSave = false;
	OnFinishLoadGame.Broadcast();
}


void USoulLikeInstance::OnAddItemQuickSlotEvent(const UItemData* Data, int32 SlotIndex)
{
	if (!IsUseGameSave())
	{
		return;
	}

	auto instance = GetSaveGameInstance();
	instance->SaveAddItemQuickSlot(Data,SlotIndex);
	SaveGameInstanceToSlot(instance);
}

void USoulLikeInstance::OnRemoveItemQuickSlotEvent(const UItemData* Data, int32 SlotIndex)
{
	if (!IsUseGameSave())
	{
		return;
	}

	auto instance = GetSaveGameInstance();
	instance->SaveRemovedItemQuickSlot(Data,SlotIndex);
	SaveGameInstanceToSlot(instance);
}

void USoulLikeInstance::OnAddAbilityQuickSlotEvent(const TArray<FGameplayTag>& AbilityQuickSlotTags)
{
	if (!IsUseGameSave())
	{
		return;
	}

	auto instance = GetSaveGameInstance();
	instance->SaveAbilityQuickSlot(AbilityQuickSlotTags);
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

	const auto& point = instance->LastCheckpoint;
	if (point.LevelName.IsEmpty())
	{
		//UKismetSystemLibrary::PrintString(this,TEXT("저장된 레벨 이름이 비어있는데요?"));
		UGameplayStatics::OpenLevel(Player->GetWorld(), *UGameplayStatics::GetCurrentLevelName(Player));
		return;
	}
	if (UGameplayStatics::GetCurrentLevelName(Player).Equals(point.LevelName) == false)
	{
		instance->GameLoadType = EGameLoadType::DEAD_RESPAWN;
		SaveGameInstanceToSlot(instance);
		UGameplayStatics::OpenLevel(Player->GetWorld(), *point.LevelName);
	}
	else
	{
		Player->SetActorLocation(point.Location);
	}
}

void USoulLikeInstance::SaveWhenEndPlay(APlayerCharacter* PlayerCharacter)
{

	if (!IsUseGameSave())
	{
		return;
	}
	
	auto instance = GetSaveGameInstance();
	instance->SaveWhenEndPlay(PlayerCharacter, DataLayerTable);
	SaveGameInstanceToSlot(instance);

	UE_LOGFMT(LogInstance,Warning,"게임 종료시 저장이 완료되었습니다.");
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
		UE_LOGFMT(LogSave,Warning,"저장 불가능한 상태입니다.");
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

USoulLikeOptionSaveGame* USoulLikeInstance::GetOptionSaveGameInstance(const FString& SlotName)
{
	if (OptionSaveGameInstance == nullptr)
	{
		//이미 세이브 파일이 있는 경우
		if (UGameplayStatics::DoesSaveGameExist(SlotName, SaveIndex))
		{
			OptionSaveGameInstance = Cast<USoulLikeOptionSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, SaveIndex));
		}

		//세이브 파일이 없는 상태에서 새로 생성하는 경우
		if (OptionSaveGameInstance == nullptr)
		{
			OptionSaveGameInstance = Cast<USoulLikeOptionSaveGame>(
				UGameplayStatics::CreateSaveGameObject(USoulLikeOptionSaveGame::StaticClass()));
		}
	}

	return OptionSaveGameInstance;
}

void USoulLikeInstance::SaveGameInstanceToSlot(USoulLikeOptionSaveGame* SaveInstance, const FString& SlotName)
{
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

void USoulLikeInstance::OnAddItemEvent(ABaseCharacter* GetBy, const FInventoryItem& ItemInfo,
                                       class AItemActor* GotItemActor)
{
	UE_LOGFMT(LogSave,Log,"{0} {1} : 추가된 아이템정보를 세이브파일에 저장합니다 : {2}",__FUNCTION__,__LINE__,ItemInfo.GetItemInformation()->Item_Name.ToString());
	if (bBlockSave)
	{
		UE_LOGFMT(LogSave,Log,"{0} {1} : 세이브 금지 상태라 저장할 수 없습니다.",__FUNCTION__,__LINE__);
		return;
	}

	if (GotItemActor == nullptr)
	{
		return;
	}

	auto player = Cast<APlayerCharacter>(GetBy);
	auto instance = GetSaveGameInstance();
	instance->SaveAddedItem(ItemInfo);
	SaveGameInstanceToSlot(instance);
}

void USoulLikeInstance::OnAddAbilityItemEvent(ABaseCharacter* GetBy, const FAbilityInformation& AbilityItemInfo,
	AItemActor* GotItemActor)
{
	UE_LOGFMT(LogSave,Log,"{0} {1} : 추가된 어빌리티 아이템정보를 세이브파일에 저장합니다 : {2}",__FUNCTION__,__LINE__,AbilityItemInfo.AbilityName.ToString());
	if (bBlockSave)
	{
		UE_LOGFMT(LogSave,Log,"{0} {1} : 세이브 금지 상태라 저장할 수 없습니다.",__FUNCTION__,__LINE__);
		return;
	}
		
	auto player = Cast<APlayerCharacter>(GetBy);
	auto instance = GetSaveGameInstance();
	instance->SaveAddedAbility(AbilityItemInfo);
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

void USoulLikeInstance::SaveLastCheckpoint(APlayerCharacter* Player, UBonfireComponent* BonfireComponent)
{
	if (bBlockSave)
	{
		return;
	}

	UE_LOGFMT(LogSave,Log,"SaveWithLastSavePoint");
	
	auto instance = GetSaveGameInstance();
	instance->GameLoadType = EGameLoadType::DEAD_RESPAWN;
	instance->SaveLastCheckpoint(BonfireComponent->GetOwner<ABonfire>()->GetBonfireInformation());
	/*instance->SaveInventory(Player);
	instance->SaveAttribute(Player);
	instance->SaveSky(Player);
	instance->SaveDataLayer(Player,DataLayerTable);
	*/
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


void USoulLikeInstance::OnDeadPlayer(APlayerCharacter* Player)
{
	if(auto instance = GetSaveGameInstance())
	{
		UE_LOGFMT(LogSave,Log,"사망상태 저장");
		instance->GameLoadType = EGameLoadType::DEAD_RESPAWN;
		instance->SaveDeadState(Player);
		if(auto attComp = Player->GetAttributeComponent())
		{
			auto curExp = attComp->GetEXP();
			attComp->SetEXP(0);
			attComp->OnUpdateExp.Broadcast(-curExp);
		}
		instance->SaveExp(Player->GetAttributeComponent()->GetEXPAttribute());
	
		SaveGameInstanceToSlot(instance);
	}
}

bool USoulLikeInstance::ExistSaveFile(FString SlotName)
{
	return UGameplayStatics::DoesSaveGameExist(SlotName, SaveIndex);
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
	const auto& activatedBonfire = instance->WorldData.ActivateBonfire;
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

void USoulLikeInstance::SaveAttributeExp(const FAttribute& ExpAttribute)
{
	if (!IsUseGameSave())
	{
		return;
	}

	if (const auto instance = GetSaveGameInstance())
	{
		instance->SaveExp(ExpAttribute);
		SaveGameInstanceToSlot(instance);
	}
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
		instance->SaveSky(Player);
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
		instance->SaveDataLayer(Player,DataLayerTable);
		SaveGameInstanceToSlot(instance);
	}
}

void USoulLikeInstance::SaveChest(AChest* Chest, bool bEaredChestItem)
{
	if (!IsUseGameSave())
	{
		return;
	}

	
	if(Chest)
	{
		if (const auto instance = GetSaveGameInstance())
		{
			instance->SaveChest(Chest,bEaredChestItem);
			SaveGameInstanceToSlot(instance);
		}
	}
}

void USoulLikeInstance::SaveNPCMet(ANPCBase* NPC)
{
	if (!IsUseGameSave())
	{
		return;
	}

	if (const auto instance = GetSaveGameInstance())
	{
		instance->SaveNPC(NPC, false);
		SaveGameInstanceToSlot(instance);
	}
}

void USoulLikeInstance::SaveNPCJoin(ANPCBase* NPC)
{
	if (!IsUseGameSave())
	{
		return;
	}

	if (const auto instance = GetSaveGameInstance())
	{
		instance->SaveNPC(NPC, false);
		SaveGameInstanceToSlot(instance);
	}
}

void USoulLikeInstance::SaveNPCDestoryed(ANPCBase* NPC)
{
	if (!IsUseGameSave())
	{
		return;
	}

	if (const auto instance = GetSaveGameInstance())
	{
		instance->SaveNPC(NPC, true);
		SaveGameInstanceToSlot(instance);
	}
}

void USoulLikeInstance::OnSellItemToPlayerEvent(APlayerCharacter* InteractPlayer, ANPCBase* Seller,
                                                const FMerchandiseItem& MerchandiseItem)
{
	if (!IsUseGameSave())
	{
		return;
	}

	if (const auto instance = GetSaveGameInstance())
	{
		instance->SaveMerchantItem(Seller,MerchandiseItem);
		SaveGameInstanceToSlot(instance);
	}
}

void USoulLikeInstance::OnSellAbilityToPlayerEvent(APlayerCharacter* InteractPlayer, ANPCBase* Seller,
                                                   const FMerchandiseAbility& MerchandiseAbility)
{
	if (!IsUseGameSave())
	{
		return;
	}

	if (const auto instance = GetSaveGameInstance())
	{
		instance->SaveMerchantAbility(Seller,MerchandiseAbility);
		SaveGameInstanceToSlot(instance);
	}
}

void USoulLikeInstance::LoadNPCState(ANPCBase* NPC)
{
	if (!IsUseGameSave())
	{
		return;
	}

	if (const auto instance = GetSaveGameInstance())
	{
		instance->LoadNPC(NPC);
	}
}

TMap<FGameplayTag, FMerchandiseItem> USoulLikeInstance::LoadNPCSellItemState(const FGameplayTag& NPCTag)
{
	if (!IsUseGameSave())
	{
		return TMap<FGameplayTag, FMerchandiseItem>();
	}
	
	if (const auto instance = GetSaveGameInstance())
	{
		if(instance->WorldData.NPCState.Contains(NPCTag))
		{
			return instance->WorldData.NPCState[NPCTag].SellItemState;
		}
	}

	return TMap<FGameplayTag, FMerchandiseItem>();
	
}

TMap<FGameplayTag, FMerchandiseAbility> USoulLikeInstance::LoadNPCSellAbilityState(const FGameplayTag& NPCTag)
{
	if (!IsUseGameSave())
	{
		return TMap<FGameplayTag, FMerchandiseAbility>();
	}
	
	if (const auto instance = GetSaveGameInstance())
	{
		if(instance->WorldData.NPCState.Contains(NPCTag))
		{
			return instance->WorldData.NPCState[NPCTag].SellAbilityState;
		}
	}

	return TMap<FGameplayTag, FMerchandiseAbility>();
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

		const auto& bonfireList = instance->WorldData.ActivateBonfire;
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

		const auto& pickUppedList = instance->WorldData.PickUppedItems;
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
		return instance->WorldData.KilledBossMonstersTag.Contains(BossMonsterTag);
	}

	return false;
}

bool USoulLikeInstance::IsOpenedChest(const AChest* Chest)
{
	if (!IsUseGameSave())
	{
		return false;
	}

	if (const auto instance = GetSaveGameInstance())
	{
		const auto& levelName = UGameplayStatics::GetCurrentLevelName(Chest);
		const auto& safeName = GetNameSafe(Chest);
		const auto& openedChest = instance->WorldData.OpenedChests;
		if(openedChest.Contains(levelName))
		{
			const auto& arr = openedChest[levelName].Information.Array();
			if(arr.Contains(safeName))
			{
				return true;
			}
		}
		
	}

	return false;
}

bool USoulLikeInstance::IsAlreadyGetChestItem(AChest* Chest)
{
	if (!IsUseGameSave())
	{
		return false;
	}

	if (const auto instance = GetSaveGameInstance())
	{
		const auto& levelName = UGameplayStatics::GetCurrentLevelName(Chest);
		const auto& safeName = GetNameSafe(Chest);
		
		const auto& openedChest = instance->WorldData.EarnedChestItems;
		if(openedChest.Contains(levelName))
		{
			const auto& arr = openedChest[levelName].Information.Array();
			if(arr.Contains(safeName))
			{
				return true;
			}
		}
		
	}

	return false;
}

FGameOption USoulLikeInstance::LoadGameOption(bool& bSucc)
{
	if (IsUseGameSave())
	{
		if (const auto instance = GetOptionSaveGameInstance())
		{
			bSucc = true;
			return instance->GameOption;
		}
	}

	bSucc = false;
	return FGameOption();
}

void USoulLikeInstance::SaveBGMVolume(float Vol)
{
	if (IsUseGameSave())
	{
		if (const auto instance = GetOptionSaveGameInstance())
		{
			instance->GameOption.BGMVolume = Vol;
			SaveGameInstanceToSlot(instance);
		}
	}
}

void USoulLikeInstance::SaveSFXVolume(float Vol)
{
	if (IsUseGameSave())
	{
		if (const auto instance = GetOptionSaveGameInstance())
		{
			instance->GameOption.SFXVolume = Vol;
			SaveGameInstanceToSlot(instance);
		}
	}
}

void USoulLikeInstance::SaveTextureStreamingPool(int32 TextureStreamingPool)
{
	if (IsUseGameSave())
	{
		if (const auto instance = GetOptionSaveGameInstance())
		{
			instance->GameOption.TexturePoolSize = TextureStreamingPool;
			SaveGameInstanceToSlot(instance);
		}
	}
}


