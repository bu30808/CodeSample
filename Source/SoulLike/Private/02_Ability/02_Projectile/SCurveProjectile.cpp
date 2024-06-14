// Fill out your copyright notice in the Description page of Project Settings.


#include "02_Ability/02_Projectile/SCurveProjectile.h"

// Sets default values
ASCurveProjectile::ASCurveProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASCurveProjectile::BeginPlay()
{
	Super::BeginPlay();

	StartLocation = GetActorLocation();
	if(TrigonometricFunctionsType == ETrigonometricFunctions::RANDOM)
	{
		if(FMath::RandBool()){
			TrigonometricFunctionsType = ETrigonometricFunctions::COS;
		}else
		{
			TrigonometricFunctionsType = ETrigonometricFunctions::SIN;
		}
	}
}

// Called every frame
void ASCurveProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//그래프 진행도 갱신
	TimeElapsed+=DeltaTime;

	float x = Speed*TimeElapsed;
	float y = Amplitude;
	
	switch (TrigonometricFunctionsType) {
	case ETrigonometricFunctions::SIN:
		y*=FMath::Sin(Frequency*TimeElapsed);
		break;
	case ETrigonometricFunctions::COS:
		y*=FMath::Cos(Frequency*TimeElapsed);
		break;
	}

	const FVector& newLocation = (GetActorRightVector() * y) + (GetActorForwardVector() * x) + StartLocation;
	SetActorLocation(newLocation);
}

