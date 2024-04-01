// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/03_Monster/03_AI/00_Common/00_Task/BTTask_GetMoveLocation.h"

#include "AIController.h"
#include "00_Character/02_Animation/00_NotifyState/AnimNotifyState_AddForce.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_GetMoveLocation::UBTTask_GetMoveLocation()
{
	BlackboardKey.SelectedKeyName = "MoveLocation";
}

EBTNodeResult::Type UBTTask_GetMoveLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (auto aiCon = OwnerComp.GetAIOwner())
	{
		if (auto pawn = aiCon->GetPawn())
		{
			if (auto bbComp = OwnerComp.GetBlackboardComponent())
			{
				FVector resultLocation;

				switch (Direction)
				{
				case EDirection::Front:
					resultLocation = pawn->GetActorLocation() + pawn->GetActorForwardVector() * Distance;
					break;
				case EDirection::Right:
					resultLocation = pawn->GetActorLocation() + pawn->GetActorRightVector() * Distance;
					break;
				case EDirection::Left:
					resultLocation = pawn->GetActorLocation() + pawn->GetActorRightVector() * Distance * -1;
					break;
				case EDirection::Back:
					resultLocation = pawn->GetActorLocation() + pawn->GetActorForwardVector() * Distance * -1;
					break;
				case EDirection::FrontRight:
					resultLocation = pawn->GetActorLocation() + (pawn->GetActorForwardVector() + pawn->
						GetActorRightVector()).GetSafeNormal() * Distance;
					break;
				case EDirection::FrontLeft:
					resultLocation = pawn->GetActorLocation() + (pawn->GetActorForwardVector() + pawn->
						GetActorRightVector() * -1).GetSafeNormal() * Distance;
					break;
				case EDirection::BackRight:
					resultLocation = pawn->GetActorLocation() + (pawn->GetActorForwardVector() * -1 + pawn->
						GetActorRightVector()).GetSafeNormal() * Distance;
					break;
				case EDirection::BackLeft:
					resultLocation = pawn->GetActorLocation() + (pawn->GetActorForwardVector() * -1 + pawn->
						GetActorRightVector() * -1).GetSafeNormal() * Distance;
					break;
				case EDirection::Up:
					resultLocation = pawn->GetActorLocation() + pawn->GetActorUpVector() * Distance;
					break;
				case EDirection::Down:
					resultLocation = pawn->GetActorLocation() + pawn->GetActorUpVector() * -1 * Distance;
					break;
				case EDirection::MAX:
					break;
				}

				bbComp->SetValueAsVector(GetSelectedBlackboardKey(), resultLocation);

				return EBTNodeResult::Succeeded;
			}
		}
	}
	return Super::ExecuteTask(OwnerComp, NodeMemory);
}
