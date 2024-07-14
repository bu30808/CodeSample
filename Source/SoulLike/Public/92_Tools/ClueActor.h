// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ClueActor.generated.h"

/*
 * 막힌 문이나, 단서 메시지 출력용입니다.
 */
UCLASS()
class SOULLIKE_API AClueActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AClueActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;
	UFUNCTION()
	void OnActorBeginOverlapEvent(AActor* OverlappedActor, AActor* OtherActor);
	UFUNCTION()
	void OnActorEndOverlapEvent(AActor* OverlappedActor, AActor* OtherActor);

	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* BoxComponent;

	UPROPERTY(EditAnywhere)
	FText ShowMsg;
	
	UPROPERTY(Transient)
	bool bCanShowMsg = true;
};
