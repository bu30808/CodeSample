// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/03_Monster/03_AI/00_Common/00_Task/BTTask_MoveTasks.h"

#include "AIController.h"
#include "NavigationSystem.h"
#include "00_Character/02_Animation/00_NotifyState/AnimNotifyState_AddForce.h"
#include "00_Character/03_Monster/00_Controller/MonsterAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/SplineComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
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
	if(!OnAIMoveComplete.ExecuteIfBound())
	{
		UE_LOGFMT(LogAICon,Log,"RandomMoveTo의 AIMove가 끝났지만, 바인드된 함수가 따로 없습니다.");
	}
	/*switch (Result)
	{
	case EPathFollowingResult::Success:
		FinishLatentTask(*Cast<UBehaviorTreeComponent>(AICon->GetBrainComponent()), EBTNodeResult::Succeeded);
		break;
	case EPathFollowingResult::Blocked:
	case EPathFollowingResult::OffPath:
	case EPathFollowingResult::Aborted:
	case EPathFollowingResult::Skipped_DEPRECATED:
	case EPathFollowingResult::Invalid:
		FinishLatentTask(*Cast<UBehaviorTreeComponent>(AICon->GetBrainComponent()), EBTNodeResult::Failed);
		break;
	}*/
}

FVector UBTTask_MoveRandomPoint::GetRandomMoveDirection(APawn* Pawn)
{
	switch (FMath::RandRange(1, 4))
	{
	case 1:
		return Pawn->GetActorForwardVector();
	case 2:
		return Pawn->GetActorForwardVector() * -1;
	case 3:
		return Pawn->GetActorRightVector();
	case 4:
		return Pawn->GetActorRightVector() * -1;
	}

	return Pawn->GetActorForwardVector();
}


EBTNodeResult::Type UBTTask_MoveRandomPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (auto character = OwnerComp.GetAIOwner()->GetPawn())
	{
		auto aiCon = OwnerComp.GetAIOwner();
		UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
		if (NavSystem)
		{
			FVector MoveDirection = GetRandomMoveDirection(character) * Distance;

			FNavLocation result;
			if (NavSystem->GetRandomPointInNavigableRadius(character->GetActorLocation() + MoveDirection, 50.f, result))
			{
				OnAIMoveComplete.BindLambda([&OwnerComp, this,aiCon]()
				{
					aiCon->ReceiveMoveCompleted.RemoveDynamic(this, &UBTTask_MoveRandomPoint::OnMoveCompleteEvent);
					FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
				});

				UE_LOGFMT(LogAICon, Log, "UBTTask_MoveRandomPoint 생성된 좌표 : {0}", result.Location.ToString());
				// 이동 명령을 내립니다.
				aiCon->MoveToLocation(result.Location);
				return EBTNodeResult::InProgress;
			}
		}
		else
		{
			UE_LOGFMT(LogAICon, Error, "UBTTask_MoveRandomPoint 네비 시스템을 가져올 수 없습니다.");
		}
	}

	UE_LOGFMT(LogAICon, Error, "UBTTask_MoveRandomPoint 랜덤 좌표 생성에 실패했습니다.");
	return EBTNodeResult::Failed;
}


EBTNodeResult::Type UBTTask_MoveRandomPoint::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UE_LOGFMT(LogTemp, Warning, "랜덤 포인트 이동 중지당함.");
	if (AAIController* AIController = OwnerComp.GetAIOwner())
	{
		AIController->ReceiveMoveCompleted.RemoveDynamic(this, &UBTTask_MoveRandomPoint::OnMoveCompleteEvent);
		AIController->StopMovement();
	}

	return Super::AbortTask(OwnerComp, NodeMemory);
}


UBTTask_MoveToDirection::UBTTask_MoveToDirection()
{
	bCreateNodeInstance = true;
}

void UBTTask_MoveToDirection::OnMoveCompleteEvent(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	switch (Result)
	{
	case EPathFollowingResult::Success:
		FinishLatentTask(*Cast<UBehaviorTreeComponent>(AICon->GetBrainComponent()), EBTNodeResult::Succeeded);
		break;
	case EPathFollowingResult::Blocked:
	case EPathFollowingResult::OffPath:
	case EPathFollowingResult::Aborted:
	case EPathFollowingResult::Skipped_DEPRECATED:
	case EPathFollowingResult::Invalid:
		FinishLatentTask(*Cast<UBehaviorTreeComponent>(AICon->GetBrainComponent()), EBTNodeResult::Failed);
		break;
	}
}

