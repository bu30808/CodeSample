// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/00_Player/02_Animation/PlayerAnimInstance.h"

#include "00_Character/BaseCharacter.h"
#include "00_Character/00_Player/PlayerCharacter.h"
#include "00_Character/00_Player/01_Component/LadderMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	if (Character.IsValid())
	{
		LadderMovementComponent = Cast<APlayerCharacter>(Character)->GetLadderMovementComponent();
	}
}

void UPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (Character.IsValid())
	{
		bUseLadder = LadderMovementComponent->GetUseLadderMovement();

		if(bStartClimbLadder)
		{
			if(bShouldUpdateNewLadderLocation)
			{
				bShouldUpdateNewLadderLocation = false;
				CreateNewLadderLocation();
			}

			switch (LadderMovementComponent->GetLadderClimbType()) {
			case ELadderClimbType::EnterFromTop:
				OnLadderEnterFromTop();
				break;
			case ELadderClimbType::EscapeFromTop:
				OnLadderEscapeFromTop();
				break;
			case ELadderClimbType::EnterFromBottom:
				break;
			case ELadderClimbType::EscapeFromBottom:
				break;
			}

		}else
		{
			bShouldUpdateNewLadderLocation = true;
		}
	}
}

void UPlayerAnimInstance::ChangeBoneTransform_Implementation(float DeltaTime)
{
	Super::ChangeBoneTransform_Implementation(DeltaTime);
	SpineRotation = FMath::RInterpTo(SpineRotation, Character->SpineRotation, DeltaTime, BoneTransformLerpSpeed);
}

void UPlayerAnimInstance::CreateNewLadderLocation()
{
	const float& x = GetCurveValue("XAxis");
	const float& y = GetCurveValue("YAxis");
	const float& z = GetCurveValue("ZAxis");

	LadderNewLocation = FVector(x, y, z);
}

void UPlayerAnimInstance::OnLadderEscapeFromTop()
{
	LadderOldLocation = LadderNewLocation;

	CreateNewLadderLocation();

	const FVector& deltaVector = LadderNewLocation - LadderOldLocation;

	float newX = (Character->GetActorForwardVector() * deltaVector.Y).X + Character->GetActorLocation().X;
	float newY = (Character->GetActorForwardVector() * deltaVector.Y).Y + Character->GetActorLocation().Y;
	float newZ = /*deltaVector.Z +*/ Character->GetActorLocation().Z;

	Character->SetActorLocation(FVector(newX, newY, newZ));
}

void UPlayerAnimInstance::OnLadderEnterFromTop()
{
	LadderOldLocation = LadderNewLocation;

	CreateNewLadderLocation();

	const FVector& deltaVector = LadderNewLocation - LadderOldLocation;

	float newX = Character->GetActorLocation().X;
	float newY = Character->GetActorLocation().Y;
	float newZ = deltaVector.Z + Character->GetActorLocation().Z;

	Character->SetActorLocation(FVector(newX, newY, newZ));
}

void UPlayerAnimInstance::AnimNotify_OnLadderIdleEnter_Implementation()
{
	if (Character.IsValid())
	{
		//기본 시작인 오른손으로 초기화
		LadderMovementComponent->SetLadderHandOrder(ELadderHandOrder::RIGHT);
	}
}

void UPlayerAnimInstance::AnimNotify_OnLadderIdleExit_Implementation()
{
	if (Character.IsValid())
	{
		LadderRightHandLocation = LadderMovementComponent->GetNextHandLocation();

	}
}

void UPlayerAnimInstance::AnimNotify_OnLadderRightExit_Implementation()
{
	if (Character.IsValid())
	{
		//다음 손인 왼손으로 초기화
		LadderMovementComponent->SetLadderHandOrder(ELadderHandOrder::LEFT);
		LadderLeftHandLocation = LadderMovementComponent->GetNextHandLocation();
	
	}
}

void UPlayerAnimInstance::AnimNotify_OnLadderLeftExit_Implementation()
{
	if (Character.IsValid())
	{
		//다음인 오른손으로 초기화
		LadderMovementComponent->SetLadderHandOrder(ELadderHandOrder::RIGHT);
		LadderRightHandLocation = LadderMovementComponent->GetNextHandLocation();
	
	}
}

void UPlayerAnimInstance::AnimNotify_OnStartClimbLadder_Implementation()
{
	bStartClimbLadder = true;
}

void UPlayerAnimInstance::AnimNotify_OnEndClimbLadder_Implementation()
{
	bStartClimbLadder = false;
}
