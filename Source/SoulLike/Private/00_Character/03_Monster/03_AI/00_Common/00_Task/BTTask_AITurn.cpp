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
	UE_LOGFMT(LogAICon, Log, "{0}의 회전 테스크 시작!", OwnerComp.GetAIOwner()->GetActorNameOrLabel());
	CurTryCount = 0;
	bTickAfterMontage = false;
	OwnersAIController = OwnerComp.GetAIOwner();

	switch (TurnType)
	{
	case EAITaskTurnType::RootMontage:
	case EAITaskTurnType::TickAfterRootMontage:
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
		if (TurnType == EAITaskTurnType::MontageWithTick || TurnType == EAITaskTurnType::TickWithoutMontage || TurnType
			== EAITaskTurnType::TickAfterRootMontage)
		{
			if (TurnType == EAITaskTurnType::TickAfterRootMontage)
			{
				if (!bTickAfterMontage)
				{
					return;
				}
			}

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
				if (FMath::Abs(DeltaYaw) <= StopDeltaYaw)
				{
					if (bShouldManualFinish == false)
					{
						UE_LOGFMT(LogAICon, Log, "{0}{1} : {2} 이미 각도 안이라서 종료합니다! ",__FUNCTION__,__LINE__ ,OwnerComp.GetAIOwner()->GetActorNameOrLabel());
						FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
					}
				}
			}
			else
			{
				UE_LOGFMT(LogAICon, Error, "{0} AI의 폰을 가져올 수 없습니다. 종료합니다.",
				          OwnerComp.GetAIOwner()->GetActorNameOrLabel());
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
						endLoc = startLoc + (UKismetMathLibrary::GetForwardVector(
								OwnersAIController->GetControlRotation()) *
							20000.f + aiPawn->GetActorLocation());
					}
					break;
				case EStopTraceType::PawnForward:
					{
						startLoc = aiPawn->GetActorLocation() + aiPawn->GetActorForwardVector() * ForwardOffset;
						endLoc = startLoc + aiPawn->GetActorForwardVector() * 20000.f;
					}
					break;
				case EStopTraceType::PawnMeshForward:
					{
						startLoc = aiPawn->GetMesh()->GetSocketLocation(SocketName) + aiPawn->GetActorForwardVector() *
							ForwardOffset;
						auto rot = aiPawn->GetMesh()->GetSocketRotation(SocketName);
						rot.Pitch = 0;
						rot.Roll = 0;
						endLoc = startLoc + UKismetMathLibrary::GetForwardVector(rot) * 20000.f;
					}
					break;
				case EStopTraceType::UseOtherMeshComponentForward:
					{
						if (auto meshComp =
							ILinetraceAbilityEffectInterface::Execute_GetPrimitiveComponentForAITurnTrace(aiPawn))
						{
							startLoc = meshComp->GetSocketLocation(SocketName) + aiPawn->GetActorForwardVector() *
								ForwardOffset;
							auto rot = meshComp->GetSocketRotation(SocketName);
							rot.Pitch = 0;
							rot.Roll = 0;
							endLoc = startLoc + UKismetMathLibrary::GetForwardVector(rot) * 20000.f;
						}
						else
						{
							UE_LOGFMT(LogAICon, Error,
							          "다른 메시 컴포넌트를 회전 정지를 위해 사용한다고 설정했지만, 인터페이스 함수에서 해당 메시 컴포넌트를 가져올 수 없습니다.");
						}
					}
					break;
				default: ;
				}


				{
					TArray<FHitResult> hits;
					TArray<TEnumAsByte<EObjectTypeQuery>> objectTypes;
					objectTypes.Emplace(UEngineTypes::ConvertToObjectType(ECC_Player));
					objectTypes.Emplace(UEngineTypes::ConvertToObjectType(ECC_Monster));

					bool bHit = UKismetSystemLibrary::BoxTraceMultiForObjects(target->GetWorld(), startLoc, endLoc,
					                                                          StopTraceBoxHalfExtent,
					                                                          FRotator::ZeroRotator,
					                                                          objectTypes, false,
					                                                          TArray<AActor*>(),
					                                                          EDrawDebugTrace::ForOneFrame, hits,
					                                                          true);
					if (bHit)
					{
						for (const auto& hit : hits)
						{
							if (hit.GetActor() == target)
							{
								if (!bShouldManualFinish)
								{
									UE_LOGFMT(LogAICon, Warning, "트레이스에 타겟을 감지했습니다. 회전을 종료합니다.");
									FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
									break;
								}
							}
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
	if (auto pawn = OwnersAIController->GetPawn<ABaseMonster>())
	{
		if (auto instance = pawn->GetMesh()->GetAnimInstance())
		{
			if (instance->Montage_IsPlaying(TurnLeftMontage))
			{
				UE_LOGFMT(LogAICon, Log, "{0}의 왼쪽 회전 테스크 종료!", OwnerComp.GetAIOwner()->GetActorNameOrLabel());
				instance->Montage_Stop(0.25f, TurnLeftMontage);
			}
			if (instance->Montage_IsPlaying(TurnRightMontage))
			{
				UE_LOGFMT(LogAICon, Log, "{0}의 오른쪽 회전 테스크 종료!", OwnerComp.GetAIOwner()->GetActorNameOrLabel());
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
		UE_LOGFMT(LogAICon, Error, "AIController가 NULLPTR입니다.");
		return;
	}
	UE_LOGFMT(LogAICon, Log, "{0} TURN", OwnersAIController->GetActorNameOrLabel());

	if (auto btComp = Cast<UBehaviorTreeComponent>(OwnersAIController->GetBrainComponent()))
	{
		if (auto target = Cast<AActor>(
			btComp->GetBlackboardComponent()->GetValueAsObject(GetSelectedBlackboardKey())))
		{
			if (auto aiPawn = btComp->GetAIOwner()->GetPawn<ABaseCharacter>())
			{
				if (bUseTurnMaxTurnCount)
				{
					if (MaxTryCount <= CurTryCount)
					{
						CurTryCount = 0;
						FinishLatentTask(*btComp, EBTNodeResult::Failed);
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
				float AbsDeltaYaw = FMath::Abs(DeltaYaw);

				//이미 각도 안이면 멈춥니다.
				if (AbsDeltaYaw < StopDeltaYaw)
				{
					if (TurnType == EAITaskTurnType::TickAfterRootMontage)
					{
						UE_LOGFMT(LogAICon, Warning, "이미 목표 각도 안이므로 회전을 중지하고, 틱으로 넘깁니다. : {0}", AbsDeltaYaw);
						bUseTick = true;
						bTickAfterMontage = true;
					}
					else
					{
						FinishLatentTask(*btComp, EBTNodeResult::Succeeded);
					}

					return;
				}

				//각도 안이 아니고, 일정 크기 이상이라면,
				if (AbsDeltaYaw > 0.1f)
				{
					UE_LOGFMT(LogAICon, Warning, "목표 회전 Yaw : {0}", AbsDeltaYaw);

					// 회전 방향을 설정합니다. DeltaYaw의 부호에 따라 회전 방향이 결정됩니다.
					float RotationDirection = FMath::Sign(DeltaYaw);
					if (auto instance = aiPawn->GetMesh()->GetAnimInstance())
					{
						if (TurnRight180Montage != nullptr && TurnLeft180Montage != nullptr)
						{
							if (AbsDeltaYaw >= 150.f)
							{
								if (RotationDirection > 0)
								{
									CurSelectedMontage = TurnRight180Montage;
									instance->Montage_Play(CurSelectedMontage);
									UE_LOGFMT(LogAICon, Log, "오른쪽 180 회전");
								}
								else if (RotationDirection < 0)
								{
									CurSelectedMontage = TurnLeft180Montage;
									instance->Montage_Play(CurSelectedMontage);
									UE_LOGFMT(LogAICon, Log, "왼쪽 180 회전");
								}
								else
								{
									UE_LOGFMT(LogAICon, Warning, "델타값이 0이라 종료합니다. : {0}", DeltaYaw);
									FinishLatentTask(*btComp, EBTNodeResult::Succeeded);
								}
								return;
							}
						}


						if (AbsDeltaYaw >= 90.f)
						{
							if (RotationDirection > 0)
							{
								CurSelectedMontage = TurnRightMontage;
								instance->Montage_Play(CurSelectedMontage);
								UE_LOGFMT(LogAICon, Log, "오른쪽 회전");
							}
							else if (RotationDirection < 0)
							{
								CurSelectedMontage = TurnLeftMontage;
								instance->Montage_Play(CurSelectedMontage);
								UE_LOGFMT(LogAICon, Log, "왼쪽 회전");
							}
							else
							{
								UE_LOGFMT(LogAICon, Warning, "델타값이 0이라 종료합니다. : {0}", DeltaYaw);
								FinishLatentTask(*btComp, EBTNodeResult::Succeeded);
							}
						}
						else
						{
							UE_LOGFMT(LogAICon, Warning, "회전해야 할 델타값이 90 이하입니다. 틱을 사용합니다. : {0}", AbsDeltaYaw);
							bUseTick = true;
							bTickAfterMontage = true;
						}
					}
					else
					{
						UE_LOGFMT(LogAICon, Error, "{0} : 애님 인스턴스를 가져올 수 없습니다.",
						          btComp->GetAIOwner()->GetActorNameOrLabel());
					}
				}else{
					UE_LOGFMT(LogAICon, Warning, "델타값이 0이라 종료합니다. : {0}", DeltaYaw);
					FinishLatentTask(*btComp, EBTNodeResult::Succeeded);
				}
			}
			else
			{
				UE_LOGFMT(LogAICon, Error, "{0} : 폰을 가져올 수 없습니다.", btComp->GetAIOwner()->GetActorNameOrLabel());
			}
		}
		else
		{
			UE_LOGFMT(LogAICon, Error, "{0} : 타겟을 가져올 수 없습니다.", btComp->GetAIOwner()->GetActorNameOrLabel());
			FinishLatentTask(*btComp, EBTNodeResult::Failed);
		}
	}else
	{
		UE_LOGFMT(LogAICon, Error, "{0} 비헤이비어트리 컴포넌트를 가져올 수 없습니다.", OwnersAIController->GetActorNameOrLabel());
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
