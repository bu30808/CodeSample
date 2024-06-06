// Fill out your copyright notice in the Description page of Project Settings.


#include "92_Tools/WorldStreamingSourceActor.h"

#include "00_Character/BaseCharacter.h"
#include "Components/WorldPartitionStreamingSourceComponent.h"
#include "Logging/StructuredLog.h"
#include "WorldPartition/WorldPartition.h"
#include "WorldPartition/WorldPartitionSubsystem.h"

// Sets default values
AWorldStreamingSourceActor::AWorldStreamingSourceActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	WorldPartitionStreamingSource = CreateDefaultSubobject<UWorldPartitionStreamingSourceComponent>(
		TEXT("WorldPartitionStreamingSource"));
	WorldPartitionStreamingSource->Priority = EStreamingSourcePriority::Highest;
	WorldPartitionStreamingSource->TargetGrids.Emplace("MainGrid");

	FStreamingSourceShape Shape;
	Shape.bUseGridLoadingRange = true;
	Shape.LoadingRangeScale = 0.5f;
	Shape.bIsSector = true;
	WorldPartitionStreamingSource->Shapes.Emplace(Shape);
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

	UE_LOGFMT(LogTemp, Warning, "스트리밍 액터 종료 : {0}",
	          StaticEnum<EEndPlayReason::Type>()->GetValueAsString(EndPlayReason));
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
		if (auto WorldPartitionSubsystem = GetWorld()->GetSubsystem<UWorldPartitionSubsystem>())
		{
			if(WorldPartitionSubsystem->IsAllStreamingCompleted())
			{
				K2_OnStreamingComplete.ExecuteIfBound();
				OnStreamingComplete.Broadcast();
				
				/*FHitResult hit;
				TArray<AActor*> ignoreActors;
				ignoreActors.Emplace(GetOwner());
				if(UKismetSystemLibrary::LineTraceSingle(this,GetActorLocation(),GetActorLocation() + GetActorUpVector() * -2000.f,UEngineTypes::ConvertToTraceType(ECC_WorldStatic),false,ignoreActors,EDrawDebugTrace::ForOneFrame,hit,true,FLinearColor::Blue,FLinearColor::Green))
				{
					UKismetSystemLibrary::PrintString(this,FString::Printf(TEXT("아래에 오브젝트(%s)를 감지했습니다. 스트리밍 완료 처리합니다"),*hit.GetActor()->GetName()));
					K2_OnStreamingComplete.ExecuteIfBound();
					OnStreamingComplete.Broadcast();
				}else
				{
					UKismetSystemLibrary::PrintString(this,FString::Printf(TEXT("아래에 걸리는 오브젝트가 없습니다!!")));
				}*/
			}
		}
		
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
	K2_OnAfterStreamingComplete.ExecuteIfBound();
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
	if (Owner != nullptr)
	{
		UE_LOGFMT(LogTemp, Log, "스트리밍을 시작합니다 : {0}",StreamingLocation.ToString());
		
		GetWorldTimerManager().ClearTimer(DisableTimerHandle);
		SetActorLocation(StreamingLocation);
		WorldPartitionStreamingSource->EnableStreamingSource();
		SetActorTickEnabled(true);
	}
	else
	{
		UE_LOGFMT(LogTemp, Error, "오너가 널이라 스트리밍을 시작할 수 없습니다.");
	}
}

void AWorldStreamingSourceActor::StreamingStartWithDelegate(FVector StreamingLocation,
                                                            const FOnStreamingCompleteK2& K2_OnStreamingCompleteEvent,
                                                            const FOnAfterStreamingCompleteK2&
                                                            K2_OnAfterStreamingCompleteEvent)
{
	StreamingStart(StreamingLocation);
	K2_OnStreamingComplete = K2_OnStreamingCompleteEvent;
	K2_OnAfterStreamingComplete = K2_OnAfterStreamingCompleteEvent;
}
