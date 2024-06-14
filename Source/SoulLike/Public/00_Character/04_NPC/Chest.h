// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NPCBase.h"
#include "Chest.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnOpenChest,class AChest*, Box,class ABaseCharacter*, OpendBy);

UCLASS()
class SOULLIKE_API AChest : public ANPCBase
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AChest();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;
	
	UFUNCTION()
	void OnBeginOverlapEvent(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(Transient,BlueprintReadOnly)
	bool bOpened = false;
	UPROPERTY(Transient,BlueprintReadOnly)
	bool bAlreadyGetChestItem = false;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	class UBoxComponent* BoxComponent;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	class UStaticMeshComponent* ChestMeshComponent;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	class UStaticMeshComponent* ChestMeshTopComponent;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	class USceneComponent* ItemSpawnPointComponent;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	class UNiagaraComponent* ChestItemGlowNiagaraComponent;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	class UTimelineComponent* ChestOpenTimelineComponent;

	UPROPERTY(EditAnywhere,Category="Item")
	TSubclassOf<class AItemActor> ChestItemObject; 
	UPROPERTY(EditAnywhere,Category="Item")
	int32 ItemCount = 1;
	UPROPERTY(EditAnywhere,Category="Item")
	class USoundBase* OpenSound;
	UPROPERTY(EditAnywhere, Category = "Timeline")
	class UCurveFloat* ChestOpenCurve;


	//상자가 열렸을 때, 해야 할 일이 있다면 사용하세요.(ex 함정)
	UPROPERTY(BlueprintAssignable)
	FOnOpenChest OnOpenChest;
	
	FOnTimelineFloatStatic OnUpdateChestOpenTimeLine;
	FOnTimelineEventStatic OnFinishChestOpenTimeLine;

	virtual void Interaction_Implementation(ABaseCharacter* Start) override;

	//이 상자가 이미 열렸었는지 확인합니다.
	bool IsAlreadyOpen();
	//강제로 열림 상태로 전환합니다.
	void SetOpen();
	//이 상자 내부 아이템이 획득된 적이 있는지 확인합니다.
	bool IsAlreadyGetChestItem();
	//상자를 엽니다.
	void OpenChest(class APlayerCharacter* Player);
	//아이템을 플레이어에게 줍니다.
	void GiveChestItemToPlayer();

	void OnUpdateChestOpenTimeLineEvent(float Pitch);
	void OnFinishChestOpenTimeLineEvent();
};

