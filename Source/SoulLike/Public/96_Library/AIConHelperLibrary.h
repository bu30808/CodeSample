// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "00_Character/BaseCharacter.h"
#include "UObject/Object.h"
#include "AIConHelperLibrary.generated.h"

enum class EPlayerCharacterState : uint8;
/**
 * 
 */
UCLASS()
class SOULLIKE_API UAIConHelperLibrary : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	static void ChangePlayerState(AActor* AIConOrMonster, AActor* Player, ECombatState NewState);

	
	/**
	 * 블랙보드에 "Target"값에 저장되어있는 대상을 가져옵니다.
	 * @param BlackboardOwner 
	 * @return 
	 */
	UFUNCTION(BlueprintCallable,BlueprintPure)
	static AActor* GetBlackboardValueNamedTarget(AActor* BlackboardOwner);
	
	static ETeamAttitude::Type CheckTeam(AActor* Checker, const AActor& Other);
};
