// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/01_Component/AnimationHelperComponent.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "00_Character/BaseCharacter.h"
#include "00_Character/01_Component/AbilityComponent.h"
#include "00_Character/03_Monster/00_Controller/MonsterAIController.h"
#include "96_Library/AbilityHelperLibrary.h"
#include "96_Library/MathHelperLibrary.h"
#include "Logging/StructuredLog.h"

// Sets default values for this component's properties
UAnimationHelperComponent::UAnimationHelperComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UAnimationHelperComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	ComponentOwnerCharacter = GetOwner<ABaseCharacter>();
	if (ComponentOwnerCharacter)
	{
		ComponentOwnerCharacter->GetDeadDissolveTimeLineComponent()->SetTimelineLength(DissolveTime);
		UpdateDissolve.BindDynamic(ComponentOwnerCharacter, &ABaseCharacter::OnUpdateDeadDissolveTimeLine);
		OnFinishDissolve.BindDynamic(ComponentOwnerCharacter, &ABaseCharacter::OnFinishDissolveTimeLine);
	}
}

void UAnimationHelperComponent::PostInitProperties()
{
	Super::PostInitProperties();
}

void UAnimationHelperComponent::ActivateHitMightyAbility(float MightyDeactivateTime)
{
	if (ComponentOwnerCharacter)
	{
		if (!ComponentOwnerCharacter->GetWorldTimerManager().TimerExists(HitMightyTimerHandle))
		{
			ComponentOwnerCharacter->GetAbilityComponent()->ActivateAbility(MIGHTY_TAG, ComponentOwnerCharacter);
			ComponentOwnerCharacter->GetWorldTimerManager().SetTimer(HitMightyTimerHandle,
			                                                         ComponentOwnerCharacter.Get(),
			                                                         &ABaseCharacter::DeactivateMightyAbility,
			                                                         MightyDeactivateTime, false);
		}
	}
}

void UAnimationHelperComponent::PlayHitMontage()
{
	if (ComponentOwnerCharacter)
	{
		if (HitAnimationType == EHitAnimationType::AnimMontage)
		{
			ComponentOwnerCharacter->GetMesh()->GetAnimInstance()->OnMontageEnded.AddUniqueDynamic(
				this, &UAnimationHelperComponent::OnHitMontageEnded);
			ComponentOwnerCharacter->GetMesh()->GetAnimInstance()->OnMontageBlendingOut.AddUniqueDynamic(
				this, &UAnimationHelperComponent::OnHitMontageBlendOut);
			if (bResetHitMontageWhenHit)
			{
				UE_LOGFMT(LogTemp, Log, "히트 몽타주 : {0} {1}", __FUNCTION__, __LINE__);
				if (IsPlayHitMontage())
				{
					UE_LOGFMT(LogTemp, Log, "히트 몽타주 : {0} {1}", __FUNCTION__, __LINE__);
					ComponentOwnerCharacter->GetMesh()->GetAnimInstance()->Montage_Stop(0.1f);
				}
			}
			if (!IsPlayHitMontage())
			{
				PlayHitMontageByDirection();
			}
		}
	}
}

