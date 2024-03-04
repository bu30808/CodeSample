// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/00_Player/02_Animation/00_NotifyState/AnimNotifyState_DodgeImmediately.h"

#include "00_Character/00_Player/PlayerCharacter.h"
#include "00_Character/00_Player/00_Controller/00_Component/InputHandlerComponent.h"
#include "00_Character/00_Player/01_Component/LockOnComponent.h"
#include "00_Character/01_Component/AbilityComponent.h"


UAnimNotifyState_DodgeImmediately::UAnimNotifyState_DodgeImmediately()
{
	bIsNativeBranchingPoint = true;
}

void UAnimNotifyState_DodgeImmediately::BranchingPointNotifyBegin(FBranchingPointNotifyPayload& BranchingPointPayload)
{
	Super::BranchingPointNotifyBegin(BranchingPointPayload);

	bPressed = false;

	if (auto player = Cast<APlayerCharacter>(BranchingPointPayload.SkelMeshComponent->GetOwner()))
	{
		HasDodgeTalent = player->GetAbilityComponent()->HasEffectTag(
			FGameplayTag::RequestGameplayTag("Item.Equipment.Orb.CompleteReward.Defence.Knockdown.CanDodge.Effect"));

		if (HasDodgeTalent)
		{
			auto pc = player->GetController<APlayerController>();
			if (const auto InputLocalPlayerSubsystem = ULocalPlayer::GetSubsystem<
				UEnhancedInputLocalPlayerSubsystem>(pc->GetLocalPlayer()))
			{
				Actionkeys = InputLocalPlayerSubsystem->QueryKeysMappedToAction(InputAction);
			}
		}
	}
}

void UAnimNotifyState_DodgeImmediately::BranchingPointNotifyTick(FBranchingPointNotifyPayload& BranchingPointPayload,
                                                                 float FrameDeltaTime)
{
	Super::BranchingPointNotifyTick(BranchingPointPayload, FrameDeltaTime);
	if (!bPressed && HasDodgeTalent)
	{
		if (const auto player = Cast<APlayerCharacter>(BranchingPointPayload.SkelMeshComponent->GetOwner()))
		{
			const auto pc = player->GetController<APlayerController>();
			if (Actionkeys.IsValidIndex(0))
			{
				if (pc->IsInputKeyDown(Actionkeys[0]))
				{
					bPressed = true;
					player->GetLockOnComponent()->SetEnableLockOnRotation(false);
					player->OnWaitKnockDownDodgeKeyPress(FKeyPressedInfo());
				}
			}
		}
	}
}

void UAnimNotifyState_DodgeImmediately::BranchingPointNotifyEnd(FBranchingPointNotifyPayload& BranchingPointPayload)
{
	Super::BranchingPointNotifyEnd(BranchingPointPayload);
}
