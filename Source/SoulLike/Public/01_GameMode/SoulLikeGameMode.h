// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "00_Character/03_Monster/BaseMonster.h"
#include "SoulLikeGameMode.generated.h"

USTRUCT(BlueprintType)
struct FMonsterRespawnInfo
{
	GENERATED_BODY()

	UPROPERTY()
	TWeakObjectPtr<class ABaseMonster> Monster;
	UPROPERTY()
	FString MonsterSafeName;
	UPROPERTY()
	TSubclassOf<class ABaseMonster> MonsterClass;
	UPROPERTY()
	FTransform RespawnTransform;

	FMonsterRespawnInfo()
	{
	}
 

	FMonsterRespawnInfo(class ABaseMonster* MonsterPtr, FString SafeName , TSubclassOf<class ABaseMonster> Class, FTransform Tr):Monster(MonsterPtr),MonsterSafeName(SafeName),MonsterClass(Class), RespawnTransform(Tr){ }

	bool operator==(const FMonsterRespawnInfo& Other) const
	{
		return Other.MonsterSafeName.Equals(MonsterSafeName);
	}


};

UCLASS(minimalapi)
class ASoulLikeGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
	//재스폰할 몬스터 정보
	//Key - UAID / Value- - 리스폰 정보
	TMap<FString, TArray<FMonsterRespawnInfo>> RespawnInfos;
	
public:
	ASoulLikeGameMode();
	virtual void Tick(float DeltaSeconds) override;
	
	//플레이어가 휴식하면 리스폰 시킬 몬스터로 추가합니다.
	void AddMonster(class ABaseMonster* BaseMonster, const FMonsterRespawnInfo& MonsterRespawnInfo);
	
	//플레이어가 휴식할 때, 필드 몬스터를 재 스폰하기 위해 호출됩니다.
	UFUNCTION()
	void RespawnMonsters(class APlayerCharacter* Player);

	const TMap<FString, TArray<FMonsterRespawnInfo>>& GetRespawnInfos(){return RespawnInfos;}

};


//이 게임모드를 사용중일때는 저장기능이 동작하지 않습니다.
UCLASS(minimalapi)
class ATESTGameMode : public AGameModeBase
{
	GENERATED_BODY()
};