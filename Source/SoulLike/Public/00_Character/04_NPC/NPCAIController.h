// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "00_Character/03_Monster/00_Controller/MonsterAIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "NPCAIController.generated.h"

UCLASS()
class SOULLIKE_API ANPCAIController : public AMonsterAIController
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ANPCAIController();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;
	
	//대상을 감지했을 떄 호출됨.
	virtual void OnTargetPerceptionUpdatedEvent(AActor* Target, FAIStimulus Stimulus) override;
	//대상을 잊었을 때 호출됨
	virtual void OnTargetPerceptionForgottenEvent(AActor* Target) override;
};
