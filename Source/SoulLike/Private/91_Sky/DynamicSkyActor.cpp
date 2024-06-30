// Fill out your copyright notice in the Description page of Project Settings.


#include "91_Sky/DynamicSkyActor.h"

#include "Net/UnrealNetwork.h"

// Sets default values
ADynamicSkyActor::ADynamicSkyActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ADynamicSkyActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ADynamicSkyActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void ADynamicSkyActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADynamicSkyActor, bAnimateTimeOfDay);
}

