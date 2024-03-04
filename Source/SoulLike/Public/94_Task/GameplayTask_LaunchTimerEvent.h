// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTask.h"
#include "GameplayTask_LaunchTimerEvent.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_DELEGATE(FOnTimerEvent);

UCLASS()
class SOULLIKE_API UGameplayTask_LaunchTimerEvent : public UGameplayTask
{
	GENERATED_BODY()

	UGameplayTask_LaunchTimerEvent(const FObjectInitializer& ObjectInitializer);

	UPROPERTY()
	FOnTimerEvent OnTimerEvent;
	UPROPERTY()
	FTimerHandle TimerHandle;
	UPROPERTY()
	UWorld* World;
	UPROPERTY()
	bool bTimerLoop;
	UPROPERTY()
	float IntervalTime;

	UFUNCTION()
	void TimerEvent();

public:
	UFUNCTION(BlueprintCallable)
	static UGameplayTask_LaunchTimerEvent* LaunchTimerEvent(AActor* TaskOwner, FOnTimerEvent TimerEvent, float Time,
	                                                        bool bLoop);

	virtual void Activate() override;
	virtual void OnDestroy(bool bInOwnerFinished) override;
};
