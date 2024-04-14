// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/03_Monster/05_Lich/Lich.h"

#include "00_Character/01_Component/AbilityComponent.h"

ALich::ALich()
{
}

void ALich::PlayIdleCue()
{
	AbilityComponent->ApplyCue(IdleCueInformation);
	GetWorldTimerManager().SetTimer(IdleCueTimerHandle, this, &ALich::ApplyIdleCueEvent, 8.f, true);
}

void ALich::BeginPlay()
{
	Super::BeginPlay();

	if (AbilityComponent)
	{
		PlayIdleCue();
	}
}

void ALich::ApplyIdleCueEvent()
{
	if (IsDead())
	{
		GetWorldTimerManager().ClearTimer(IdleCueTimerHandle);
	}
	else
	{
		AbilityComponent->ApplyCue(IdleCueInformation);
	}
}

void ALich::Activate()
{
	Super::Activate();
	PlayIdleCue();
}

void ALich::Deactivate()
{
	Super::Deactivate();
	GetWorldTimerManager().ClearTimer(IdleCueTimerHandle);
}

void ALich::RestoreComponentAttachment() const
{
	Super::RestoreComponentAttachment();
}

void ALich::OnDeadMontageEndedEvent(UAnimMontage* Montage, bool bInterrupted)
{
	Super::OnDeadMontageEndedEvent(Montage, bInterrupted);
	
	GetMesh()->SetVisibility(false);
}
