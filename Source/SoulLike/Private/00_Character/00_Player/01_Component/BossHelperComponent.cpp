// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/00_Player/01_Component/BossHelperComponent.h"

#include "96_Library/LevelEventHelperLibrary.h"


// Sets default values for this component's properties
UBossHelperComponent::UBossHelperComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UBossHelperComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UBossHelperComponent::AddTrigger(AActor* Trigger)
{
	TriggerBoxes.Add(Trigger);
}

void UBossHelperComponent::AddMistBlock(const TArray<AActor*>& MistBlockActors)
{
	MistBlocks.Append(MistBlockActors);
}

void UBossHelperComponent::DisableTrigger()
{
	for(const auto iter : TriggerBoxes)
	{
		iter->SetActorEnableCollision(false);
	}

	TriggerBoxes.Empty();
}

void UBossHelperComponent::DisableMistBlock()
{
	ULevelEventHelperLibrary::ActiveBlockingActors(MistBlocks, false);
	MistBlocks.Empty();
}

void UBossHelperComponent::ResetTrigger()
{
	for(const auto iter : TriggerBoxes)
	{
		iter->SetActorEnableCollision(true);
	}
}

void UBossHelperComponent::ResetMistBlock()
{
	ULevelEventHelperLibrary::ActiveBlockingActors(MistBlocks, false);
}

