// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "00_Character/04_NPC/NPCBase.h"
#include "93_SaveGame/SoulLikeSaveGame.h"
#include "Engine/DataTable.h"
#include "Bonfire.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API ABonfire : public ANPCBase
{
	GENERATED_BODY()

public:
	ABonfire();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UNiagaraComponent* NiagaraComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UNiagaraComponent* CannotActivate_NiagaraComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UAudioComponent* InfiniteAudioComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UBillboardComponent* SpawnPointBillboardComponent;

	UPROPERTY(EditAnywhere)
	class USoundBase* ActivateSound;
	UPROPERTY(EditAnywhere)
	class UDataTable* BonfireTable;

	UPROPERTY(EditAnywhere, Category="Info")
	FBonfireInformation BonfireInformation;


	//이 저장장소가 활성화 되었는지 확인하는 변수입니다.
	UPROPERTY(Transient)
	bool bIsActivate = false;
	UPROPERTY(EditAnywhere)
	FLinearColor PeaceColor = FLinearColor(0.f, 6.394939f, 20.f, 1.000000);
	UPROPERTY(EditAnywhere)
	FLinearColor CombatColor = FLinearColor(20.f, 0.776941f, 0.f, 1.f);
	//이 화톳불이 위치하는 곳의 하늘 시간입니다.
	UPROPERTY(EditAnywhere)
	float SkyTime;

	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;


	//이 화톳불의 위치정보를 테이블에 추가합니다.
	UFUNCTION(BlueprintCallable, CallInEditor, DisplayName= "테이블 업데이트")
	void AddBonfireInToDataTable();
	void SetActivate();

public:
	UFUNCTION()
	void OnChangePlayerStateEvent(EPlayerCharacterState State);

	UFUNCTION(BlueprintCallable)
	void ActivateBonfire(class APlayerCharacter* Player);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool GetIsBonfireActivate() const { return bIsActivate; }

	TArray<FBonfireInformation*> GetTeleportList() const;

	const FBonfireInformation& GetBonfireInformation() { return BonfireInformation; }
};
