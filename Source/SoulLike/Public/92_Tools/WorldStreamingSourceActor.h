// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WorldStreamingSourceActor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStreamingComplete);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAfterStreamingComplete);

DECLARE_DYNAMIC_DELEGATE(FOnStreamingCompleteK2);
DECLARE_DYNAMIC_DELEGATE(FOnAfterStreamingCompleteK2);


UCLASS(Blueprintable)
class SOULLIKE_API AWorldStreamingSourceActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AWorldStreamingSourceActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void PostInitializeComponents() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnStreamingCompleteEvent();

protected:
	UPROPERTY(VisibleAnywhere)
	class UWorldPartitionStreamingSourceComponent* WorldPartitionStreamingSource;

public:
	UPROPERTY()
	FOnStreamingComplete OnStreamingComplete;
	//스트리밍 완료 이벤트 호출 후 다음으로 이어서 호출됩니다.
	UPROPERTY()
	FOnAfterStreamingComplete OnAfterStreamingComplete;
	
	UPROPERTY()
	FOnStreamingCompleteK2 K2_OnStreamingComplete;
	UPROPERTY()
	FOnAfterStreamingCompleteK2 K2_OnAfterStreamingComplete;
	
	UPROPERTY(Transient)
	bool bShouldDestroy = false;


protected:
	FTimerHandle DisableTimerHandle;
	void EnableSource();

public:
	UFUNCTION(BlueprintCallable)
	void StreamingStart(FVector StreamingLocation);

	//블루프린트 내부에서만 호출하는것을 권장합니다.
	UFUNCTION(BlueprintCallable)
	void StreamingStartWithDelegate(FVector StreamingLocation,const FOnStreamingCompleteK2& K2_OnStreamingCompleteEvent,const FOnAfterStreamingCompleteK2& K2_OnAfterStreamingCompleteEvent);
};
