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

	//확인용. 내부적으로 데이터가 필요하면 그때마다 데이터테이블을 읽어서 이 값을 업데이트합니다.
	//테이블에 자동 업데이트 하고 싶다면, 이 값을 설정한 뒤, AddBonfireInToDataTable함수를 에디터에서 호출하세요.
	UPROPERTY(EditAnywhere, Category="Default")
	FBonfireInformation BonfireInformation;


	//이 저장장소가 활성화 되었는지 확인하는 변수입니다.
	UPROPERTY(Transient)
	bool bIsActivate = false;
	UPROPERTY(EditAnywhere, Category="Default")
	FLinearColor PeaceColor = FLinearColor(0.f, 6.394939f, 20.f, 1.000000);
	UPROPERTY(EditAnywhere, Category="Default")
	FLinearColor CombatColor = FLinearColor(20.f, 0.776941f, 0.f, 1.f);
	//이 화톳불이 위치하는 곳의 하늘 시간입니다.
	UPROPERTY(EditAnywhere, Category="Sky")
	float SkyTime;
	UPROPERTY(EditAnywhere, Category="Sky")
	class UDataAsset* Weather;
	UPROPERTY(EditAnywhere, Category="Default")
	float MonsterCheckRadius = 500.f;
	UPROPERTY(Transient,VisibleAnywhere,BlueprintReadWrite)
	bool bCanUseBonfire = true;

	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;


	//이 화톳불의 위치정보를 테이블에 추가합니다.
	UFUNCTION(BlueprintCallable, CallInEditor, DisplayName= "테이블 업데이트")
	void AddBonfireInToDataTable();
	UFUNCTION(BlueprintCallable, CallInEditor, DisplayName= "정보 불러오기")
	void LoadBonfireInfoFromTable();
	
	void SetActivate();

public:
	UFUNCTION()
	void OnChangePlayerStateEvent(EPlayerCharacterState State);

	UFUNCTION(BlueprintCallable)
	void ActivateBonfire(class APlayerCharacter* Player);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool GetIsBonfireActivate() const { return bIsActivate; }

	TArray<FBonfireInformation*> GetTeleportList() const;

	const FBonfireInformation& GetBonfireInformation();

	virtual bool ShowInteractionWidgetDirectly_Implementation() override;
	virtual void Interaction_Implementation(ABaseCharacter* Start) override;
};