FVector UBTTask_MoveToDirection::GetControlDirLocation(AAIController* AIController, EDirection Direction,
                                                       float Distance, bool bRandom, float RandMin, float RandMax)
{
	float distance = Distance;
	if (bRandom)
	{
		distance = FMath::RandRange(RandMin, RandMax);
	}

	switch (Direction)
	{
	case EDirection::Front:
		return UKismetMathLibrary::GetForwardVector(FRotator(0, AIController->GetControlRotation().Yaw, 0)) * distance +
			AIController->GetPawn()->GetActorLocation();
	case EDirection::Right:
		return UKismetMathLibrary::GetRightVector(FRotator(0, AIController->GetControlRotation().Yaw, 0)) * distance +
			AIController->GetPawn()->GetActorLocation();
	case EDirection::Left:
		return UKismetMathLibrary::GetRightVector(FRotator(0, AIController->GetControlRotation().Yaw, 0)) * distance * -
			1 + AIController->GetPawn()->GetActorLocation();
	case EDirection::Back:
		return UKismetMathLibrary::GetForwardVector(FRotator(0, AIController->GetControlRotation().Yaw, 0)) * distance *
			-1 + AIController->GetPawn()->GetActorLocation();
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

FVector UBTTask_MoveToDirection::GetDirLocation(AAIController* AIController, EDirection Direction, float Distance,
                                                bool bRandom, float RandMin, float RandMax)
{
	float distance = Distance;
	if (bRandom)
	{
		distance = FMath::RandRange(RandMin, RandMax);
	}

	auto pawn = AIController->GetPawn();

	switch (Direction)
	{
	case EDirection::Front:
		return pawn->GetActorForwardVector() * distance + pawn->GetActorLocation();
	case EDirection::Right:
		return pawn->GetActorRightVector() * distance + pawn->GetActorLocation();
	case EDirection::Left:
		return pawn->GetActorRightVector() * distance * -1 + pawn->GetActorLocation();
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

		AICon->ReceiveMoveCompleted.AddUniqueDynamic(this, &UBTTask_MoveToDirection::OnMoveCompleteEvent);

		FVector moveLocation;
		if (bUseControlDirection)
		{
			moveLocation = GetControlDirLocation(AICon, MoveDirection, MoveDistance, bRandomInMoveDistance,
			                                     RandomRangeMin, RandomRangeMax);
		}
		else
		{
			moveLocation = GetDirLocation(AICon, MoveDirection, MoveDistance, bRandomInMoveDistance, RandomRangeMin,
			                              RandomRangeMax);
		}

		// 이동 명령을 내립니다.
		OwnerComp.GetAIOwner()->MoveToLocation(moveLocation, AcceptanceRadius);

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

	AICon->ReceiveMoveCompleted.RemoveDynamic(this, &UBTTask_MoveToDirection::OnMoveCompleteEvent);
}

UBTTask_JumpMove::UBTTask_JumpMove()
{
	bCreateNodeInstance = true;
	bNotifyTick = true;
	bNotifyTaskFinished = true;
	NodeName = TEXT("점프이동");
}

void UBTTask_JumpMove::OnJumpEnded(const FHitResult& Hit)
{
	AICon->GetPawn<ACharacter>()->LandedDelegate.RemoveDynamic(this, &UBTTask_JumpMove::OnJumpEnded);
	FinishLatentTask(*Cast<UBehaviorTreeComponent>(AICon->GetBrainComponent()), EBTNodeResult::Succeeded);
}

EBTNodeResult::Type UBTTask_JumpMove::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (auto character = OwnerComp.GetAIOwner()->GetPawn())
	{
		AICon = OwnerComp.GetAIOwner();
		if (AICon != nullptr)
		{
			if (UNavigationSystemV1* navSystem = UNavigationSystemV1::GetCurrent(AICon))
			{
				FVector moveLocation;
				if (bUseControlDirection)
				{
					moveLocation = UBTTask_MoveToDirection::GetControlDirLocation(AICon, MoveDirection);
				}
				else
				{
					moveLocation = UBTTask_MoveToDirection::GetDirLocation(AICon, MoveDirection);
				}

				DrawDebugPoint(AICon->GetWorld(), moveLocation, 50.f, FColor::Yellow, true, 10.f);

				FNavLocation StartNavLocation;
				if (navSystem->ProjectPointToNavigation(moveLocation, StartNavLocation))
				{
					UKismetSystemLibrary::PrintString(this, "JUMP");
					AICon->GetPawn<ACharacter>()->LandedDelegate.AddUniqueDynamic(this, &UBTTask_JumpMove::OnJumpEnded);
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

	if (bUseControlDirection)
	{
		switch (MoveDirection)
		{
		case EDirection::Front:
			pawn->AddActorWorldOffset(
				UKismetMathLibrary::GetForwardVector(AICon->GetControlRotation()) * OffsetPower * DeltaSeconds);
			break;
		case EDirection::Right:
			pawn->AddActorWorldOffset(
				UKismetMathLibrary::GetRightVector(AICon->GetControlRotation()) * OffsetPower * DeltaSeconds);
			break;
		case EDirection::Left:
			pawn->AddActorWorldOffset(
				UKismetMathLibrary::GetRightVector(AICon->GetControlRotation()) * -1 * OffsetPower * DeltaSeconds);
			break;
		case EDirection::Back:
			pawn->AddActorWorldOffset(
				UKismetMathLibrary::GetForwardVector(AICon->GetControlRotation()) * -1 * OffsetPower * DeltaSeconds);
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
	else
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
	if (AICon != nullptr)
	{
		AICon->GetPawn<ACharacter>()->StopJumping();
	}
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
}

UBTTask_MoveToWithBlackboardCheck::UBTTask_MoveToWithBlackboardCheck()
{
	bCreateNodeInstance = true;
	bNotifyTick = true;
	BlackboardKey.SelectedKeyName = "Target";
	NodeName = TEXT("이동");
}

EBTNodeResult::Type UBTTask_MoveToWithBlackboardCheck::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return Super::ExecuteTask(OwnerComp, NodeMemory);
}

void UBTTask_MoveToWithBlackboardCheck::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
                                                 float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	if (auto character = OwnerComp.GetAIOwner()->GetPawn<ABaseCharacter>())
	{
		if (OwnerComp.GetBlackboardComponent()->GetValueAsObject(GetSelectedBlackboardKey()) == nullptr)
		{
			UE_LOGFMT(LogAICon, Warning, "블랙보드 타겟이 비어있습니다. MoveTo를 종료합니다.");
			OwnerComp.GetAIOwner()->StopMovement();
			FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		}

		if (character->GetCharacterState() != ECharacterState::NORMAL)
		{
			UE_LOGFMT(LogAICon, Warning, "ECharacterState::NORMAL이 아닙니다. MoveTo를 종료합니다.");
			OwnerComp.GetAIOwner()->StopMovement();
			FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		}
	}
}






UBTTask_MoveAlongSpline::UBTTask_MoveAlongSpline()
{
	bCreateNodeInstance = true;
	bNotifyTick = false;
	NodeName = TEXT("스플라인 경로 이동");
}

void UBTTask_MoveAlongSpline::SaveOriginalValues(ACharacter* character)
{
	OriginalRotationRate = character->GetCharacterMovement()->RotationRate;
	OriginalSmoothFocusInterpSpeed = Cast<AMonsterAIController>(AICon)->GetSmoothFocusInterpSpeed();
	bOriginalOrientRotationToMovement = character->GetCharacterMovement()->bOrientRotationToMovement;
	bOriginalUseControllerDesiredRotation = character->GetCharacterMovement()->bUseControllerDesiredRotation;
	bOriginalUseControllerRotationYaw = character->bUseControllerRotationYaw;
	
}

void UBTTask_MoveAlongSpline::ApplyFocusSetting(ACharacter* character)
{
	if(bUseSmoothFocus)
	{
		Cast<AMonsterAIController>(AICon)->SetSmoothFocusInterpSpeed(SmoothFocusInterpSpeed);
		character->GetCharacterMovement()->RotationRate = RotationRate;
		character->bUseControllerRotationYaw = true;
		character->GetCharacterMovement()->bOrientRotationToMovement = false;
		character->GetCharacterMovement()->bUseControllerDesiredRotation = true;
		AICon->SetActorTickEnabled(true);
	}
			
}

void UBTTask_MoveAlongSpline::RestoreOriginalValues(ACharacter* character)
{
	if(bUseSmoothFocus)
	{
		AICon->K2_ClearFocus();
		AICon->SetActorTickEnabled(false);
	}
	
	Cast<AMonsterAIController>(AICon)->SetSmoothFocusInterpSpeed(OriginalSmoothFocusInterpSpeed);
	character->bUseControllerRotationYaw = bOriginalUseControllerRotationYaw;
	character->GetCharacterMovement()->bOrientRotationToMovement = bOriginalOrientRotationToMovement;
}

EBTNodeResult::Type UBTTask_MoveAlongSpline::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AICon = OwnerComp.GetAIOwner();
	CurMoveSplinePoint = 0;
	

	if (auto character = Cast<ACharacter>(OwnerComp.GetAIOwner()->GetPawn()))
	{
		if (auto comp = character->GetComponentByClass(USplineComponent::StaticClass()))
		{
			//기존 설정값을 저장합니다.
			SaveOriginalValues(character);
			ApplyFocusSetting(character);
			
			if(SplineComp == nullptr)
			{
				SplineComp = Cast<USplineComponent>(comp);
			}

			if(SplineComp->IsAttachedTo(character->GetRootComponent()))
			{
				SplineComp->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld,false));
			}
			
			if (SplineComp.IsValid())
			{
				if (bReverse)
				{
					CurMoveSplinePoint = SplineComp->GetNumberOfSplinePoints() - 1;
				}
				AICon->ReceiveMoveCompleted.AddUniqueDynamic(this, &UBTTask_MoveAlongSpline::OnAIMoveCompleted);
				MoveToNextPoint();
				return EBTNodeResult::InProgress;
			}
		}
	}

	return EBTNodeResult::Failed;
}

EBTNodeResult::Type UBTTask_MoveAlongSpline::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if(AICon.IsValid())
	{
		AICon->StopMovement();
		RestoreOriginalValues(Cast<ACharacter>(AICon->GetPawn()));
		AICon->ReceiveMoveCompleted.RemoveDynamic(this, &UBTTask_MoveAlongSpline::OnAIMoveCompleted);
	}
	return Super::AbortTask(OwnerComp, NodeMemory);
}

void UBTTask_MoveAlongSpline::MoveFinish()
{
	RestoreOriginalValues(Cast<ACharacter>(AICon->GetPawn()));
	AICon->ReceiveMoveCompleted.RemoveDynamic(this, &UBTTask_MoveAlongSpline::OnAIMoveCompleted);
	FinishLatentTask(*Cast<UBehaviorTreeComponent>(AICon->GetBrainComponent()), EBTNodeResult::Succeeded);
}

void UBTTask_MoveAlongSpline::Move(const FVector& nextPoint)
{
	if(bUseSmoothFocus)
	{
		AICon->SetFocalPoint(nextPoint);
	}
	
	AICon->MoveToLocation(nextPoint);
}

void UBTTask_MoveAlongSpline::MoveToNextPoint()
{
	if (AICon.IsValid())
	{
		int32 totalPoint = SplineComp->GetNumberOfSplinePoints();
		if (bReverse)
		{
			if (CurMoveSplinePoint < 0)
			{
				MoveFinish();
				return;
			}
			UE_LOGFMT(LogAICon,Log,"{0}번 포인트로 이동합니다",CurMoveSplinePoint);
			const FVector& nextPoint = SplineComp->GetLocationAtSplinePoint(
				CurMoveSplinePoint, ESplineCoordinateSpace::World);
			CurMoveSplinePoint--;
			Move(nextPoint);
		}
		else
		{
			if (CurMoveSplinePoint > totalPoint)
			{
				MoveFinish();
				return;
			}
			UE_LOGFMT(LogAICon,Log,"{0}번 포인트로 이동합니다",CurMoveSplinePoint);
			const FVector& nextPoint = SplineComp->GetLocationAtSplinePoint(
				CurMoveSplinePoint, ESplineCoordinateSpace::World);
			CurMoveSplinePoint++;
			Move(nextPoint);
		}
	}
}

void UBTTask_MoveAlongSpline::OnAIMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	//UE_LOGFMT(LogAICon,Log,"이동 결과 : {0}",StaticEnum<EPathFollowingResult::Type>()->GetValueAsString(Result));
	MoveToNextPoint();
}









UBTTask_MoveWithLeader::UBTTask_MoveWithLeader()
{
	bCreateNodeInstance = true;
	bNotifyTick = true;
	NodeName = TEXT("리더와 이동");
	BlackboardKey.SelectedKeyName = TEXT("Leader");
	bNotifyTaskFinished = true;
}

void UBTTask_MoveWithLeader::MoveToLeader()
{
	if(AICon.IsValid())
	{
		if(const auto leader = Cast<AActor>( AICon->GetBlackboardComponent()->GetValueAsObject(GetSelectedBlackboardKey())))
		{
			FVector leaderLoc = leader->GetActorLocation();
			FRotator leaderRot = leader->GetActorRotation();
			FVector relativePos = AICon->GetBlackboardComponent()->GetValueAsVector("RelativePositionWithLeader");

			//relativePos를 leaderRot각만큼 회전시킵니다.
			//이 과정으로 리더에 대한 상대 위치를 계속 유지할 수 있습니다.
			FVector targetLoc = leaderLoc + leaderRot.RotateVector(relativePos);
			auto result = AICon->MoveToLocation(targetLoc,AcceptanceRadius);
			UE_LOGFMT(LogAICon,Log,"{0}의 리더 이동 실행 결과 : {1}",AICon->GetPawn()->GetName(),StaticEnum<EPathFollowingRequestResult::Type>()->GetValueAsString(result));
			
		}else
		{
			FinishLatentTask(*Cast<UBehaviorTreeComponent>(AICon->GetBrainComponent()),EBTNodeResult::Failed);
		}
	}
}

void UBTTask_MoveWithLeader::OverrideMoveSpeed()
{
	if(bOverrideToLeaderMoveSpeed)
	{
		if(AICon.IsValid())
		{
			if(const auto leader =Cast<ACharacter>( AICon->GetBlackboardComponent()->GetValueAsObject(GetSelectedBlackboardKey())))
			{
				if (auto character = Cast<ACharacter>(AICon->GetPawn()))
				{
					character->GetCharacterMovement()->MaxWalkSpeed = leader->GetCharacterMovement()->MaxWalkSpeed * SpeedMultiplier;
				}
			}
		}
	}
}

void UBTTask_MoveWithLeader::SaveOriginalValues()
{
	if(AICon.IsValid())
	{
		if (auto character = Cast<ACharacter>(AICon->GetPawn()))
		{
			OriginalMoveSpeed = character->GetCharacterMovement()->MaxWalkSpeed;
		}
	}
}

void UBTTask_MoveWithLeader::RestoreOriginalValues()
{
	if(AICon.IsValid())
	{
		if (auto character = Cast<ACharacter>(AICon->GetPawn()))
		{
			character->GetCharacterMovement()->MaxWalkSpeed = OriginalMoveSpeed;
		}
	}
}

EBTNodeResult::Type UBTTask_MoveWithLeader::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AICon = OwnerComp.GetAIOwner();
	TimeSinceLastUpdate = 0.0f;
	
	if(AICon.IsValid())
	{
		AICon->ReceiveMoveCompleted.AddUniqueDynamic(this,&UBTTask_MoveWithLeader::OnMoveFinish);
		SaveOriginalValues();
		OverrideMoveSpeed();
		/*MoveToLeader();*/
		return EBTNodeResult::InProgress;
	}
	
	return EBTNodeResult::Failed;
}

void UBTTask_MoveWithLeader::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	TimeSinceLastUpdate += DeltaSeconds;
	//UpdateInterval마다 실행하도록 합니다.
	if (TimeSinceLastUpdate >= UpdateInterval)
	{
		TimeSinceLastUpdate = 0.0f;
		
		if(AICon.IsValid())
		{
			MoveToLeader();
		}
		
	}
}

void UBTTask_MoveWithLeader::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
                                            EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
	RestoreOriginalValues();
	OwnerComp.GetAIOwner()->ReceiveMoveCompleted.RemoveDynamic(this,&UBTTask_MoveWithLeader::OnMoveFinish);
}

void UBTTask_MoveWithLeader::OnMoveFinish(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	UE_LOGFMT(LogAICon,Log,"{0}의 이동 결과 : {1}",AICon->GetPawn()->GetName(),StaticEnum<EPathFollowingRequestResult::Type>()->GetValueAsString(Result));
	switch(Result)
	{
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
	default: ;
	}
}
