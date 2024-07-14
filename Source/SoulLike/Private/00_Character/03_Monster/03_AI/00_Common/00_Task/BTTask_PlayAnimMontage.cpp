// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/03_Monster/03_AI/00_Common/00_Task/BTTask_PlayAnimMontage.h"

#include "AIController.h"
#include "00_Character/BaseCharacter.h"
#include "00_Character/01_Component/AnimationHelperComponent.h"
#include "00_Character/01_Component/AttributeComponent.h"
#include "00_Character/02_Animation/BaseAnimInstance.h"
#include "00_Character/03_Monster/00_Controller/MonsterAIController.h"
#include "Logging/StructuredLog.h"

UBTTask_PlayAnimMontage::UBTTask_PlayAnimMontage()
{
	bCreateNodeInstance = true;
	bNotifyTaskFinished = true;
	NodeName = "PlayMontage";
	/*bNotifyTick = true;*/
}

EBTNodeResult::Type UBTTask_PlayAnimMontage::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (MontageToPlay)
	{
		AIController = OwnerComp.GetAIOwner();

		if (auto monster = OwnerComp.GetAIOwner()->GetPawn<ABaseCharacter>())
		{
			if(bForceTriggerHitExitEvent)
			{
				if (!monster->GetAnimationHelperComponent()->GetIsTriggeredHitAnimationExitEvent())
				{
					UE_LOGFMT(LogAICon, Log, "{0} {1} : OnTriggerHitAnimationExit이벤트가 발동하지 않은 상태임으로 강제로 브로드케스트 합니다.",
							  __FUNCTION__, __LINE__);
					monster->GetAnimationHelperComponent()->OnTriggerHitAnimationExit.Broadcast(monster, nullptr);
				}
			}

			if (auto instance = Cast<UBaseAnimInstance>(monster->GetMesh()->GetAnimInstance()))
			{
				bOriginalMirror = instance->IsMirrorAnimation();

				instance->SetMirrorAnimation(bMirrorAnimation);

				if (!bEnableRootMotion)
				{
					instance->RootMotionMode = ERootMotionMode::IgnoreRootMotion;
				}

				float playSpeed = 1.f;
				if (bUseCustomPlayRate)
				{
					playSpeed = PlaySpeed;
				}
				else
				{
					playSpeed = monster->GetAttributeComponent()->GetActionSpeed();
				}

				if (bNonBlocking)
				{
					if(playSpeed <=0.f)
					{
						UE_LOGFMT(LogAICon,Error,"{0}의 행동속도가 0이라 몽타주를 재생할 수 없습니다. 어트리뷰트 설정을 확인하세요.",monster->GetName());
						return EBTNodeResult::Failed;
					}
					
					instance->Montage_Play(MontageToPlay, playSpeed);
					return EBTNodeResult::Succeeded;
				}


				switch (MontageFinishType)
				{
				case EMontageEndType::ENDED:
					instance->OnMontageEnded.AddUniqueDynamic(
						this, &UBTTask_PlayAnimMontage::OnMontageEndEvent);
					break;
				case EMontageEndType::BLEND_OUT:
					instance->OnMontageBlendingOut.AddUniqueDynamic(
						this, &UBTTask_PlayAnimMontage::OnMontageEndEvent);
					break;
				default: ;
				}

				if(instance->Montage_Play(MontageToPlay, playSpeed) ==0.f)
				{
					UE_LOGFMT(LogAICon, Error, "몽타주 재생에 실패했습니다.");
				}
				return EBTNodeResult::InProgress;
			}
			
			UE_LOGFMT(LogAICon, Error, "몽타주를 재생할 인스턴스를 가져올 수 없어요!!");
		}
	}
	UE_LOGFMT(LogAICon, Error, "플레이할 몽타주가 없어요!");

	return EBTNodeResult::Failed;
}

void UBTTask_PlayAnimMontage::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
                                             EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
	if (auto aiCon = OwnerComp.GetAIOwner())
	{
		// Root Motion 모드를 복구
		if (auto pawn = aiCon->GetPawn<ABaseCharacter>())
		{
			if (auto instance = Cast<UBaseAnimInstance>(pawn->GetMesh()->GetAnimInstance()))
			{
				instance->SetMirrorAnimation(bOriginalMirror);
				instance->RootMotionMode = ERootMotionMode::RootMotionFromMontagesOnly;
			}
		}
	}
}

void UBTTask_PlayAnimMontage::OnMontageEndEvent(UAnimMontage* Montage, bool bInterrupted)
{
	if (MontageToPlay == Montage)
	{
		if (AIController != nullptr)
		{
			if (auto pawn = AIController->GetPawn<ABaseCharacter>())
			{
				if (auto instance = pawn->GetMesh()->GetAnimInstance())
				{
					switch (MontageFinishType)
					{
					case EMontageEndType::ENDED:
						instance->OnMontageEnded.RemoveAll(this);
						break;
					case EMontageEndType::BLEND_OUT:
						instance->OnMontageBlendingOut.RemoveAll(this);
						break;
					default: ;
					}
					UE_LOGFMT(LogAICon,Log,"몽타주 재생 테스크 종료");
					FinishLatentTask(*Cast<UBehaviorTreeComponent>(AIController->GetBrainComponent()),
					                 EBTNodeResult::Succeeded);
				}
			}
		}
	}
}
