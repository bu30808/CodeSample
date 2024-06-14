// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/03_Monster/10_ForestGolem/ForestGolem_ElderEnt.h"


// Sets default values
AForestGolem_ElderEnt::AForestGolem_ElderEnt()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	WeaponComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Weapon"));
	WeaponComponent->SetupAttachment(GetMesh(),"Weapon");
	WeaponComponent->SetVisibility(false);
	WeaponComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Called when the game starts or when spawned
void AForestGolem_ElderEnt::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AForestGolem_ElderEnt::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

