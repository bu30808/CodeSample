// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/03_Monster/03_AI/00_Common/00_Task/BTTask_AITurn.h"

#include "00_Character/03_Monster/BaseMonster.h"
#include "00_Character/03_Monster/00_Controller/MonsterAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Tasks/BTTask_MoveTo.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Logging/StructuredLog.h"


UBTTask_AITurn::UBTTask_AITurn()
{
	bCreateNodeInstance = true;

	NodeName = TEXT("회전");
	bNotifyTaskFinished = true;
	bNotifyTick = true;

	BlackboardKey.SelectedKeyName = "Target";
	BlackboardKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_AITurn, BlackboardKey), AActor::StaticClass());
	BlackboardKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_AITurn, BlackboardKey));
}

EBTNodeResult::Type UBTTask_AITurn::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	CurTryCount = 0;
	OwnersAIController = OwnerComp.GetAIOwner();

	switch (TurnType)
	{
	case EAITaskTurnType::RootMontage:
		if (auto instance = OwnersAIController->GetPawn<ABaseMonster>()->GetMesh()->GetAnimInstance())
		{
			instance->OnMontageBlendingOut.AddUniqueDynamic(this, &UBTTask_AITurn::OnTurnMontageBlendOutEvent);
		}
		Turn();
		break;
	case EAITaskTurnType::MontageWithTick:
		if (auto instance = OwnersAIController->GetPawn<ABaseMonster>()->GetMesh()->GetAnimInstance())
		{
			instance->OnMontageBlendingOut.AddUniqueDynamic(this, &UBTTask_AITurn::OnTurnMontageBlendOutEvent);
		}
		bUseTick = true;
		break;
	case EAITaskTurnType::TickWithoutMontage:
		bUseTick = true;
		break;
	}

	if (bUseStopTrace)
	{
		bUseTick = true;
	}

	return EBTNodeResult::InProgress;
}

void UBTTask_AITurn::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	if (!bUseTick)
	{
		return;
	}


	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	if (auto target = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(GetSelectedBlackboardKey())))
	{
		if( TurnType != EAITaskTurnType::RootMontage)
		{
			if (auto aiPawn = OwnersAIController->GetPawn<ABaseCharacter>())
			{
				// 회전 목표 각도를 설정합니다.
				float TargetYaw = UKismetMathLibrary::FindLookAtRotation(aiPawn->GetActorLocation(),
																		 target->GetActorLocation()).Yaw;
				float CurrentYaw = aiPawn->GetActorRotation().Yaw; // 현재 몬스터의 각도

				// 회전 목표 각도와 현재 각도를 비교하여 회전해야 할 각도를 계산합니다.
				//예전에 각 구해다가 -180을 빼거나 더했던 그 작업입니다.
				float DeltaYaw = FMath::FindDeltaAngleDegrees(CurrentYaw, TargetYaw);
				

				FRotator LerpRot = FMath::RInterpTo(aiPawn->GetActorRotation(),
													UKismetMathLibrary::FindLookAtRotation(
														aiPawn->GetActorLocation(), target->GetActorLocation()),
													DeltaSeconds, InterpSpeed);
				LerpRot.Pitch = aiPawn->GetActorRotation().Pitch;
				LerpRot.Roll = aiPawn->GetActorRotation().Roll;
				aiPawn->SetActorRotation(LerpRot);

				//이미 각도 안이면 멈춥니다.
				if (FMath::Abs(DeltaYaw) < StopDeltaYaw)
				{
					if (bShouldManualFinish == false)
					{
						FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
					}
				}
			}
			else
			{
				UE_LOGFMT(LogAICon, Error, "{0} AI의 폰을 가져올 수 없습니다. 종료합니다.", OwnerComp.GetAIOwner()->GetActorNameOrLabel());
				FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
			}
		}

	
		if (bUseStopTrace)
		{
			if (auto aiPawn = OwnersAIController->GetPawn<ABaseCharacter>())
			{
				FVector startLoc;
				FVector endLoc;

				switch (StopTraceType)
				{
				case EStopTraceType::ControlDirection:
					{
						startLoc = UKismetMathLibrary::GetForwardVector(OwnersAIController->GetControlRotation()) +
					aiPawn->GetActorLocation();
						endLoc = startLoc + (UKismetMathLibrary::GetForwardVector(OwnersAIController->GetControlRotation()) *
							20000.f + aiPawn->GetActorLocation());
					}
					break;
				case EStopTraceType::PawnForward:
					{
						startLoc = aiPawn->GetActorLocation();
						endLoc = startLoc + aiPawn->GetActorForwardVector() * 20000.f;
					}
					break;
				case EStopTraceType::MeshForward:
					{
						startLoc = aiPawn->GetActorLocation();
						auto rot = aiPawn->GetMesh()->GetSocketRotation(SocketName);
						rot.Pitch = 0;
						rot.Roll = 0;
						endLoc = startLoc + UKismetMathLibrary::GetForwardVector(rot) * 20000.f;
					}
					break;
				default: ;
				}
		
				TArray<AActor*> ignoreActors;
				ignoreActors.Emplace(Cast<AActor>(ABaseMonster::StaticClass()));

				TArray<FHitResult> hits;
				bool bHit = UKismetSystemLibrary::BoxTraceMulti(target->GetWorld(), startLoc, endLoc,FVector(1,1,500),FRotator::ZeroRotator,
																 UEngineTypes::ConvertToTraceType(ECC_Visibility), false,
																 ignoreActors, EDrawDebugTrace::ForOneFrame, hits, true);

				if (bHit)
				{
					for (auto iter : hits)
					{
						if (iter.GetActor() == target)
						{
							DrawDebugPoint(OwnersAIController->GetWorld(), iter.Location, 50.f, FColor::Red, true, 3.f);
							FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
							break;
						}
					}
				}
			}
		}
	}
	else
	{
		UE_LOGFMT(LogAICon, Error, "{0} 타겟을 가져올 수 없습니다. 종료합니다.", OwnerComp.GetAIOwner()->GetActorNameOrLabel());
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	}
}

