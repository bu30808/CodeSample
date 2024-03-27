// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character\03_Monster\03_AI\00_Common\00_Task\BTTask_MoveTasks.h"

#include "AIController.h"
#include "NavigationSystem.h"
#include "00_Character/02_Animation/00_NotifyState/AnimNotifyState_AddForce.h"
#include "00_Character/03_Monster/00_Controller/MonsterAIController.h"
#include "Kismet/KismetMathLibrary.h"
#include "Logging/StructuredLog.h"
#include "Navigation/PathFollowingComponent.h"

UBTTask_MoveRandomPoint::UBTTask_MoveRandomPoint()
{
	bCreateNodeInstance = true;
	NodeName = TEXT("랜덤 포인트로 이동");
	bNotifyTick = false;
	bNotifyTaskFinished = true;
}

void UBTTask_MoveRandomPoint::OnMoveCompleteEvent(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	switch (Result) {
	case EPathFollowingResult::Success:
		FinishLatentTask(*Cast<UBehaviorTreeComponent>(AICon->GetBrainComponent()),EBTNodeResult::Succeeded);
		break;
	case EPathFollowingResult::Blocked:
	case EPathFollowingResult::OffPath:
	case EPathFollowingResult::Aborted:
	case EPathFollowingResult::Skipped_DEPRECATED:
	case EPathFollowingResult::Invalid:
		FinishLatentTask(*Cast<UBehaviorTreeComponent>(AICon->GetBrainComponent()),EBTNodeResult::Failed);
		break;
	}
}

EBTNodeResult::Type UBTTask_MoveRandomPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	
	if (auto character = OwnerComp.GetAIOwner()->GetPawn())
	{
		AICon = OwnerComp.GetAIOwner();
		if(UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(AICon->GetWorld()))
		{
			FNavLocation result;
			if (NavSystem->GetRandomReachablePointInRadius(character->GetActorLocation(), Radius, result))
			{
				DrawDebugPoint(character->GetWorld(), result.Location, 100.f, FColor::Blue);
				OwnerComp.GetAIOwner()->ReceiveMoveCompleted.AddUniqueDynamic(this,&UBTTask_MoveRandomPoint::OnMoveCompleteEvent);
				// 이동 명령을 내립니다.
				OwnerComp.GetAIOwner()->MoveToLocation(result.Location,50.f);
				return EBTNodeResult::InProgress;
			}
		}else
		{
			UE_LOGFMT(LogAICon,Error,"{0} 네비게이션을 가져올 수 없습니다.",GetNameSafe(AICon->GetPawn()));
		}
	}

	UE_LOGFMT(LogAICon,Error,"{0} 랜덤 좌표 생성에 실패했습니다.",GetNameSafe(AICon->GetPawn()));
	return EBTNodeResult::Failed;
}


EBTNodeResult::Type UBTTask_MoveRandomPoint::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UE_LOGFMT(LogTemp,Warning,"랜덤 포인트 이동 중지당함.");
	if (AAIController* AIController = OwnerComp.GetAIOwner())
	{
		AIController->StopMovement();
	}
	
	return Super::AbortTask(OwnerComp, NodeMemory);
}

void UBTTask_MoveRandomPoint::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);

	AICon->ReceiveMoveCompleted.RemoveDynamic(this,&UBTTask_MoveRandomPoint::OnMoveCompleteEvent);
}

FString UBTTask_MoveRandomPoint::GetStaticDescription() const
{
	return TEXT("반지름 ") + FString::FormatAsNumber(Radius) + TEXT("안의 랜덤한 이동가능한 좌표를 가져옵니다.");
}

UBTTask_MoveToDirection::UBTTask_MoveToDirection()
{
	bCreateNodeInstance = true;
}

void UBTTask_MoveToDirection::OnMoveCompleteEvent(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	switch (Result) {
	case EPathFollowingResult::Success:
		FinishLatentTask(*Cast<UBehaviorTreeComponent>(AICon->GetBrainComponent()),EBTNodeResult::Succeeded);
		break;
	case EPathFollowingResult::Blocked:
	case EPathFollowingResult::OffPath:
	case EPathFollowingResult::Aborted:
	case EPathFollowingResult::Skipped_DEPRECATED:
	case EPathFollowingResult::Invalid:
		FinishLatentTask(*Cast<UBehaviorTreeComponent>(AICon->GetBrainComponent()),EBTNodeResult::Failed);
		break;
	}
}

