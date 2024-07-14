// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DynamicSkyActor.generated.h"

UCLASS()
class SOULLIKE_API ADynamicSkyActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ADynamicSkyActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//0에서 2400 사이의 값을 주세요.
	//ex) 오후 1시 30분 : 1330
	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category="𝙰𝟷  ·  Basic Controls")
	float TimeOfDay;
	//이 값이 참이면 밤낮 시뮬레이션을 돌립니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category="𝙱𝟷  ·  Animate Time Of Day")
	bool bAnimateTimeOfDay;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintImplementableEvent)
	void SetWeather(class UDataAsset* Weather);
	UFUNCTION(BlueprintImplementableEvent)
	class UDataAsset* GetWeather();
};
