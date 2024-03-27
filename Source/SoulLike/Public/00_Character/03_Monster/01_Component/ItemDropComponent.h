// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "ItemDropComponent.generated.h"


USTRUCT(BlueprintType)
struct FDropItem : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TSoftClassPtr<class AItemActor> DropItem = nullptr;
	UPROPERTY(EditAnywhere, meta=(ClampMin=0, ClampMax=1))
	float DropRate = 0.f;
};

USTRUCT(BlueprintType)
struct FExp : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, meta=(ClampMin = 0))
	float Exp = 0;
};

/**
 * 사망시 아이템 드롭을 위해 생성된 컴포넌트 입니다.
 * 사망시 플레이어에게 부여하는 경험치 처리도 이 컴포넌트에서 합니다.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SOULLIKE_API UItemDropComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UItemDropComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	class UDataTable* DropTable;
	UPROPERTY(EditAnywhere)
	class UDataTable* ExpTable;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AExpOrb> ExpOrbObject;

	//몬스터가 드롭할 수 있는 아이템 리스트
	UPROPERTY(VisibleAnywhere)
	TArray<FDropItem> DropItems;
	//몬스터가 사망시 플레이어에게 전달할 경험치
	UPROPERTY(VisibleAnywhere)
	float Exp;
	//아이템이 드롭될 떄 이 소켓에 부착됩니다.
	UPROPERTY(EditAnywhere)
	FName ItemAttachSocketName = NAME_None;

	void CreateDropItemListFromTable();
	void GetDropExpFromTable();

	virtual void PostInitProperties() override;

public:
	void BossDropItem(class ABaseCharacter* DropBy);
	/**
	 * 아이템을 드롭합니다.
	 * 주사위 굴려서 확률에 당첨된 대상들을 모아서 랜덤으로 하나 드롭합니다.
	 * @param DropBy 누구에 의해 드롭되나요?
	 */
	UFUNCTION(BlueprintCallable)
	AItemActor* DropItem(class ABaseCharacter* DropBy);

	/**
	 * 이 오너를 사망하게 만든 대상에게 경험치를 지급합니다.
	 * @param Cast 
	 */
	UFUNCTION(BlueprintCallable)
	void GiveExp(ABaseCharacter* DropBy);
};
