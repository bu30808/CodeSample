// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/00_Player/02_Animation/00_NotifyState/AnimNotifyState_DisableInput.h"

#include "00_Character/00_Player/PlayerCharacter.h"
#include "00_Character/00_Player/00_Controller/UserController.h"
#include "00_Character/00_Player/00_Controller/00_Component/InputHandlerComponent.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Components/SkeletalMeshComponent.h"
#include "Logging/StructuredLog.h"
#include "SoulLike/SoulLike.h"

UAnimNotifyState_DisableInput::UAnimNotifyState_DisableInput()
{
	bIsNativeBranchingPoint = true;
}

void UAnimNotifyState_DisableInput::BranchingPointNotifyBegin(FBranchingPointNotifyPayload& BranchingPointPayload)
{
	Super::BranchingPointNotifyBegin(BranchingPointPayload);
	if (BranchingPointPayload.SkelMeshComponent != nullptr)
	{
		if (auto player = BranchingPointPayload.SkelMeshComponent->GetOwner<APlayerCharacter>())
		{
			UE_LOGFMT(LogTemp, Warning, "{0} {1} : 플레이어 입력을 막습니다, 입력금지 노티파이 ", __FUNCTION__, __LINE__);
			if (auto pc = Cast<AUserController>(player->GetController()))
			{
				switch (DisableInputType)
				{
				case EDisableInputType::DisableLookInput:
					pc->SetIgnoreLookInput(true);
					break;
				case EDisableInputType::DisableMoveInput:
					//pc->SetIgnoreMoveInput(true);
					player->SetIgnoreMoveInput(true, player,
					                           FGameplayTag::RequestGameplayTag(
						                           "Common.Passive.IgnoreMoveInput.Animation.NotifyState.DisableInput"));
					break;
				case EDisableInputType::Both:
					player->DisableInput(pc);
					break;
				default: ;
				}
			}
		}
	}
}

void UAnimNotifyState_DisableInput::BranchingPointNotifyEnd(FBranchingPointNotifyPayload& BranchingPointPayload)
{
	Super::BranchingPointNotifyEnd(BranchingPointPayload);

	if (BranchingPointPayload.SkelMeshComponent != nullptr)
	{
		if (auto player = BranchingPointPayload.SkelMeshComponent->GetOwner<APlayerCharacter>())
		{
			if (auto pc = Cast<AUserController>(player->GetController()))
			{
				switch (DisableInputType)
				{
				case EDisableInputType::DisableLookInput:
					pc->SetIgnoreLookInput(false);
					break;
				case EDisableInputType::DisableMoveInput:
					//pc->SetIgnoreMoveInput(false);
					player->SetIgnoreMoveInput(false, player,
					                           FGameplayTag::RequestGameplayTag(
						                           "Common.Passive.IgnoreMoveInput.Animation.NotifyState.DisableInput"));
					break;
				case EDisableInputType::Both:
					player->DisableInput(pc);
					break;
				default: ;
				}

				UE_LOGFMT(LogTemp, Warning, "{0} {1} : 플레이어 막힌 입력을 되돌립니다. 입력금지 노티파이", __FUNCTION__, __LINE__);
			}
		}
	}
}
