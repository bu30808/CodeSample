// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WorldStreamingSourceActor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStreamingComplete);

UCLASS()
class SOULLIKE_API AWorldStreamingSourceActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AWorldStreamingSourceActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION()
	void OnStreamingCompleteEvent();

protected:
	UPROPERTY(VisibleAnywhere)
	class UWorldPartitionStreamingSourceComponent* WorldPartitionStreamingSource;

	UPROPERTY(BlueprintCallable)
	FOnStreamingComplete OnStreamingComplete;

	FTimerHandle DisableTimerHandle;

public:
	UFUNCTION(BlueprintCallable)
	void StartStreaming();
};