FVector UBTTask_MoveToDirection::GetControlDirLocation(AAIController* AIController, EDirection Direction, float Distance, bool bRandom, float RandMin, float RandMax)
{

	float distance = Distance;
	if(bRandom)
	{
		distance = FMath::RandRange(RandMin,RandMax);
	}
	
	switch (Direction)
	{
	case EDirection::Front:
		return UKismetMathLibrary::GetForwardVector( FRotator(0, AIController->GetControlRotation().Yaw,0)) * distance + AIController->GetPawn()->GetActorLocation();		
	case EDirection::Right:
		return UKismetMathLibrary::GetRightVector(FRotator(0, AIController->GetControlRotation().Yaw,0)) * distance + AIController->GetPawn()->GetActorLocation();
	case EDirection::Left:
		return UKismetMathLibrary::GetRightVector(FRotator(0, AIController->GetControlRotation().Yaw,0)) * distance * -1 + AIController->GetPawn()->GetActorLocation();
	case EDirection::Back:
		return UKismetMathLibrary::GetForwardVector(FRotator(0, AIController->GetControlRotation().Yaw,0)) * distance * -1 + AIController->GetPawn()->GetActorLocation();
	case EDirection::FrontRight:
	case EDirection::FrontLeft:
	case EDirection::BackRight:
	case EDirection::BackLeft:
	case EDirection::Up:
	case EDirection::Down:
	case EDirection::MAX:
		break;
	default: ;
	}

	return FVector::ZeroVector;
}

FVector UBTTask_MoveToDirection::GetDirLocation(AAIController* AIController, EDirection Direction, float Distance, bool bRandom, float RandMin, float RandMax)
{
	float distance = Distance;
	if(bRandom)
	{
		distance = FMath::RandRange(RandMin,RandMax);
	}

	auto pawn = AIController->GetPawn();
	
	switch (Direction)
	{
	case EDirection::Front:
		return pawn->GetActorForwardVector() * distance + pawn->GetActorLocation();		
	case EDirection::Right:
		return pawn->GetActorRightVector() * distance + pawn->GetActorLocation();
	case EDirection::Left:
		return pawn->GetActorRightVector() * distance * -1 +pawn->GetActorLocation();
	case EDirection::Back:
		return pawn->GetActorForwardVector() * distance * -1 + pawn->GetActorLocation();
	case EDirection::FrontRight:
	case EDirection::FrontLeft:
	case EDirection::BackRight:
	case EDirection::BackLeft:
	case EDirection::Up:
	case EDirection::Down:
	case EDirection::MAX:
		break;
	default: ;
	}

	return FVector::ZeroVector;
}

EBTNodeResult::Type UBTTask_MoveToDirection::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (auto character = OwnerComp.GetAIOwner()->GetPawn())
	{
		AICon = OwnerComp.GetAIOwner();

		AICon->ReceiveMoveCompleted.AddUniqueDynamic(this,&UBTTask_MoveToDirection::OnMoveCompleteEvent);

		FVector moveLocation;
		if(bUseControlDirection)
		{
			moveLocation = GetControlDirLocation(AICon,MoveDirection, MoveDistance, bRandomInMoveDistance, RandomRangeMin, RandomRangeMax);
		}else
		{
			moveLocation = GetDirLocation(AICon,MoveDirection, MoveDistance, bRandomInMoveDistance, RandomRangeMin, RandomRangeMax);
		}
		
		// 이동 명령을 내립니다.
		OwnerComp.GetAIOwner()->MoveToLocation(moveLocation,AcceptanceRadius);

		return EBTNodeResult::InProgress;
	}

	return EBTNodeResult::Failed;
}

EBTNodeResult::Type UBTTask_MoveToDirection::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (AAIController* AIController = OwnerComp.GetAIOwner())
	{
		AIController->StopMovement();
	}
	
	return Super::AbortTask(OwnerComp, NodeMemory);
}

FString UBTTask_MoveToDirection::GetStaticDescription() const
{
	return TEXT("특정 방향으로 특정 거리만큼 이동합니다.");
}

void UBTTask_MoveToDirection::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);

	AICon->ReceiveMoveCompleted.RemoveDynamic(this,&UBTTask_MoveToDirection::OnMoveCompleteEvent);
}

UBTTask_JumpMove::UBTTask_JumpMove()
{
	bCreateNodeInstance = true;
	bNotifyTick =true;
	bNotifyTaskFinished = true;
	NodeName = TEXT("점프이동");
}

