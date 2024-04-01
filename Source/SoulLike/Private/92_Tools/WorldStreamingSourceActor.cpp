// Fill out your copyright notice in the Description page of Project Settings.


#include "92_Tools/WorldStreamingSourceActor.h"

#include "00_Character/BaseCharacter.h"
#include "Components/WorldPartitionStreamingSourceComponent.h"
#include "Logging/StructuredLog.h"

// Sets default values
AWorldStreamingSourceActor::AWorldStreamingSourceActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	WorldPartitionStreamingSource = CreateDefaultSubobject<UWorldPartitionStreamingSourceComponent>(
		TEXT("WorldPartitionStreamingSource"));
}

// Called when the game starts or when spawned
void AWorldStreamingSourceActor::BeginPlay()
{
	Super::BeginPlay();
	UE_LOGFMT(LogTemp, Log, "스트리밍 액터 준비됨.");
	WorldPartitionStreamingSource->DisableStreamingSource();
}

void AWorldStreamingSourceActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	UE_LOGFMT(LogTemp,Warning,"스트리밍 액터 종료 : {0}",StaticEnum<EEndPlayReason::Type>()->GetValueAsString(EndPlayReason));
}

void AWorldStreamingSourceActor::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	OnStreamingComplete.AddUniqueDynamic(this, &AWorldStreamingSourceActor::OnStreamingCompleteEvent);
}

// Called every frame
void AWorldStreamingSourceActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (WorldPartitionStreamingSource->IsStreamingCompleted())
	{
		OnStreamingComplete.Broadcast();
	}
}

void AWorldStreamingSourceActor::OnStreamingCompleteEvent()
{
	UE_LOGFMT(LogTemp, Warning, "스트리밍이 완료되었습니다..");
	SetActorTickEnabled(false);
	
	if (Owner->IsA<ABaseCharacter>())
	{
		Owner->SetActorLocation(GetActorLocation());
	}
	
	WorldPartitionStreamingSource->DisableStreamingSource();

	OnAfterStreamingComplete.Broadcast();
	
	if (bShouldDestroy)
	{
		Destroy();
	}
}

void AWorldStreamingSourceActor::EnableSource()
{
	WorldPartitionStreamingSource->EnableStreamingSource();
	SetActorTickEnabled(true);
}

void AWorldStreamingSourceActor::StreamingStart(FVector StreamingLocation)
{
	if(Owner!=nullptr){
		
		UE_LOGFMT(LogTemp, Log, "스트리밍을 시작합니다.");

		GetWorldTimerManager().ClearTimer(DisableTimerHandle);
		SetActorLocation(StreamingLocation);
		WorldPartitionStreamingSource->EnableStreamingSource();
		SetActorTickEnabled(true);
		
	}else
	{
		UE_LOGFMT(LogTemp, Error, "오너가 널이라 스트리밍을 시작할 수 없습니다.");
	}
}
