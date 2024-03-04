// Fill out your copyright notice in the Description page of Project Settings.


#include "92_Tools/Ladder.h"

#include "Components/BillboardComponent.h"
#include "Components/BoxComponent.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"


// Sets default values
ALadder::ALadder()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent =CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	PoleRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("PoleRootComponent"));	
	PoleRootComponent->SetupAttachment(RootComponent); 

	LadderPoleRightMeshComponent= CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LadderPoleRightMeshComponent"));
	LadderPoleRightMeshComponent->SetupAttachment(PoleRootComponent);
	
	LadderPoleLeftMeshComponent= CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LadderPoleLeftMeshComponent"));
	LadderPoleLeftMeshComponent->SetupAttachment(PoleRootComponent);
	
	TopBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("LadderTop"));
	TopBoxComponent->SetupAttachment(PoleRootComponent);

	BottomBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("LadderBottom"));
	BottomBoxComponent->SetupAttachment(PoleRootComponent);
	
	BarInstancedStaticMeshComponent = CreateDefaultSubobject<UInstancedStaticMeshComponent>(
		TEXT("BarInstancedStaticMeshComponent"));
	BarInstancedStaticMeshComponent->SetupAttachment(RootComponent);

	ScrewInstancedStaticMeshComponent = CreateDefaultSubobject<UInstancedStaticMeshComponent>(
		TEXT("ScrewInstancedStaticMeshComponent"));
	ScrewInstancedStaticMeshComponent->SetupAttachment(RootComponent);


	LadderEnter_Bottom = CreateDefaultSubobject<UBillboardComponent>(TEXT("LadderEnter_Bottom"));
	LadderEnter_Bottom->SetupAttachment(RootComponent);
	LadderEnter_Top = CreateDefaultSubobject<UBillboardComponent>(TEXT("LadderEnter_Top"));
	LadderEnter_Top->SetupAttachment(RootComponent);


	LadderExit_Bottom = CreateDefaultSubobject<UBillboardComponent>(TEXT("LadderExit_Bottom"));
	LadderExit_Bottom->SetupAttachment(RootComponent);
	LadderExit_Top = CreateDefaultSubobject<UBillboardComponent>(TEXT("LadderExit_Top"));
	LadderExit_Top->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ALadder::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ALadder::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
