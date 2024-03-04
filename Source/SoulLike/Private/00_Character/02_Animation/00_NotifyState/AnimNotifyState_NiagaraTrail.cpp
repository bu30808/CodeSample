// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/02_Animation/00_NotifyState/AnimNotifyState_NiagaraTrail.h"

#include "Logging/StructuredLog.h"
#include "NiagaraComponent.h"

UAnimNotifyState_NiagaraTrail::UAnimNotifyState_NiagaraTrail()
{
	SocketName = "Root";
}

void UAnimNotifyState_NiagaraTrail::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                               float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	if (MeshComp)
	{
		if (const auto effect = GetSpawnedEffect(MeshComp))
		{
			effect->SetVectorParameter("StartTrail", MeshComp->GetSocketLocation(Start));
			effect->SetVectorParameter("EndTrail", MeshComp->GetSocketLocation(End));
		}
		else
		{
			UE_LOGFMT(LogTemp, Error, "나이아가라 트레일이 유효하지 않습니다.");
		}
	}
}