void UBTTask_JumpMove::OnJumpEnded(const FHitResult& Hit)
{
	AICon->GetPawn<ACharacter>()->LandedDelegate.RemoveDynamic(this,&UBTTask_JumpMove::OnJumpEnded);
	FinishLatentTask(*Cast<UBehaviorTreeComponent>(AICon->GetBrainComponent()),EBTNodeResult::Succeeded);
	
}

EBTNodeResult::Type UBTTask_JumpMove::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (auto character = OwnerComp.GetAIOwner()->GetPawn())
	{
		AICon = OwnerComp.GetAIOwner();
		if(AICon!=nullptr)
		{
			if(UNavigationSystemV1* navSystem = UNavigationSystemV1::GetCurrent(AICon))
			{
				FVector moveLocation;
				if(bUseControlDirection)
				{
					moveLocation = UBTTask_MoveToDirection::GetControlDirLocation(AICon,MoveDirection);
				}else
				{
					moveLocation = UBTTask_MoveToDirection::GetDirLocation(AICon,MoveDirection);
				}

				DrawDebugPoint(AICon->GetWorld(),moveLocation,50.f,FColor::Yellow,true,10.f);
				
				FNavLocation StartNavLocation;
				if(navSystem->ProjectPointToNavigation(moveLocation, StartNavLocation))
				{
					UKismetSystemLibrary::PrintString(this,"JUMP");
					AICon->GetPawn<ACharacter>()->LandedDelegate.AddUniqueDynamic(this,&UBTTask_JumpMove::OnJumpEnded);
					AICon->GetPawn<ACharacter>()->Jump();
					return EBTNodeResult::InProgress;
				}
	
			}
		
		}
	}
	
	return EBTNodeResult::Failed;
}

void UBTTask_JumpMove::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	auto pawn = AICon->GetPawn();
	
	if(bUseControlDirection)
	{
		switch (MoveDirection)
		{
		case EDirection::Front:
			pawn->AddActorWorldOffset(UKismetMathLibrary::GetForwardVector(AICon->GetControlRotation()) * OffsetPower*DeltaSeconds);
			break;
		case EDirection::Right:
			pawn->AddActorWorldOffset(UKismetMathLibrary::GetRightVector(AICon->GetControlRotation()) * OffsetPower*DeltaSeconds);
			break;
		case EDirection::Left:
			pawn->AddActorWorldOffset(UKismetMathLibrary::GetRightVector(AICon->GetControlRotation())*-1 * OffsetPower*DeltaSeconds);
			break;
		case EDirection::Back:
			pawn->AddActorWorldOffset(UKismetMathLibrary::GetForwardVector(AICon->GetControlRotation())*-1 * OffsetPower*DeltaSeconds);
			break;
		case EDirection::FrontRight:
		case EDirection::FrontLeft:
		case EDirection::BackRight:
		case EDirection::BackLeft:
		case EDirection::Up:
		case EDirection::Down:
		case EDirection::MAX:
			break;
		default: ;
		}

	}else
	{	
		switch (MoveDirection)
		{
		case EDirection::Front:
			pawn->AddActorWorldOffset(pawn->GetActorForwardVector());
			break;
		case EDirection::Right:
			pawn->AddActorWorldOffset(pawn->GetActorRightVector());
			break;
		case EDirection::Left:
			pawn->AddActorWorldOffset(pawn->GetActorRightVector() * -1);
			break;
		case EDirection::Back:
			pawn->AddActorWorldOffset(pawn->GetActorForwardVector() * -1);
			break;
		case EDirection::FrontRight:
		case EDirection::FrontLeft:
		case EDirection::BackRight:
		case EDirection::BackLeft:
		case EDirection::Up:
		case EDirection::Down:
		case EDirection::MAX:
			break;
		default: ;
		}
	}
}

FString UBTTask_JumpMove::GetStaticDescription() const
{
	return TEXT("점프합니다. 점프중 OffsetPower만큼 밀어냅니다.");
}

EBTNodeResult::Type UBTTask_JumpMove::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AICon->GetPawn<ACharacter>()->StopJumping();
	return Super::AbortTask(OwnerComp, NodeMemory);
}

void UBTTask_JumpMove::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	EBTNodeResult::Type TaskResult)
{
	if(AICon!=nullptr)
	{
		AICon->GetPawn<ACharacter>()->StopJumping();
	}
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
}
