// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/02_Animation/01_Notify/AnimNotify_FootStep.h"

#include "00_Character/BaseCharacter.h"
#include "00_Character/01_Component/FootStepComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Logging/StructuredLog.h"


void UAnimNotify_FootStep::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                  const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (MeshComp)
	{
		/*
		// 발소리 트리거의 위치와 방향 설정
		FVector startLocation = MeshComp->GetSocketLocation(SocketName);
		FVector endLocation = startLocation + (MeshComp->GetUpVector() * -500.f); // 발 아래로 라인 트레이

		TArray<FHitResult> HitResults;
		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
		TArray<AActor*> ActorsToIgnore;
		bool bHit = UKismetSystemLibrary::LineTraceMultiByProfile(
			MeshComp,
			startLocation,
			endLocation,
			"Pawn",
			false,
			ActorsToIgnore,
			EDrawDebugTrace::ForDuration,
			HitResults, true
		);
		*/


		if (auto character = MeshComp->GetOwner<ABaseCharacter>())
		{
			character->GetFootStepComponent()->MakeFootStep(SocketName);
		}
	}
}
