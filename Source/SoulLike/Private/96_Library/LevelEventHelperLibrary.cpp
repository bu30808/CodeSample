// Fill out your copyright notice in the Description page of Project Settings.


#include "96_Library/LevelEventHelperLibrary.h"

#include "00_Character/03_Monster/BaseMonster.h"
#include "96_Library/SaveGameHelperLibrary.h"

void AMainLevelScriptActor::OnDeadBossEvent(AActor* Who, AActor* DeadBy)
{
	Cast<ABaseMonster>(Who)->OnDead.RemoveAll(this);
	Cast<ABaseMonster>(Who)->StopMusic(3.f);
	ULevelEventHelperLibrary::ActiveBlockingActors(BlockingActors, false);
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
