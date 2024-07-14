// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/02_Animation/00_NotifyState/AnimNotifyState_GettingSlower.h"

#include "00_Character/BaseCharacter.h"
#include "00_Character/01_Component/AttributeComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

UAnimNotifyState_GettingSlower::UAnimNotifyState_GettingSlower()
{
	UpdateSpeed.BindDynamic(this, &UAnimNotifyState_GettingSlower::OnUpdateSpeedTimeLine);
	OnFinishSpeed.BindDynamic(this, &UAnimNotifyState_GettingSlower::OnFinishSpeedTimeLine);
}

void UAnimNotifyState_GettingSlower::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                                 float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if(MeshComp->GetOwner()!=nullptr)
	{
		Character = MeshComp->GetOwner<ABaseCharacter>();
		
		if(Character!=nullptr)
		{
			if(TimelineComp==nullptr)
			{
				TimelineComp = NewObject<UTimelineComponent>(Character);
				TimelineComp->RegisterComponent();

				TimelineComp->AddInterpFloat(SpeedCurve, UpdateSpeed);
				TimelineComp->SetTimelineFinishedFunc(OnFinishSpeed);
			}
			TimelineComp->PlayFromStart();
		}
	}
}

void UAnimNotifyState_GettingSlower::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	if(TimelineComp)
	{
		TimelineComp->DestroyComponent();
		TimelineComp = nullptr;
	}
	
	if(Character != nullptr)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = Character->GetAttributeComponent()->GetMoveSpeed();
	}
}

void UAnimNotifyState_GettingSlower::OnUpdateSpeedTimeLine(float Output)
{
	if(Character != nullptr)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = Character->GetAttributeComponent()->GetMoveSpeed() * Output;
	}
}

void UAnimNotifyState_GettingSlower::OnFinishSpeedTimeLine()
{
	
}
