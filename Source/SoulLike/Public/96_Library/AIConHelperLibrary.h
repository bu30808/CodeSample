// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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
	static void ChangePlayerState(AActor* AIConOrMonster,AActor* Player,EPlayerCharacterState NewState);
};
