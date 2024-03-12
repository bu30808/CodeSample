// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/02_Animation/00_NotifyState/AnimNotifyState_AddLocalRotation.h"

#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"


void UAnimNotifyState_AddLocalRotation::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                                    float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	TargetYaw = Degree/EventReference.GetNotify()->Duration;
	TotalYaw = 0;
}

void UAnimNotifyState_AddLocalRotation::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                                   float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
	if (!MeshComp)
	{
		return;
	}
	

	if(auto pawn = MeshComp->GetOwner<ACharacter>())
	{
		if(const auto animInstance = MeshComp->GetAnimInstance())
		{
			const float playRate = animInstance->Montage_GetPlayRate(animInstance->GetCurrentActiveMontage());
			const float addYaw  = ((Degree/EventReference.GetNotify()->Duration)) * playRate * FrameDeltaTime;
			//UKismetSystemLibrary::PrintString(MeshComp,FString::Printf(TEXT("더해야 할 요 : %f , 현재 플레이 속도 : %f"),addYaw,playRate));
			TotalYaw+= FMath::Abs(addYaw);
			if(TotalYaw > FMath::Abs(TargetYaw))
			{
				pawn->SetActorRotation(FRotator(0,TargetYaw,0));
				return;
			}
			pawn->AddActorWorldRotation(FRotator(0,addYaw,0));
			return;
		}
	}

#if WITH_EDITOR
	{
		const float playRate = MeshComp->GetAnimInstance()->Montage_GetPlayRate(MeshComp->GetAnimInstance()->GetCurrentActiveMontage());
		const float addYaw = (Degree/EventReference.GetNotify()->Duration) * playRate * FrameDeltaTime;
		//UKismetSystemLibrary::PrintString(MeshComp,FString::Printf(TEXT("더해야 할 요 : %f , 현재 플레이 속도 : %f"),addYaw,playRate));
		TotalYaw+= FMath::Abs(addYaw);
		if(TotalYaw > FMath::Abs(TargetYaw))
		{
			MeshComp->SetWorldRotation(FRotator(0,TargetYaw,0));
			return;
		}
		MeshComp->AddWorldRotation(FRotator(0,addYaw,0));
	}
#endif

}

void UAnimNotifyState_AddLocalRotation::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	//UKismetSystemLibrary::PrintString(MeshComp,FString::Printf(TEXT("더한 요의 합계 : %f"),TotalYaw));
}

