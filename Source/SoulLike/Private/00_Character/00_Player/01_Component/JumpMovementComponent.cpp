// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/00_Player/01_Component/JumpMovementComponent.h"

#include "00_Character/00_Player/PlayerCharacter.h"
#include "92_Tools/JumpPad.h"
#include "Components/CapsuleComponent.h"

// Sets default values for this component's properties
UJumpMovementComponent::UJumpMovementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UJumpMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	Pawn = GetOwner<APlayerCharacter>();
}

void UJumpMovementComponent::ActivateJumpPad(class AJumpPad* JumpPad)
{
	if (Pawn)
	{
		CurJumpPad = JumpPad;
		Pawn->GetMesh()->GetAnimInstance()->OnMontageBlendingOut.AddUniqueDynamic(
			this, &UJumpMovementComponent::OnMontageBlendingOutEvent);
		Pawn->PlayAnimMontage(ActivateMontage);
	}
}

void UJumpMovementComponent::PlayJumpStartMontage(AJumpPad* JumpPad)
{
	if (CurJumpPad)
	{
		CurJumpPad->JumpMove();
	}

	/*Pawn->GetMesh()->GetAnimInstance()->OnMontageBlendingOut.AddUniqueDynamic(this,&UJumpMovementComponent::OnMontageBlendingOutEvent);
	Pawn->PlayAnimMontage(JumpStartMontage);*/
}

void UJumpMovementComponent::PlayJumpLandMontage()
{
	Pawn->PlayAnimMontage(LandMontage);
}

void UJumpMovementComponent::OnMontageBlendingOutEvent(UAnimMontage* Montage, bool bInterrupted)
{
	if (Pawn)
	{
		if (Montage == ActivateMontage)
		{
			if (CurJumpPad)
			{
				CurJumpPad->SetActivate();
			}
			Pawn->GetMesh()->GetAnimInstance()->OnMontageBlendingOut.RemoveAll(this);
		}

		if (Montage == JumpStartMontage)
		{
			if (CurJumpPad)
			{
				CurJumpPad->JumpMove();
			}
			Pawn->GetMesh()->GetAnimInstance()->OnMontageBlendingOut.RemoveAll(this);
		}
	}
}