void UBTTask_AITurn::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
                                    EBTNodeResult::Type TaskResult)
{
	UE_LOGFMT(LogAICon, Log, "{0}의 회전 테스크 종료!", OwnerComp.GetAIOwner()->GetActorNameOrLabel());
	if(auto pawn = OwnersAIController->GetPawn<ABaseMonster>())
	{
		if (auto instance = pawn->GetMesh()->GetAnimInstance())
		{
			if (instance->Montage_IsPlaying(TurnLeftMontage))
			{
				instance->Montage_Stop(0.25f, TurnLeftMontage);
			}
			if (instance->Montage_IsPlaying(TurnRightMontage))
			{
				instance->Montage_Stop(0.25f, TurnRightMontage);
			}

			instance->OnMontageBlendingOut.RemoveDynamic(this, &UBTTask_AITurn::OnTurnMontageBlendOutEvent);
		}
	}
	bNotifyTick = false;
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
}


void UBTTask_AITurn::Turn()
{
	if (OwnersAIController == nullptr)
	{
		return;
	}

	if (auto OwnerComp = Cast<UBehaviorTreeComponent>(OwnersAIController->GetBrainComponent()))
	{
		if (auto target = Cast<AActor>(
			OwnerComp->GetBlackboardComponent()->GetValueAsObject(GetSelectedBlackboardKey())))
		{
			if (auto aiPawn = OwnerComp->GetAIOwner()->GetPawn<ABaseCharacter>())
			{
				if (bUseTurnMaxTurnCount)
				{
					if (MaxTryCount <= CurTryCount)
					{
						CurTryCount = 0;
						FinishLatentTask(*OwnerComp, EBTNodeResult::Failed);
						return;
					}

					CurTryCount++;
				}

				// 회전 목표 각도를 설정합니다.
				float TargetYaw = UKismetMathLibrary::FindLookAtRotation(
					aiPawn->GetActorLocation(), target->GetActorLocation()).Yaw;
				float CurrentYaw = aiPawn->GetActorRotation().Yaw; // 현재 몬스터의 각도

				// 회전 목표 각도와 현재 각도를 비교하여 회전해야 할 각도를 계산합니다.
				float DeltaYaw = FMath::FindDeltaAngleDegrees(CurrentYaw, TargetYaw);

				//이미 각도 안이면 멈춥니다.
				if (FMath::Abs(DeltaYaw) < StopDeltaYaw)
				{
					FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
					return;
				}


				if (FMath::Abs(DeltaYaw) > 0.1f)
				{
					// 회전 방향을 설정합니다. DeltaYaw의 부호에 따라 회전 방향이 결정됩니다.
					float RotationDirection = FMath::Sign(DeltaYaw);
					if (auto instance = aiPawn->GetMesh()->GetAnimInstance())
					{
						if (RotationDirection > 0)
						{
							CurSelectedMontage = TurnRightMontage;
							instance->Montage_Play(TurnRightMontage);
						}
						else if (RotationDirection < 0)
						{
							CurSelectedMontage = TurnLeftMontage;
							instance->Montage_Play(TurnLeftMontage);
						}
						else
						{
							FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
						}
					}
					else
					{
						UE_LOGFMT(LogAICon, Error, "{0} : 애님 인스턴스를 가져올 수 없습니다.",
						          OwnerComp->GetAIOwner()->GetActorNameOrLabel());
					}
				}
			}
			else
			{
				UE_LOGFMT(LogAICon, Error, "{0} : 폰을 가져올 수 없습니다.", OwnerComp->GetAIOwner()->GetActorNameOrLabel());
			}
		}
		else
		{
			UE_LOGFMT(LogAICon, Error, "{0} : 타겟을 가져올 수 없습니다.", OwnerComp->GetAIOwner()->GetActorNameOrLabel());
			FinishLatentTask(*OwnerComp, EBTNodeResult::Failed);
		}
	}
}


void UBTTask_AITurn::OnTurnMontageBlendOutEvent(UAnimMontage* Montage, bool bInterrupted)
{
	if (OwnersAIController != nullptr)
	{
		if (Montage == CurSelectedMontage)
		{
			Turn();
		}
	}
}

/*
UTickingTest::UTickingTest()
{
	bCreateNodeInstance  =true;
}

EBTNodeResult::Type UTickingTest::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{

	bNotifyTick = bUseTick;
	
	return EBTNodeResult::InProgress;
}

void UTickingTest::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	UE_LOGFMT(LogAICon,Log,"TICK");
}
*/
