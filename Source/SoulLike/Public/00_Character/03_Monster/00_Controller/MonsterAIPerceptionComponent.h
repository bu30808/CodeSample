// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Perception/AIPerceptionComponent.h"
#include "MonsterAIPerceptionComponent.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API UMonsterAIPerceptionComponent : public UAIPerceptionComponent
{
	GENERATED_BODY()


	virtual void HandleExpiredStimulus(FAIStimulus& StimulusStore) override;
};
