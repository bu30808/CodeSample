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

void AMainLevelScriptActor::OnDeadBossEvent(AActor* Who, AActor* DeadBy)
{
	Cast<ABaseMonster>(Who)->OnDead.RemoveAll(this);
	Cast<ABaseMonster>(Who)->StopMusic(3.f);
	ULevelEventHelperLibrary::ActiveBlockingActors(BlockingActors, false);
}

bool AMainLevelScriptActor::IsAlreadySet(AActor* Player, const TArray<UDataLayerAsset*>& LayerToActive,
	const TArray<UDataLayerAsset*>& LayerToLoaded)
{
	if (UDataLayerSubsystem* dataLayerSubsystem = UDataLayerHelperLibrary::GetDataLayerSubsystem(Player))
	{
		//해당 레이어가 전부 활성화 상태인가요?
		for(auto iter : LayerToActive)
		{
			if(dataLayerSubsystem->GetDataLayerInstanceRuntimeState(iter) != EDataLayerRuntimeState::Activated)
			{
				return false;	
			}
		}
		//해당 레이어가 전부 로드됨 상태인가요?
		for(auto iter : LayerToLoaded)
		{
			if(dataLayerSubsystem->GetDataLayerInstanceRuntimeState(iter) != EDataLayerRuntimeState::Loaded)
			{
				return false;	
			}
		}

	}
	return true;
}

void AMainLevelScriptActor::ActiveLayerSetting(AActor* Player, TArray<UDataLayerAsset*> LayerToActive,
                                               TArray<UDataLayerAsset*> LayerToLoaded)
{
	if (Player)
	{
		if(IsAlreadySet(Player,LayerToActive,LayerToLoaded)){
			return;
		}
		
		UE_LOGFMT(LogTemp, Log, "AMainLevelScriptActor : 레이어 설정을 시작합니다.");
		
		if (UDataLayerSubsystem* dataLayerSubsystem = UDataLayerHelperLibrary::GetDataLayerSubsystem(Player))
		{
			if (auto player = Cast<APlayerCharacter>(Player))
			{
				OnNexusLayerActivated.BindLambda(
					[LayerToActive, Player, this](const UDataLayerInstance* DataLayer, EDataLayerRuntimeState State)
					{
						if (State == EDataLayerRuntimeState::Activated)
						{
							for (auto iter : LayerToActive)
							{
								if (DataLayer->GetDataLayerFullName().Equals(iter->GetPathName()))
								{
									EndActiveLayerSetting(Player);
									break;
								}
							}
						}
					});

				dataLayerSubsystem->OnDataLayerRuntimeStateChanged.AddUniqueDynamic(
					this, &AMainLevelScriptActor::OnChangedLayerStateForEnterNexus);

				player->DisableInput(Cast<APlayerController>(player->GetController()));
				player->GetCapsuleComponent()->SetEnableGravity(false);

				auto manager = UGameplayStatics::GetPlayerCameraManager(Player, 0);
				manager->StartCameraFade(0, 1.f, 1.f, FColor::Black, true, true);

				for (const auto iter : LayerToActive)
				{
					//UE_LOGFMT(LogTemp, Log, "활성화할 레이어의 경로입니다 : {0}", iter->GetPathName());
					dataLayerSubsystem->SetDataLayerInstanceRuntimeState(iter, EDataLayerRuntimeState::Activated);
				}

				for (const auto iter : LayerToLoaded)
				{
					dataLayerSubsystem->SetDataLayerInstanceRuntimeState(iter, EDataLayerRuntimeState::Loaded);
				}
			}
		}
	}
}

void AMainLevelScriptActor::EndActiveLayerSetting(AActor* Player)
{
	if (auto player = Cast<APlayerCharacter>(Player))
	{
		auto manager = UGameplayStatics::GetPlayerCameraManager(Player, 0);
		manager->StopCameraFade();
		player->GetCapsuleComponent()->SetEnableGravity(true);
		player->EnableInput(Cast<APlayerController>(player->GetController()));

		UE_LOGFMT(LogTemp, Log, "AMainLevelScriptActor : 레이어 설정이 끝났습니다.");

		if (auto subsystem = UDataLayerHelperLibrary::GetDataLayerSubsystem(this))
		{
			subsystem->OnDataLayerRuntimeStateChanged.RemoveDynamic(
				this, &AMainLevelScriptActor::OnChangedLayerStateForEnterNexus);
		}
		
		OnNexusLayerActivated.Unbind();
	}
}

void AMainLevelScriptActor::OnChangedLayerStateForEnterNexus(const UDataLayerInstance* DataLayer,
                                                             EDataLayerRuntimeState State)
{
	//UE_LOGFMT(LogTemp, Log, "변경된 레이어의 경로입니다 : {0}", DataLayer->GetDataLayerFullName());
	if(!OnNexusLayerActivated.ExecuteIfBound(DataLayer, State))
	{
		UE_LOGFMT(LogTemp, Error, "AMainLevelScriptActor : 레이어 설정 델리게이트 호출에 실패했습니다.");
	}
}


ABaseMonster* ULevelEventHelperLibrary::SpawnBoss(AMainLevelScriptActor* ScriptActor, AActor* Trigger,
                                                  AActor* OverlapActor,
                                                  TArray<AActor*> BlockingActors,
                                                  AActor* TargetPoint, TSubclassOf<ABaseMonster> BossToSpawn,
                                                  USoundBase* MusicToPlay)
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
	boss->PlayMusic(MusicToPlay);

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