void UAnimationHelperComponent::PlayHitMontageByDirection()
{
	if (ComponentOwnerCharacter)
	{
		UE_LOGFMT(LogTemp, Log, "히트 몽타주 : {0} {1}", __FUNCTION__, __LINE__);
		if (HitAnimationType != EHitAnimationType::AnimMontage)
		{
			return;
		}

		if (!bUseDirection)
		{
			UE_LOGFMT(LogTemp, Log, "히트 몽타주 : {0} {1}", __FUNCTION__, __LINE__);
			ComponentOwnerCharacter->GetMesh()->GetAnimInstance()->Montage_Play(HitMontage);
			return;
		}

		const auto& dir = UMathHelperLibrary::DegreeToDirection(HitDegree);
		if (HitMontageByDirection.Contains(dir))
		{
			ComponentOwnerCharacter->PlayAnimMontage(HitMontageByDirection[dir]);
			return;
		}
		switch (dir)
		{
		case EDirection::Front:
			if (HitMontageByDirection.Contains(EDirection::FrontLeft) || HitMontageByDirection.Contains(
				EDirection::FrontRight))
			{
				if (FMath::RandBool())
				{
					ComponentOwnerCharacter->PlayAnimMontage(HitMontageByDirection[EDirection::FrontLeft]);
				}
				else
				{
					ComponentOwnerCharacter->PlayAnimMontage(HitMontageByDirection[EDirection::FrontRight]);
				}
			}
			return;
		case EDirection::Back:
			if (HitMontageByDirection.Contains(EDirection::BackLeft) || HitMontageByDirection.Contains(
				EDirection::BackRight))
			{
				if (FMath::RandBool())
				{
					ComponentOwnerCharacter->PlayAnimMontage(HitMontageByDirection[EDirection::BackLeft]);
				}
				else
				{
					ComponentOwnerCharacter->PlayAnimMontage(HitMontageByDirection[EDirection::BackRight]);
				}
			}
			return;
		}


		switch (dir)
		{
		case EDirection::FrontRight:
		case EDirection::FrontLeft:
			ComponentOwnerCharacter->PlayAnimMontage(HitMontageByDirection[EDirection::Front]);
			return;
		case EDirection::BackRight:
		case EDirection::BackLeft:
			ComponentOwnerCharacter->PlayAnimMontage(HitMontageByDirection[EDirection::Back]);
			return;
		}


		UE_LOGFMT(LogTemp, Error, "이 방향에 대한 피격 애니메이션을 찾을 수 없습니다 : {0}",
		          StaticEnum<EDirection>()->GetValueAsString(dir));

		/*
		//전
		{
			
			if(HitMontageByDirection.Contains(EDirection::Front))
			{
				ComponentOwnerCharacter->PlayAnimMontage(HitMontageByDirection[EDirection::Front]);
				return;
			}
		
			//전 우
			if (0 < HitDegree && HitDegree <= 45)
			{
				ComponentOwnerCharacter->PlayAnimMontage(HitMontageByDirection[EDirection::FrontRight]);
			}
			//전 좌
			if (-45 < HitDegree && HitDegree <= 0)
			{
				ComponentOwnerCharacter->PlayAnimMontage(HitMontageByDirection[EDirection::FrontLeft]);
			}
		}
		//좌
		if (-135 < HitDegree && HitDegree <= -45)
		{
			ComponentOwnerCharacter->PlayAnimMontage(HitMontageByDirection[EDirection::Left]);
		}
		//우
		if (45 < HitDegree && HitDegree <= 135)
		{
			ComponentOwnerCharacter->PlayAnimMontage(HitMontageByDirection[EDirection::Right]);
		}

		//뒤
		{
			if(HitMontageByDirection.Contains(EDirection::Back))
			{
				ComponentOwnerCharacter->PlayAnimMontage(HitMontageByDirection[EDirection::Back]);
				return;
			}
			
		
			//좌
			if (-180 < HitDegree && HitDegree <= -135)
			{
				ComponentOwnerCharacter->PlayAnimMontage(HitMontageByDirection[EDirection::BackLeft]);
			}
			//우
			if (135 < HitDegree && HitDegree <= 180)
			{
				ComponentOwnerCharacter->PlayAnimMontage(HitMontageByDirection[EDirection::BackRight]);
			}
		}*/
	}
}

void UAnimationHelperComponent::PlayGuardHitMontage()
{
	if (ComponentOwnerCharacter)
	{
		if (HitAnimationType == EHitAnimationType::AnimMontage)
		{
			ComponentOwnerCharacter->GetMesh()->GetAnimInstance()->OnMontageEnded.AddUniqueDynamic(
				this, &UAnimationHelperComponent::OnHitMontageEnded);
			ComponentOwnerCharacter->GetMesh()->GetAnimInstance()->OnMontageBlendingOut.AddUniqueDynamic(
				this, &UAnimationHelperComponent::OnHitMontageBlendOut);

			if (bResetHitMontageWhenHit)
			{
				if (IsPlayGuardHitMontage())
				{
					ComponentOwnerCharacter->GetMesh()->GetAnimInstance()->Montage_Stop(0.1f);
				}
			}

			if (!IsPlayGuardHitMontage())
			{
				ComponentOwnerCharacter->GetMesh()->GetAnimInstance()->Montage_Play(GuardHitMontage);
			}
		}
	}
}

bool UAnimationHelperComponent::IsPlayHitMontage()
{
	if (ComponentOwnerCharacter)
	{
		if (HitAnimationType != EHitAnimationType::AnimMontage)
		{
			return false;
		}

		if (bUseDirection)
		{
			for (const auto& iter : HitMontageByDirection)
			{
				if (ComponentOwnerCharacter->GetMesh()->GetAnimInstance()->Montage_IsPlaying(iter.Value))
				{
					return true;
				}
			}
		}
		else
		{
			return ComponentOwnerCharacter->GetMesh()->GetAnimInstance()->Montage_IsPlaying(HitMontage);
		}
	}
	return false;
}

