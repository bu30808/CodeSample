// Fill out your copyright notice in the Description page of Project Settings.


#include "94_Task/GameplayTask_LaunchTimerEvent.h"

#include "GameplayTasksComponent.h"
#include "Logging/StructuredLog.h"

UGameplayTask_LaunchTimerEvent::UGameplayTask_LaunchTimerEvent(const FObjectInitializer& ObjectInitializer) : Super(
	ObjectInitializer)
{
	bTickingTask = false;
}

void UGameplayTask_LaunchTimerEvent::TimerEvent()
{
	OnTimerEvent.ExecuteIfBound();
}

UGameplayTask_LaunchTimerEvent* UGameplayTask_LaunchTimerEvent::LaunchTimerEvent(AActor* TaskOwner,
	FOnTimerEvent TimerEvent, float Time, bool bLoop)
{
	if (TaskOwner->IsValidLowLevel() == false)
	{
		UE_LOGFMT(LogTemp, Error, "테스크 오너가 유효하지 않습니다 : {0} {1}", __FUNCTION__, __LINE__);
		return nullptr;
	}

	UActorComponent* taskComp = nullptr;
	if (TaskOwner->FindComponentByClass(UGameplayTasksComponent::StaticClass()) == nullptr)
	{
		if (UActorComponent* newComp = NewObject<UActorComponent>(TaskOwner, UGameplayTasksComponent::StaticClass()))
		{
			newComp->RegisterComponent();
			taskComp = newComp;
		}
		else
		{
			return nullptr;
		}
	}
	else
	{
		taskComp = TaskOwner->GetComponentByClass(UGameplayTasksComponent::StaticClass());
	}

	UGameplayTask_LaunchTimerEvent* newTask = NewTask<UGameplayTask_LaunchTimerEvent>(taskComp);
	newTask->OnTimerEvent = TimerEvent;
	newTask->bTimerLoop = bLoop;
	newTask->IntervalTime = Time;
	newTask->World = TaskOwner->GetWorld();

	return newTask;
}

void UGameplayTask_LaunchTimerEvent::Activate()
{
	Super::Activate();
	if (World)
	{
		World->GetTimerManager().SetTimer(TimerHandle, this, &UGameplayTask_LaunchTimerEvent::TimerEvent, IntervalTime,
		                                  bTimerLoop);
	}
	else
	{
		EndTask();
	}
}

void UGameplayTask_LaunchTimerEvent::OnDestroy(bool bInOwnerFinished)
{
	Super::OnDestroy(bInOwnerFinished);
	if (World)
	{
		World->GetTimerManager().ClearTimer(TimerHandle);
	}
}
