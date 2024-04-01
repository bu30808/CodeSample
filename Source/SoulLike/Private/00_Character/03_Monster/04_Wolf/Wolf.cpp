// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/03_Monster/04_Wolf/Wolf.h"

#include "00_Character/01_Component/AbilityComponent.h"
#include "00_Character/03_Monster/00_Controller/MonsterAIController.h"
#include "00_Character/03_Monster/02_Animation/MonsterAnimInstance.h"

class AAIController;

void AWolf::TriggerHitAnimation_Implementation(UAbilityEffectAdditionalInformation* AdditionalInformation)
{
	Super::TriggerHitAnimation_Implementation(AdditionalInformation);

	//전방히트는 애니메이션 인스턴스에서 처리하게 됩니다.
	GetMesh()->GetAnimInstance()->OnMontageBlendingOut.AddUniqueDynamic(this, &AWolf::OnHitMontageBlendOutEvent);

	//좌?
	if (-45 > HitDegree && HitDegree > -135)
	{
		PlayAnimMontage(HitMontages[EDirection::Left]);
	}

	//우?
	if (45 < HitDegree && HitDegree < 135)
	{
		PlayAnimMontage(HitMontages[EDirection::Right]);
	}

	//뒤
	{
		//좌
		if (-135 >= HitDegree && HitDegree >= -180)
		{
			PlayAnimMontage(HitMontages[EDirection::BackLeft]);
		}
		//우
		if (135 <= HitDegree && HitDegree <= 180)
		{
			PlayAnimMontage(HitMontages[EDirection::BackRight]);
		}
	}
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
