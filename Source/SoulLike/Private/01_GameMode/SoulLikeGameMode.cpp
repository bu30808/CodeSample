// Copyright Epic Games, Inc. All Rights Reserved.


#include "01_GameMode/SoulLikeGameMode.h"

#include "00_Character/03_Monster/BaseMonster.h"
#include "93_SaveGame/SoulLikeSaveGame.h"
#include "98_GameInstance/SoulLikeInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Logging/StructuredLog.h"
#include "UObject/ConstructorHelpers.h"

ASoulLikeGameMode::ASoulLikeGameMode()
{
	/*// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}*/
}

void ASoulLikeGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ASoulLikeGameMode::AddMonster(ABaseMonster* BaseMonster,
                                   const FMonsterRespawnInfo& MonsterRespawnInfo)
{
	const auto& safeName = GetNameSafe(BaseMonster);
	const auto& mapName = UGameplayStatics::GetCurrentLevelName(BaseMonster->GetWorld());

	if (!RespawnInfos.Contains(mapName))
	{
		RespawnInfos.Add(mapName, TArray<FMonsterRespawnInfo>());
	}

	UE_LOGFMT(LogTemp, Log, "몬스터 상태 복구 / 레벨 몬스터 저장 {0}", safeName);
	if(RespawnInfos[mapName].Contains(MonsterRespawnInfo))
	{
		const auto index = RespawnInfos[mapName].Find(MonsterRespawnInfo);
		if(index!=INDEX_NONE)
		{
			RespawnInfos[mapName][index].Monster = BaseMonster;
		}

		return;
	}
	RespawnInfos[mapName].AddUnique(MonsterRespawnInfo);
	
}


void ASoulLikeGameMode::RespawnMonsters(class APlayerCharacter* Player)
{
	UE_LOGFMT(LogTemp, Log, "몬스터 상태 복구 / RespawnMonsters");
	const auto& mapName =UGameplayStatics::GetCurrentLevelName(Player->GetWorld());

	if (RespawnInfos.Contains(mapName))
	{

		TArray<FMonsterRespawnInfo> respawnInfo;
		
		const auto& monstersInfo = RespawnInfos[mapName];
		//스폰되어 저장된 친구들중에, 사망상태이거나 지워져서 유효하지 않은 친구들의 정보를 가져옵니다.
		for (auto i = monstersInfo.Num() - 1; i >= 0; i--)
		{
			const auto& info = monstersInfo[i];
			//유효하지 않을 때
			if(info.Monster->IsValidLowLevel()==false)
			{
				UKismetSystemLibrary::PrintString(this,TEXT("저장된 대상이 유효하지 않으니, 재 스폰 목록에 추가합니다. 기존 아이디 : ")+info.MonsterSafeName);
				//저장한 뒤, 아래에서 재 스폰해줍니다.
				respawnInfo.AddUnique(info);
			}else
			{
				//사망상태인가 체크합니다
				if(info.Monster->IsDead() == false)
				{
					//사망상태인체로 필드에 남아있는 상태라면, 그냥 상태복구만 해줍니다.
					info.Monster->ResetMonster(info.RespawnTransform);
				}else
				{
					//이름을 바꾸고 제거합니다. 이후 저장해서 아래에서 재 스폰 합니다.
					const auto& tempName = info.Monster->GetName() + TEXT("Destroyed");
					info.Monster->Rename(*tempName);
					UKismetSystemLibrary::PrintString(this,TEXT("사망한 대상이니, 제거하고, 재 스폰 목록에 추가합니다. 변경한 이름 : ")+info.Monster->GetName());
					info.Monster->Destroy();
					respawnInfo.AddUnique(info);
				}
			}
			
		}


		//사망하거나 유효하지 않은 몬스터 재 스폰
		for(const auto& iter : respawnInfo)
		{
			FActorSpawnParameters spawnParams;
			spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			spawnParams.Name = *iter.MonsterSafeName;

			if (const auto spawnMonster = GetWorld()->SpawnActor<ABaseMonster>(
				iter.MonsterClass, iter.RespawnTransform, spawnParams))
			{
				spawnMonster->SetActorTransform(iter.RespawnTransform);
			}
		}
	}
}
