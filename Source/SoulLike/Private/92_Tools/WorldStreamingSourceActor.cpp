// Fill out your copyright notice in the Description page of Project Settings.


#include "92_Tools/WorldStreamingSourceActor.h"
#include "Components/WorldPartitionStreamingSourceComponent.h"

// Sets default values
AWorldStreamingSourceActor::AWorldStreamingSourceActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	WorldPartitionStreamingSource = CreateDefaultSubobject<UWorldPartitionStreamingSourceComponent>(
		TEXT("WorldPartitionStreamingSource"));

	
}

// Called when the game starts or when spawned
void AWorldStreamingSourceActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void AWorldStreamingSourceActor::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	SetActorTickEnabled(false);
	WorldPartitionStreamingSource->DisableStreamingSource();
	OnStreamingComplete.AddUniqueDynamic(this,&AWorldStreamingSourceActor::OnStreamingCompleteEvent);
}

// Called every frame
void AWorldStreamingSourceActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(WorldPartitionStreamingSource->IsStreamingCompleted())
	{
		OnStreamingComplete.Broadcast();
	}
}

void AWorldStreamingSourceActor::OnStreamingCompleteEvent()
{
	SetActorTickEnabled(false);
	GetWorldTimerManager().SetTimer(DisableTimerHandle,WorldPartitionStreamingSource,&UWorldPartitionStreamingSourceComponent::DisableStreamingSource,5.f);
}

void AWorldStreamingSourceActor::StartStreaming()
{
	GetWorldTimerManager().ClearTimer(DisableTimerHandle);
	
	WorldPartitionStreamingSource->EnableStreamingSource();
	SetActorTickEnabled(true);
}

