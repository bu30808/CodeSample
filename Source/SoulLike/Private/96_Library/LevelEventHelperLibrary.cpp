// Fill out your copyright notice in the Description page of Project Settings.


#include "96_Library/LevelEventHelperLibrary.h"

#include "00_Character/00_Player/PlayerCharacter.h"
#include "00_Character/03_Monster/BaseMonster.h"
#include "96_Library/DataLayerHelperLibrary.h"
#include "96_Library/SaveGameHelperLibrary.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "WorldPartition/DataLayer/DataLayerAsset.h"
#include "WorldPartition/DataLayer/DataLayerSubsystem.h"

void AMainLevelScriptActor::OnDeadBossEvent_Implementation(AActor* Who, AActor* DeadBy)
{
	Cast<ABaseMonster>(Who)->OnDead.RemoveAll(this);
	Cast<ABaseMonster>(Who)->StopMusic(3.f);
	ULevelEventHelperLibrary::ActiveBlockingActors(BlockingActors, false);
}

bool AMainLevelScriptActor::IsAlreadySet(AActor* Player, const TArray<UDataLayerAsset*>& LayerToActive,
	const TArray<UDataLayerAsset*>& LayerToLoaded)
{
	if (auto manager = UDataLayerManager::GetDataLayerManager(Player))
	{
		//해당 레이어가 전부 활성화 상태인가요?
		for(auto iter : LayerToActive)
		{
			if(iter!=nullptr)
			{
				if(manager->GetDataLayerInstanceRuntimeState(manager->GetDataLayerInstanceFromAsset(iter)) != EDataLayerRuntimeState::Activated)
				{
					return false;	
				}
			}
		}
		//해당 레이어가 전부 로드됨 상태인가요?
		for(auto iter : LayerToLoaded)
		{
			if(iter!=nullptr)
			{
				if(manager->GetDataLayerInstanceRuntimeState(manager->GetDataLayerInstanceFromAsset(iter)) != EDataLayerRuntimeState::Loaded)
				{
					return false;	
				}
			}
		}

	}
	return true;
}

void AMainLevelScriptActor::ActiveLayerSetting(AActor* Player, TArray<UDataLayerAsset*> LayerToActive,
                                               TArray<UDataLayerAsset*> LayerToLoaded, const FK2OnLayerActivated& K2_OnLayerActivated)
{
	if (Player)
	{
		if(IsAlreadySet(Player,LayerToActive,LayerToLoaded)){
			return;
		}
		
		UE_LOGFMT(LogTemp, Log, "AMainLevelScriptActor : 레이어 설정을 시작합니다.");
		
		if (auto manager = UDataLayerManager::GetDataLayerManager(Player))
		{
			if (auto player = Cast<APlayerCharacter>(Player))
			{
				OnLayerActivated.BindLambda(
					[LayerToActive, Player, this, K2_OnLayerActivated](const UDataLayerInstance* DataLayer, EDataLayerRuntimeState State)
					{
						UE_LOGFMT(LogTemp,Warning,"OnLayerActivated");
						
						if (State == EDataLayerRuntimeState::Activated)
						{
							for (auto iter : LayerToActive)
							{
								if (DataLayer->GetDataLayerFullName().Equals(iter->GetPathName()))
								{
									EndActiveLayerSetting(Player);
									K2_OnLayerActivated.ExecuteIfBound();
									break;
								}
							}
						}
					});

				manager->OnDataLayerInstanceRuntimeStateChanged.AddUniqueDynamic(
					this, &AMainLevelScriptActor::OnActivateLayerEvent);

				player->ShowLoadingScreen(true);

				for (const auto iter : LayerToActive)
				{
					if(iter!=nullptr)
					{
						UE_LOGFMT(LogTemp, Log, "다음 레이어를 활성화 합니다. : {0}", iter->GetPathName());
						manager->SetDataLayerInstanceRuntimeState(manager->GetDataLayerInstanceFromAsset(iter), EDataLayerRuntimeState::Activated);
					}
				}

				for (const auto iter : LayerToLoaded)
				{
					if(iter!=nullptr)
					{
						UE_LOGFMT(LogTemp, Log, "다음 레이어를 활성화 합니다. : {0}", iter->GetPathName());
						manager->SetDataLayerInstanceRuntimeState(manager->GetDataLayerInstanceFromAsset(iter), EDataLayerRuntimeState::Loaded);
					}
				}
			}
		}
	}
}

void AMainLevelScriptActor::EndActiveLayerSetting(AActor* Player)
{
	if (auto player = Cast<APlayerCharacter>(Player))
	{
		/*player->ShowLoadingScreen(false);*/
		
		UE_LOGFMT(LogTemp, Log, "AMainLevelScriptActor : 레이어 설정이 끝났습니다.");

		if (auto manager = UDataLayerManager::GetDataLayerManager(Player))
		{
			manager->OnDataLayerInstanceRuntimeStateChanged.RemoveDynamic(
				this, &AMainLevelScriptActor::OnActivateLayerEvent);
		}
		
		OnLayerActivated.Unbind();
	}
}

void AMainLevelScriptActor::OnActivateLayerEvent(const UDataLayerInstance* DataLayer,
                                                             EDataLayerRuntimeState State)
{
	UE_LOGFMT(LogTemp,Warning,"OnActivateLayerEvent_Implementation");
	OnLayerActivated.ExecuteIfBound(DataLayer, State);
}


ABaseMonster* ULevelEventHelperLibrary::SpawnBoss(AMainLevelScriptActor* ScriptActor, AActor* Trigger,
                                                  AActor* OverlapActor,
                                                  TArray<AActor*> BlockingActors,
                                                  AActor* TargetPoint, TSubclassOf<ABaseMonster> BossToSpawn)
{
	//이미 처치한 우두머리면 아무것도 안 합니다.
	if (USaveGameHelperLibrary::IsBossKilled(OverlapActor, BossToSpawn.GetDefaultObject()->GetMonsterTag()))
	{
		return nullptr;
	}


	//트리거 콜리전을 끕니다.
	Trigger->SetActorEnableCollision(false);

	//못 나가게 길에 블록을 활성화 합니다.
	ActiveBlockingActors(BlockingActors, true);

	ScriptActor->BlockingActors = BlockingActors;

	FActorSpawnParameters spawnParams;
	spawnParams.Instigator = Cast<ABaseCharacter>(OverlapActor);
	spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	//스폰합니다.
	auto boss = Trigger->GetWorld()->SpawnActor<ABaseMonster>(BossToSpawn, TargetPoint->GetActorLocation(),
	                                                          TargetPoint->GetActorRotation(), spawnParams);
	boss->OnDead.AddUniqueDynamic(ScriptActor, &AMainLevelScriptActor::OnDeadBossEvent);
	boss->PlayMusic();

	return boss;
}

void ULevelEventHelperLibrary::ActiveBlockingActors(TArray<AActor*> BlockingActors, bool bActive)
{
	for (auto iter : BlockingActors)
	{
		UFunction* MyEventFunction = iter->FindFunction(TEXT("ActivateBlocking"));
		if (MyEventFunction != nullptr)
		{
			struct Params
			{
				bool Active;
			};

			Params p;
			p.Active = bActive;
			iter->ProcessEvent(MyEventFunction, &p);
		}
	}
}
