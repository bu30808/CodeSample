// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/03_Monster/03_AI/00_Common/00_Task/BTTask_RunAwayFromPlayer.h"

#include "AIController.h"
#include "NavigationSystem.h"
#include "00_Character/03_Monster/BaseMonster.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Logging/StructuredLog.h"
#include "Navigation/PathFollowingComponent.h"

UBTTask_RunAwayFromPlayer::UBTTask_RunAwayFromPlayer()
{
	NodeName = TEXT("도주");
	bNotifyTick = true;
	bNotifyTaskFinished = true;
	BlackboardKey.SelectedKeyName = "Target";
}

EBTNodeResult::Type UBTTask_RunAwayFromPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (auto character = Cast<ABaseCharacter>(OwnerComp.GetAIOwner()->GetPawn()))
	{
		/*character->bUseControllerRotationYaw = !character->bUseControllerRotationYaw;
		character->GetCharacterMovement()->bOrientRotationToMovement = !character->GetCharacterMovement()->bOrientRotationToMovement ;*/

		return EBTNodeResult::InProgress;
	}

	return EBTNodeResult::Failed;
}

void UBTTask_RunAwayFromPlayer::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	if (auto character = Cast<ABaseCharacter>(OwnerComp.GetAIOwner()->GetPawn()))
	{
		//플레이어를 가져옵니다.
		if (auto target = OwnerComp.GetBlackboardComponent()->GetValueAsObject(GetSelectedBlackboardKey()))
		{
			auto targetLocation = Cast<AActor>(target)->GetActorLocation();
			auto lookRot = UKismetMathLibrary::FindLookAtRotation(targetLocation, character->GetActorLocation());
			lookRot.Pitch = 0;
			lookRot.Roll = 0;

			OwnerComp.GetAIOwner()->SetFocalPoint(
				UKismetMathLibrary::GetForwardVector(lookRot) + character->GetActorLocation());

			const FRotator Rotation = OwnerComp.GetAIOwner()->GetControlRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);
			// get forward vector
			const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
			// get right vector 
			const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
			// add movement 
			character->AddMovementInput(ForwardDirection);
			character->AddMovementInput(RightDirection);

			UKismetSystemLibrary::DrawDebugPoint(
				this, (ForwardDirection + RightDirection) * 100.f + character->GetActorLocation(), 25.f,
				FLinearColor::Yellow);
		}
		else
		{
			OwnerComp.GetAIOwner()->K2_ClearFocus();
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		}
	}
}

FString UBTTask_RunAwayFromPlayer::GetStaticDescription() const
{
	return TEXT("플레이어 반대방향으로 이동합니다. 플레이어를 잊을때까지.");
}

void UBTTask_RunAwayFromPlayer::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
                                               EBTNodeResult::Type TaskResult)
{
	if (auto character = Cast<ABaseCharacter>(OwnerComp.GetAIOwner()->GetPawn()))
	{
		/*character->bUseControllerRotationYaw = !character->bUseControllerRotationYaw ;
		character->GetCharacterMovement()->bOrientRotationToMovement = !character->GetCharacterMovement()->bOrientRotationToMovement;*/
	}

	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
}
