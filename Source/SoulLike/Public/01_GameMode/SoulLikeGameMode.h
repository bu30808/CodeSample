// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "93_SaveGame/SoulLikeSaveGame.h"
#include "SoulLikeGameMode.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogSoulLikeGameMode, Log, All);


UCLASS(minimalapi)
class ASoulLikeGameMode : public AGameModeBase
{
	GENERATED_BODY()

	/**
	 * 1.레벨 이름은 저장할 필요가 없습니다. 어차피 레벨을 이동한 경우 저장한 데이터가 필요가 없어지기 때문이죠.
	 * 2.레이어 이름도 저장할 필요가 없습니다. 굳이 레이어 가리지 않고 모든 대상이 필요하기 때문이죠.
	 */

	//재스폰할 몬스터 정보
	//Key - safename / Value- - 리스폰 정보 세트
	UPROPERTY(Transient)
	TMap<FName, FActorSave> RespawnInfos;

	//셀에 로드된 몬스터의 상태를 복구하기 위해 저장하는 변수
	//key - safename / value - 상태정보
	TMap<FName, FCharacterSave> TemporarySavedMonsterState;

public:
	ASoulLikeGameMode();

	//플레이어가 휴식하면 리스폰 시킬 몬스터로 추가합니다.
	void AddToRespawnMonster(ABaseMonster* BaseMonster);
	//리스폰할 목록에서 제거합니다.
	UFUNCTION()
	void RemoveFromRespawnMonster(const class ABaseMonster* BaseMonster);
	UFUNCTION()
	void RemoveFromRespawnMonsterWithSafeName(const FString& SafeName);
	//언로드 되었을 때, 사망상태가 아닌 경우 어트리뷰트 정보를 저장합니다.
	//다시 로드되었을 때, 체력상태등을 복구하기 위함입니다.
	UFUNCTION()
	void SaveMonsterAttributeWhenUnload(const ABaseMonster* BaseMonster);


	//플레이어가 휴식할 때, 필드 몬스터를 재 스폰하기 위해 호출됩니다.
	UFUNCTION()
	void RespawnMonsters(class APlayerCharacter* Player);
	//월드 파티션 환경에서 몬스터 사망시 휴식을 취하지 않아도 몬스터가 다시 로드될 때 되살아나는것을 막기 위해 상태저장을 위해 사용합니다.
	UFUNCTION()
	void OnDeadMonsterEvent(AActor* Who, AActor* DeadBy, EDeadReason DeadReason);

	//사망으로 기록된 몬스터의 상태를 사망으로 되돌립니다.
	void RestoreMonsterState(class ABaseMonster* BaseMonster);

	bool IsContainDeadList(const class ABaseMonster* BaseMonster);

	//DeadMonsters 변수 초기화
	UFUNCTION()
	void ClearTemporarySavedMonsterData(APlayerCharacter* Player);

	UFUNCTION(BlueprintCallable)
	const TMap<FName, FCharacterSave>& GetSavedMonsterState(){return TemporarySavedMonsterState;}

	void SaveMonsterState(class ABaseMonster* BaseMonster);
};

#if WITH_EDITOR
//이 게임모드를 사용중일때는 저장기능이 동작하지 않습니다.
UCLASS(minimalapi)
class ATESTGameMode : public AGameModeBase
{
	GENERATED_BODY()
};
#endif

