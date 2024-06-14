// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SCurveProjectile.generated.h"


//어떤 타입의 곡선을 사용할 것인가?
UENUM(BlueprintType)
enum class ETrigonometricFunctions : uint8
{
	SIN,
	COS,
	RANDOM
};

/*
* 사인, 혹은 코사인 그래프를 이용해 S를 그리며 나아가는 프로젝타일 입니다.
 */
UCLASS()
class SOULLIKE_API ASCurveProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASCurveProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	//사용할 곡선 타입입니다.
	UPROPERTY(EditAnywhere,Category=Default)
	ETrigonometricFunctions TrigonometricFunctionsType;
	//프로젝타일 속도
	UPROPERTY(EditAnywhere,Category=Default)
	float Speed = 1000.f;
	//사인곡선의 결과값에다 이 값을 곱하게 됩니다. 이 값이 클 수록 좌우로 흔들리는 정도가 커집니다.
	UPROPERTY(EditAnywhere,Category=Default)
	float Amplitude = 100.f;
	//사인곡선의 주기를 결정합니다. 값이 클 수록 자주 좌우로 흔들리게 됩니다.
	UPROPERTY(EditAnywhere,Category=Default)
	float Frequency = 3.f;
private:
	//사인그래프의 진행도를 저장할 변수입니다.
	UPROPERTY(Transient)
	float TimeElapsed = 0.f;

	//시작지점
	UPROPERTY(Transient)
	FVector StartLocation;
};
