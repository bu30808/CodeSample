// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/03_Monster/04_Wolf/Wolf.h"


class AAIController;

/*
void AWolf::TriggerHitAnimation_Implementation(UAbilityEffectAdditionalInformation* AdditionalInformation)
{
	Super::TriggerHitAnimation_Implementation(AdditionalInformation);


}

void AWolf::OnHitMontageBlendOutEvent(UAnimMontage* Montage, bool bInterrupted)
{
	TArray<UAnimMontage*> montages;
	HitMontages.GenerateValueArray(montages);
	if (montages.Contains(Montage))
	{
		Cast<UMonsterAnimInstance>(GetMesh()->GetAnimInstance())->AnimNotify_OnHitExit();
		GetMesh()->GetAnimInstance()->OnMontageBlendingOut.RemoveDynamic(this, &AWolf::OnHitMontageBlendOutEvent);
	}
}
*/
