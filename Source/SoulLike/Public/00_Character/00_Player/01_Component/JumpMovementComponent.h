// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "00_Character/00_Player/PlayerCharacter.h"
#include "Components/ActorComponent.h"
#include "JumpMovementComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SOULLIKE_API UJumpMovementComponent : public UActorComponent
{
	GENERATED_BODY()

	friend class AJumpPad;

protected:
	UPROPERTY(EditAnywhere)
	class UAnimMontage* ActivateMontage;
	UPROPERTY(EditAnywhere)
	class UAnimMontage* JumpStartMontage;
	UPROPERTY(EditAnywhere)
	class UAnimMontage* LandMontage;

	UPROPERTY()
	TObjectPtr<class APlayerCharacter> Pawn;
	UPROPERTY()
	TObjectPtr<class AJumpPad> CurJumpPad;

public:
	// Sets default values for this component's properties
	UJumpMovementComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	void ActivateJumpPad(class AJumpPad* JumpPad);
	void PlayJumpStartMontage(class AJumpPad* JumpPad);
	void PlayJumpLandMontage();
	UFUNCTION()
	void OnMontageBlendingOutEvent(UAnimMontage* Montage, bool bInterrupted);
};