bool UAnimationHelperComponent::IsPlayGuardHitMontage() const
{
	if (ComponentOwnerCharacter)
	{
		if (HitAnimationType != EHitAnimationType::AnimMontage)
		{
			return false;
		}

		return ComponentOwnerCharacter->GetMesh()->GetAnimInstance()->Montage_IsPlaying(GuardHitMontage);
	}
	return false;
}

bool UAnimationHelperComponent::IsHitMontage(const UAnimMontage* Montage)
{
	if (HitAnimationType != EHitAnimationType::AnimMontage)
	{
		return false;
	}

	if (bUseDirection)
	{
		for (const auto& iter : HitMontageByDirection)
		{
			if (Montage == iter.Value)
			{
				return true;
			}
		}
	}
	else
	{
		return Montage == HitMontage;
	}

	return false;
}

bool UAnimationHelperComponent::IsGuradHitMontage(const UAnimMontage* Montage)
{
	return Montage == GuardHitMontage;
}

void UAnimationHelperComponent::OnHitMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (ComponentOwnerCharacter)
	{
		if (!bInterrupted)
		{
			if (ComponentOwnerCharacter->IsDead() == false)
			{
				if (IsHitMontage(Montage) || IsGuradHitMontage(Montage))
				{
					ComponentOwnerCharacter->SetCharacterState(ECharacterState::NORMAL);
					if (const auto aiCon = ComponentOwnerCharacter->GetController<AMonsterAIController>())
					{
						UKismetSystemLibrary::PrintString(
							this, FString::Printf(TEXT("%s : 비헤이비어 트리 재 실행"), *GetNameSafe(this)));
					}
				}
			}
		}
	}
}

void UAnimationHelperComponent::OnHitMontageBlendOut(UAnimMontage* Montage, bool bInterrupted)
{
	if (ComponentOwnerCharacter)
	{
		if (!bInterrupted)
		{
			if (ComponentOwnerCharacter->IsDead() == false)
			{
				if (IsHitMontage(Montage) || IsGuradHitMontage(Montage))
				{
					if (bUseHitMighty)
					{
						UE_LOGFMT(LogTemp, Log, "슈퍼아머 부여");
						ActivateHitMightyAbility(HitMightyTime);
					}
				}
			}
		}
	}
}

void UAnimationHelperComponent::StartDeadDissolve()
{
	//디졸브 타임라인
	if (DissolveCurve && ComponentOwnerCharacter)
	{
		if (DissolveParticle)
		{
			UE_LOGFMT(LogCharacter, Log, "사망 파티클 생성");
			const auto comp = UNiagaraFunctionLibrary::SpawnSystemAttached(
				DissolveParticle, ComponentOwnerCharacter->GetRootComponent(), NAME_None, FVector::ZeroVector,
				FRotator::ZeroRotator,
				EAttachLocation::SnapToTargetIncludingScale, false);
			comp->SetColorParameter("Color", DissolveColor);
		}

		auto dissolveComp = ComponentOwnerCharacter->GetDeadDissolveTimeLineComponent();
		dissolveComp->AddInterpFloat(DissolveCurve, UpdateDissolve);
		dissolveComp->SetTimelineFinishedFunc(OnFinishDissolve);
		dissolveComp->PlayFromStart();
	}
}

void UAnimationHelperComponent::PlayDeadAnimationByMode()
{
	switch (DeadAnimationPlayMode)
	{
	case EDeadAnimationPlayMode::Sequence:
		PlayDeadAnimationSequence();
		break;
	case EDeadAnimationPlayMode::Montage:
		GetRandomDeadAnimationMontage();
		break;
	case EDeadAnimationPlayMode::None:
		break;
	default: ;
	}
}

void UAnimationHelperComponent::PlayDeadAnimationSequence()
{
	if (ComponentOwnerCharacter)
	{
		ComponentOwnerCharacter->GetMesh()->SetAnimationMode(EAnimationMode::AnimationSingleNode);
		auto randIndex = FMath::RandRange(0, DeadAnimations.Num() - 1);
		if (DeadAnimations.IsValidIndex(randIndex))
		{
			ComponentOwnerCharacter->GetMesh()->PlayAnimation(DeadAnimations[randIndex], false);
		}
	}
}

UAnimMontage* UAnimationHelperComponent::GetRandomDeadAnimationMontage()
{
	auto randIndex = FMath::RandRange(0, DeadMontages.Num() - 1);
	if (DeadMontages.IsValidIndex(randIndex))
	{
		return DeadMontages[randIndex];
	}

	return nullptr;
}

bool UAnimationHelperComponent::CanApplyRagdoll()
{
	return DeadAnimationPlayMode == EDeadAnimationPlayMode::None;
}
