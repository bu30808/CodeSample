// Fill out your copyright notice in the Description page of Project Settings.


#include "02_Ability/AbilityBase.h"

#include "00_Character/BaseCharacter.h"
#include "00_Character/00_Player/00_Controller/UserController.h"
#include "00_Character/00_Player/00_Controller/00_Component/InputHandlerComponent.h"
#include "00_Character/01_Component/AbilityComponent.h"
#include "02_Ability/AbilityEffect.h"
#include "02_Ability/AbilityRequirement.h"
#include "Logging/StructuredLog.h"
#include "00_Character/00_Player/PlayerCharacter.h"
#include "00_Character/00_Player/01_Component/AbilityTalentComponent.h"
#include "00_Character/01_Component/AnimationHelperComponent.h"
#include "00_Character/01_Component/AttributeComponent.h"
#include "99_Subsystem/AttackChainSubsystem.h"
#include "Kismet/GameplayStatics.h"

class UAttackChainSubsystem;
DEFINE_LOG_CATEGORY(LogAbility)


void UAbilityDamageTalent::SelfBind()
{
	if (!IsRooted())
	{
		AddToRoot();
	}

	if (OnChangeHealAmount.IsBound() == false)
	{
		OnChangeHealAmount.BindDynamic(this, &UAbilityDamageTalent::OnChangeHealAmountEvent);
	}

	if (OnIncreasePhysicalDamage.IsBound() == false)
	{
		OnIncreasePhysicalDamage.BindDynamic(this, &UAbilityDamageTalent::OnIncreasePhysicalDamageEvent);
	}

	if (OnIncreaseMagicalDamage.IsBound() == false)
	{
		OnIncreaseMagicalDamage.BindDynamic(this, &UAbilityDamageTalent::OnIncreaseMagicalDamageEvent);
	}

	if (OnIncreaseResultDamage.IsBound() == false)
	{
		OnIncreaseResultDamage.BindDynamic(this, &UAbilityDamageTalent::OnIncreaseResultDamageEvent);
	}

	if (OnSuccessDamageHit.IsBound() == false)
	{
		OnSuccessDamageHit.BindDynamic(this, &UAbilityDamageTalent::OnSuccessDamageHitEvent);
	}

	if (OnSuccessDamageHitIncreasePhysicalDam.IsBound() == false)
	{
		OnSuccessDamageHitIncreasePhysicalDam.BindDynamic(
			this, &UAbilityDamageTalent::OnSuccessDamageHitIncreasePhysicalDamEvent);
	}

	if (OnSuccessDamageHitIncreaseMagicalDam.IsBound() == false)
	{
		OnSuccessDamageHitIncreaseMagicalDam.BindDynamic(
			this, &UAbilityDamageTalent::OnSuccessDamageHitIncreaseMagicalDamEvent);
	}

	if (OnGotHit.IsBound() == false)
	{
		OnGotHit.BindDynamic(this, &UAbilityDamageTalent::OnGotHitEvent);
	}
}

void UAbilityDamageTalent::Bind(UAbilityTalentComponent* ATComp)
{
	if (ATComp)
	{
		SelfBind();
		UE_LOGFMT(LogTemp, Warning, "피해 특성 바인드 : {0}", GetName());
		ATComp->OnIncreasePhysicalDamage.Emplace(this, OnIncreasePhysicalDamage);
		ATComp->OnIncreaseMagicalDamage.Emplace(this, OnIncreaseMagicalDamage);

		ATComp->OnIncreaseResultDamage.Emplace(this, OnIncreaseResultDamage);


		ATComp->OnSuccessDamageHit.Emplace(this, OnSuccessDamageHit);
		ATComp->OnSuccessDamageHitIncreasePhysicalDam.Emplace(this, OnSuccessDamageHitIncreasePhysicalDam);
		ATComp->OnSuccessDamageHitIncreaseMagicalDam.Emplace(this, OnSuccessDamageHitIncreaseMagicalDam);

		ATComp->OnGotHit.Emplace(this, OnGotHit);

		ATComp->OnChangeHealAmount.Emplace(this, OnChangeHealAmount);
	}
}

void UAbilityDamageTalent::UnBind(UAbilityTalentComponent* ATComp)
{
	if (ATComp)
	{
		UE_LOGFMT(LogTemp, Warning, "특성 바인드 해제");
		ATComp->OnIncreasePhysicalDamage[this].Unbind();
		ATComp->OnIncreaseMagicalDamage[this].Unbind();
		ATComp->OnIncreaseMagicalDamage[this].Unbind();

		ATComp->OnSuccessDamageHit[this].Unbind();
		ATComp->OnSuccessDamageHitIncreasePhysicalDam[this].Unbind();
		ATComp->OnSuccessDamageHitIncreaseMagicalDam[this].Unbind();

		ATComp->OnGotHit[this].Unbind();
		ATComp->OnChangeHealAmount[this].Unbind();
	}

	if (IsRooted())
	{
		RemoveFromRoot();
	}
}


void UAbilityDefenceTalent::SelfBind()
{
	if (!IsRooted())
	{
		AddToRoot();
	}

	if (OnChangeHealAmount.IsBound() == false)
	{
		OnChangeHealAmount.BindDynamic(this, &UAbilityDefenceTalent::OnChangeHealAmountEvent);
	}

	if (OnKillMonster.IsBound() == false)
	{
		OnKillMonster.BindDynamic(this, &UAbilityTalent::OnKillMonsterEvent);
	}

	if(OnDecreaseGotHitDamage.IsBound() == false)
	{
		OnDecreaseGotHitDamage.BindDynamic(this,&UAbilityDefenceTalent::OnDecreaseGotHitDamageEvent);
	}

	if (OnDecreasePhysicalGotHitDamage.IsBound() == false)
	{
		OnDecreasePhysicalGotHitDamage.BindDynamic(
			this, &UAbilityDefenceTalent::OnDecreasePhysicalGotHitDamageEvent);
	}

	if (OnDecreaseMagicalGotHitDamage.IsBound() == false)
	{
		OnDecreaseMagicalGotHitDamage.BindDynamic(
			this, &UAbilityDefenceTalent::OnDecreaseMagicalGotHitDamageEvent);
	}


	if (OnIncreaseGotHitDamage.IsBound() == false)
	{
		OnIncreaseGotHitDamage.BindDynamic(this, &UAbilityDefenceTalent::OnIncreaseGotHitDamageEvent);
	}

	if (OnIncreaseRunSpeed.IsBound() == false)
	{
		OnIncreaseRunSpeed.BindDynamic(this, &UAbilityDefenceTalent::OnIncreaseMoveSpeedEvent);
	}

	if (OnDecreaseDodgeSP.IsBound() == false)
	{
		OnDecreaseDodgeSP.BindDynamic(this, &UAbilityDefenceTalent::OnDecreaseDodgeSPEvent);
	}

	if (OnIncreaseDodgeDistance.IsBound() == false)
	{
		OnIncreaseDodgeDistance.BindDynamic(this, &UAbilityDefenceTalent::OnIncreaseDodgeDistanceEvent);
	}

	if (OnIncreaseRecoverSPAmount.IsBound() == false)
	{
		OnIncreaseRecoverSPAmount.BindDynamic(this, &UAbilityDefenceTalent::OnIncreaseRecoverSPAmountEvent);
	}

	if (OnIncreaseDodgeInvincibilityTime.IsBound() == false)
	{
		OnIncreaseDodgeInvincibilityTime.BindDynamic(
			this, &UAbilityDefenceTalent::OnIncreaseDodgeInvincibilityTimeEvent);
	}

	if (OnReplaceDamageToMP.IsBound() == false)
	{
		OnReplaceDamageToMP.BindDynamic(this, &UAbilityDefenceTalent::OnReplaceDamageToMPEvent);
	}

	if (OnSuccessDodge.IsBound() == false)
	{
		OnSuccessDodge.BindDynamic(this, &UAbilityDefenceTalent::OnSuccessDodgeEvent);
	}

	if (OnIncreaseDamWhenSuccDodge.IsBound() == false)
	{
		OnIncreaseDamWhenSuccDodge.BindDynamic(this, &UAbilityDefenceTalent::OnIncreaseDamWhenSuccDodgeEvent);
	}

	if (OnGotHit.IsBound() == false)
	{
		OnGotHit.BindDynamic(this, &UAbilityDefenceTalent::OnGotHitEvent);
	}

	if (OnChangePhysicalDefence.IsBound() == false)
	{
		OnChangePhysicalDefence.BindDynamic(this, &UAbilityDefenceTalent::OnChangePhysicalDefenceEvent);
	}

	if (OnChangeMagicalDefence.IsBound() == false)
	{
		OnChangeMagicalDefence.BindDynamic(this, &UAbilityDefenceTalent::OnChangeMagicalDefenceEvent);
	}

	if (OnChangeDefenceByMonsterType.IsBound() == false)
	{
		OnChangeDefenceByMonsterType.BindDynamic(this, &UAbilityDefenceTalent::OnChangeDefenceByMonsterTypeEvent);
	}
}

void UAbilityDefenceTalent::Bind(UAbilityTalentComponent* ATComp)
{
	if (ATComp)
	{
		UE_LOGFMT(LogTalent,Warning,"{0} 방어 특성 바인드",ATComp->GetOwner()->GetActorNameOrLabel());
		SelfBind();
		
		ATComp->OnDecreaseGotHitDamage.Emplace(this, OnDecreaseGotHitDamage);

		ATComp->OnDecreasePhysicalGotHitDamage.Emplace(this, OnDecreasePhysicalGotHitDamage);
		ATComp->OnDecreaseMagicalGotHitDamage.Emplace(this, OnDecreaseMagicalGotHitDamage);

		ATComp->OnIncreaseGotHitDamage.Emplace(this, OnIncreaseGotHitDamage);
		ATComp->OnIncreaseRunSpeed.Emplace(this, OnIncreaseRunSpeed);

		ATComp->OnDecreaseDodgeSP.Emplace(this, OnDecreaseDodgeSP);
		for (auto iter : ATComp->OnDecreaseDodgeSP)
		{
			UE_LOGFMT(LogTemp, Log, "회피SP 바인드된 오브젝트 : {0}", iter.Key->GetName());
		}


		ATComp->OnIncreaseDodgeDistance.Emplace(this, OnIncreaseDodgeDistance);
		for (auto iter : ATComp->OnIncreaseDodgeDistance)
		{
			UE_LOGFMT(LogTemp, Log, "회피거리 바인드된 오브젝트 : {0}", iter.Key->GetName());
		}

		ATComp->OnIncreaseRecoverSPAmount.Emplace(this, OnIncreaseRecoverSPAmount);

		ATComp->OnIncreaseDodgeInvincibilityTime.Emplace(this, OnIncreaseDodgeInvincibilityTime);

		ATComp->OnReplaceDamageToMP.Emplace(this, OnReplaceDamageToMP);

		ATComp->OnSuccessDodge.Emplace(this, OnSuccessDodge);

		ATComp->OnIncreaseDamWhenSuccDodge.Emplace(this, OnIncreaseDamWhenSuccDodge);

		ATComp->OnGotHit.Emplace(this, OnGotHit);

		ATComp->OnChangePhysicalDefence.Emplace(this, OnChangePhysicalDefence);
		ATComp->OnChangeMagicalDefence.Emplace(this, OnChangeMagicalDefence);

		ATComp->OnChangeHealAmount.Emplace(this, OnChangeHealAmount);
		ATComp->OnKillMonster.Emplace(this, OnKillMonster);

		ATComp->OnChangeDefenceByMonsterType.Emplace(this, OnChangeDefenceByMonsterType);
	}
}

void UAbilityDefenceTalent::UnBind(UAbilityTalentComponent* ATComp)
{
	if (ATComp)
	{
		UE_LOGFMT(LogTemp, Warning, "특성 바인드 해제");
		if (ATComp->OnDecreaseGotHitDamage.Contains(this))
		{
			ATComp->OnDecreaseGotHitDamage[this].Unbind();
		}
		
		if (ATComp->OnDecreasePhysicalGotHitDamage.Contains(this))
		{
			ATComp->OnDecreasePhysicalGotHitDamage[this].Unbind();
		}
		if (ATComp->OnDecreaseMagicalGotHitDamage.Contains(this))
		{
			ATComp->OnDecreaseMagicalGotHitDamage[this].Unbind();
		}

		if (ATComp->OnIncreaseGotHitDamage.Contains(this))
		{
			ATComp->OnIncreaseGotHitDamage[this].Unbind();
		}

		if (ATComp->OnIncreaseRunSpeed.Contains(this))
		{
			ATComp->OnIncreaseRunSpeed[this].Unbind();
		}
		if (ATComp->OnDecreaseDodgeSP.Contains(this))
		{
			ATComp->OnDecreaseDodgeSP[this].Unbind();
		}
		if (ATComp->OnIncreaseDodgeDistance.Contains(this))
		{
			ATComp->OnIncreaseDodgeDistance[this].Unbind();
		}
		if (ATComp->OnIncreaseRecoverSPAmount.Contains(this))
		{
			ATComp->OnIncreaseRecoverSPAmount[this].Unbind();
		}
		if (ATComp->OnIncreaseDodgeInvincibilityTime.Contains(this))
		{
			ATComp->OnIncreaseDodgeInvincibilityTime[this].Unbind();
		}
		if (ATComp->OnReplaceDamageToMP.Contains(this))
		{
			ATComp->OnReplaceDamageToMP[this].Unbind();
		}

		if (ATComp->OnSuccessDodge.Contains(this))
		{
			ATComp->OnSuccessDodge[this].Unbind();
		}

		if (ATComp->OnIncreaseDamWhenSuccDodge.Contains(this))
		{
			ATComp->OnIncreaseDamWhenSuccDodge[this].Unbind();
		}

		if (ATComp->OnGotHit.Contains(this))
		{
			ATComp->OnGotHit[this].Unbind();
		}

		if (ATComp->OnChangePhysicalDefence.Contains(this))
		{
			ATComp->OnChangePhysicalDefence[this].Unbind();
		}

		if (ATComp->OnChangeMagicalDefence.Contains(this))
		{
			ATComp->OnChangeMagicalDefence[this].Unbind();
		}

		if (ATComp->OnChangeHealAmount.Contains(this))
		{
			ATComp->OnChangeHealAmount[this].Unbind();
		}

		if (ATComp->OnKillMonster.Contains(this))
		{
			ATComp->OnKillMonster[this].Unbind();
		}

		if (ATComp->OnChangeDefenceByMonsterType.Contains(this))
		{
			ATComp->OnChangeDefenceByMonsterType[this].Unbind();
		}
	}

	if (IsRooted())
	{
		RemoveFromRoot();
	}
}


void UAbilityFreeTalent::SelfBind()
{
	if (!IsRooted())
	{
		AddToRoot();
	}

	if (OnIncreaseGetExp.IsBound() == false)
	{
		OnIncreaseGetExp.BindDynamic(this, &UAbilityFreeTalent::OnIncreaseGetExpEvent);
	}

	if (OnDecreaseRunSP.IsBound() == false)
	{
		OnDecreaseRunSP.BindDynamic(this, &UAbilityFreeTalent::OnDecreaseRunSPEvent);
	}

	if (OnDodgeMinimumSP.IsBound() == false)
	{
		OnDodgeMinimumSP.BindDynamic(this, &UAbilityFreeTalent::OnDodgeMinimumSPEvent);
	}
}

void UAbilityFreeTalent::Bind(UAbilityTalentComponent* ATComp)
{
	if (ATComp)
	{
		SelfBind();
		ATComp->OnIncreaseGetExp.Emplace(this, OnIncreaseGetExp);
		ATComp->OnDecreaseRunSP.Emplace(this, OnDecreaseRunSP);
		ATComp->OnDodgeMinimumSP = OnDodgeMinimumSP;
	}
}

void UAbilityFreeTalent::UnBind(UAbilityTalentComponent* ATComp)
{
	if (ATComp)
	{
		ATComp->OnIncreaseGetExp[this].Unbind();
		ATComp->OnDecreaseRunSP[this].Unbind();
		ATComp->OnDodgeMinimumSP.Unbind();
	}

	if (IsRooted())
	{
		RemoveFromRoot();
	}
}


UAnimMontage* UAbilityBase::GetNextMontage()
{
	if (Montages.Num() == 0)
	{
		return nullptr;
	}

	if (Montages.IsValidIndex(MontageIndex))
	{
		CurrentMontage = Montages[MontageIndex];

		if (bUseChainSystem)
		{
			//배율을 저장합니다.
			if (AbilityOwner->IsA<APlayerCharacter>() && ChainValue.IsValidIndex(MontageIndex))
			{
				if (auto subsystem = UGameplayStatics::GetGameInstance(AbilityOwner.Get())->GetSubsystem<
					UAttackChainSubsystem>())
				{
					UE_LOGFMT(LogTemp, Log, "배율 저장 : {0} , {1}", ChainTag.ToString(), ChainValue[MontageIndex]);
					subsystem->AddChainValue(ChainTag, ChainValue[MontageIndex], this);
				}
			}
		}

		MontageIndex++;

		return CurrentMontage;
	}

	MontageIndex = 0;

	return GetNextMontage();
}

UAnimMontage* UAbilityBase::GetRandomMontage()
{
	if (Montages.Num() > 0)
	{
		return Montages[FMath::RandRange(0, Montages.Num() - 1)];
	}

	return nullptr;
}

float UAbilityBase::PlayMontageWithCustomChain(ABaseCharacter* Target, UAnimMontage* Montage, float CustomChainValue)
{
	if (auto subsystem = UGameplayStatics::GetGameInstance(AbilityOwner.Get())->GetSubsystem<
		UAttackChainSubsystem>())
	{
		UE_LOGFMT(LogTemp, Log, "커스텀 배율 저장 : {0} , {1}", ChainTag.ToString(), CustomChainValue);
		subsystem->AddChainValue(ChainTag, CustomChainValue,this);
	}

	const auto& playRate = Target->GetAttributeComponent()->GetActionSpeed();
	return Target->GetMesh()->GetAnimInstance()->Montage_Play(Montage, playRate);
}

void UAbilityBase::SetAbilityInformation()
{
	if (!AbilityTag.IsValid())
	{
		AbilityInformation = FAbilityInformation();
		return;
	}

	if (AbilityInformationTable != nullptr)
	{
		FString contextString;
		if (const auto findInfo = AbilityInformationTable->FindRow<FAbilityInformation>(
			AbilityTag.GetTagName(), contextString))
		{
			AbilityInformation = *findInfo;
		}
	}
	else
	{
		AbilityInformation = FAbilityInformation();
	}
}

void UAbilityBase::PostInitProperties()
{
	UObject::PostInitProperties();
	SetAbilityInformation();
}
#if WITH_EDITOR
void UAbilityBase::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	UObject::PostEditChangeProperty(PropertyChangedEvent);
	SetAbilityInformation();
}
#endif

void UAbilityBase::InitAbility(TWeakObjectPtr<ABaseCharacter> Owner,
                               AActor* Target/*, class UInputAction* InputAction*/)
{
	AbilityOwner = Owner;
	AbilityTarget = Target;
	/*AbilityInputAction = InputAction;*/
}

void UAbilityBase::TryActivateAbility_Implementation(UObject* AdditionalInfo)
{
	bAlreadyEndAbility = false;

	OnCommitResultFailed.AddUniqueDynamic(this, &UAbilityBase::OnCommitResultFailedEvent);
	OnCommitFailedByCost.AddUniqueDynamic(this, &UAbilityBase::OnCommitFailedByCostEvent);
	OnCommitFailedByCooldown.AddUniqueDynamic(this, &UAbilityBase::OnCommitFailedByCooldownEvent);

	if (CommitAbility())
	{
		UE_LOGFMT(LogAbility, Warning, "어빌리티 사용 : {0}", AbilityTag.ToString());

		BindDeadEvent();
		ForceEndAbilities();
		HandleOnTriggerHitAnimationNotEntered();
		BindTalent();
		ApplyCost();
		ApplyCooldown();
		BindMontageEvent();
		BindActionEvent();
		RegisterAbilityTag();
		ApplyEffect();
		ApplyCues();
		SetAbilityOwnerState();
		ActivateAbility(AdditionalInfo);
	}
	else
	{
		OnCommitResultFailed.Broadcast();
		EndAbility();
	}
}

bool UAbilityBase::CommitAbility_Implementation()
{
	if (AbilityInformation.AbilityRequirement != nullptr)
	{
		return AbilityInformation.AbilityRequirement->GetDefaultObject<UAbilityRequirement>()->IsActivateAbility(
			AbilityTag, AbilityOwner.Get(), this);
	}

	UE_LOGFMT(LogTemp, Warning, "{0}가 사용할 어빌리티 요구사항 오브젝트가 없습니다. 무조건 Commit 성공합니다.", AbilityTag.ToString());
	return true;
}

void UAbilityBase::ActivateAbility_Implementation(UObject* AdditionalInfo)
{
}

void UAbilityBase::CancelAbility_Implementation()
{
	if (AbilityOwner.IsValid())
	{
		if (auto instance = AbilityOwner->GetMesh()->GetAnimInstance())
		{
			if (Montages.Contains(instance->GetCurrentActiveMontage()))
			{
				instance->Montage_Stop(0.25f, instance->GetCurrentActiveMontage());
			}
		}
	}

	EndAbility();
}

void UAbilityBase::EndAbility_Implementation()
{
	if (bAlreadyEndAbility)
	{
		UE_LOGFMT(LogAbility, Warning, "이미 종료된 어빌리티  : {0}", AbilityTag.ToString());
		return;
	}

	bAlreadyEndAbility = true;
	UE_LOGFMT(LogAbility, Warning, "어빌리티 종료 : {0}", AbilityTag.ToString());

	UnBindDeadEvent();
	MontageIndex = 0;
	UnBindMontageEvent();
	UnBindActionEvent();
	UnBindTalent();
	RemoveCues();
	EndEffect();
	//어빌리티 컴포넌트에서 이 어빌리티 태그를 제거합니다.
	AbilityOwner->GetAbilityComponent()->UnRegisterAbilityTag(AbilityTag);
	OnEndAbility.Broadcast();
}

UAbilityComponent* UAbilityBase::GetAbilityComponent(ABaseCharacter* Character)
{
	if (Character != nullptr)
	{
		return Character->GetAbilityComponent();
	}

	return nullptr;
}

void UAbilityBase::SetAbilityOwnerState_Implementation()
{
	if (AbilityOwner.IsValid())
	{
		if (AbilityOwner->GetCharacterState() == ECharacterState::HIT)
		{
			if (AbilityOwner->IsA<APlayerCharacter>())
			{
				AbilityOwner->GetController<AUserController>()->SetIgnoreMoveInput(false);
			}
			AbilityOwner->SetCharacterState(ECharacterState::NORMAL);
		}
	}
}

void UAbilityBase::ApplyEffect_Implementation()
{
	ensure(AbilityOwner.Get());
	ensure(AbilityTarget.Get());

	///Warning : 오리지널 이팩트 객체를 복사해서 쓰는 부분에서 GC 문제가 발생합니다.
	if (AbilityTarget.IsValid())
	{
		if (SelfEffectInstance.IsEmpty())
		{
			//적용할 이팩트를 복사해서 가지고 있습니다.
			for (auto e : SelfEffect)
			{
				if (auto copy = DuplicateObject(e.GetDefaultObject(), this))
				{
					SelfEffectInstance.Emplace(copy);
				}
				//SelfEffectInstance.Emplace(e.GetDefaultObject());
			}
		}

		OnOverrideSelfEffect.Broadcast(SelfEffectInstance, AbilityTarget.Get());
		ApplySelfEffect(AbilityOwner.Get());

		if (TargetEffectInstance.IsEmpty())
		{
			for (auto e : TargetEffect)
			{
				if (auto copy = DuplicateObject(e.GetDefaultObject(), this))
				{
					TargetEffectInstance.Emplace(copy);
				}
				//TargetEffectInstance.Emplace(e.GetDefaultObject());
			}
		}
		OnOverrideTargetEffect.Broadcast(TargetEffectInstance, AbilityTarget.Get());

		if (AbilityTarget->IsA<ABaseCharacter>())
		{
			ApplyTargetEffect(Cast<ABaseCharacter>(AbilityTarget.Get()));
		}
	}
}

void UAbilityBase::ApplySelfEffect(ABaseCharacter* Character)
{
	ensure(Character);

	FOnEffectExpired OnExpiredSelfEffect;
	OnExpiredSelfEffect.BindDynamic(this, &UAbilityBase::OnExpiredSelfEffectEvent);

	Character->GetAbilityComponent()->ApplyEffects(SelfEffectInstance, Character, OnExpiredSelfEffect, this, nullptr);
}

void UAbilityBase::ApplyTargetEffect(ABaseCharacter* Character)
{
	ensure(Character);

	FOnEffectExpired OnExpiredTargetEffect;
	OnExpiredTargetEffect.BindDynamic(this, &UAbilityBase::OnExpiredTargetEffect);

	Character->GetAbilityComponent()->ApplyEffects(TargetEffectInstance, Character, OnExpiredTargetEffect, this,
	                                               nullptr);
}

void UAbilityBase::EndSelfEffect(ABaseCharacter* Character) const
{
	UE_LOGFMT(LogEffect, Log, "셀프 이팩트 종료!!");
	Character->GetAbilityComponent()->EndEffects(SelfEffectInstance);
}

void UAbilityBase::EndTargetEffect(ABaseCharacter* Character) const
{
	Character->GetAbilityComponent()->EndEffects(TargetEffectInstance);
}

void UAbilityBase::EndEffect_Implementation()
{
	if (bEndAppliedEffectManually)
	{
		return;
	}

	UE_LOGFMT(LogAbility, Warning, "{0} {1} {2}에 적용된 이팩트 종료", __FUNCTION__, __LINE__, AbilityTag.ToString());
	if (AbilityTarget.IsValid())
	{
		EndSelfEffect(AbilityOwner.Get());

		if (AbilityTarget->IsA<ABaseCharacter>())
		{
			EndTargetEffect(Cast<ABaseCharacter>(AbilityTarget.Get()));
		}
	}
	else
	{
		UE_LOGFMT(LogAbility, Error, "{0} {1}", __FUNCTION__, __LINE__);
	}
}

void UAbilityBase::RegisterAbilityTag() const
{
	UE_LOGFMT(LogAbility, Warning, "{0} {1} : 어빌리티 태그 등록 : {2}", __FUNCTION__, __LINE__, AbilityTag.ToString());
	AbilityOwner->GetAbilityComponent()->RegisterAbilityTag(AbilityTag);
}

void UAbilityBase::ApplyCost()
{
	if (AbilityInformation.AbilityRequirement != nullptr)
	{
		UE_LOGFMT(LogAbility, Warning, "코스트 적용 : {0}", AbilityTag.ToString());
		if (auto costObject = AbilityInformation.AbilityRequirement->GetDefaultObject<UAbilityRequirement>()->GetCost())
		{
			if (auto costEffect = DuplicateObject<UAbilityEffect>(costObject.GetDefaultObject(), AbilityOwner.Get()))
			{
				AbilityOwner->GetAbilityComponent()->ApplyEffect(costEffect, AbilityOwner.Get(), FOnEffectExpired(),
				                                                 this, nullptr);
			}
			else
			{
				UE_LOGFMT(LogAbility, Error, "{0} {1}", __FUNCTION__, __LINE__);
			}
		}
	}
}

void UAbilityBase::ApplyCooldown()
{
	if (AbilityInformation.AbilityRequirement != nullptr)
	{
		UE_LOGFMT(LogAbility, Warning, "쿨다운 적용 : {0}", AbilityTag.ToString());
		if (auto cooldownObject = AbilityInformation.AbilityRequirement->GetDefaultObject<UAbilityRequirement>()->
		                                             GetCooldown())
		{
			//AbilityOwner->GetAbilityComponent()->ProcessEffect(cooldownObject.GetDefaultObject(),FOnEffectExpired());
			if (auto cooldownEffect = DuplicateObject<UAbilityEffect>(cooldownObject.GetDefaultObject(),
			                                                          AbilityOwner.Get()))
			{
				AbilityOwner->GetAbilityComponent()->
				              ApplyEffect(cooldownEffect, AbilityOwner.Get(), FOnEffectExpired(), this, nullptr);
			}
			else
			{
				UE_LOGFMT(LogAbility, Error, "{0} {1}", __FUNCTION__, __LINE__);
			}
		}
	}
}

void UAbilityBase::ApplyCues()
{
	if (AbilityOwner.IsValid())
	{
		if (auto abComp = AbilityOwner->GetAbilityComponent())
		{
			AppliedSelfCue = abComp->ApplyCues(SelfCue);
		}
	}

	if (AbilityTarget.IsValid() && AbilityTarget->IsA<ABaseCharacter>())
	{
		if (auto abComp = Cast<ABaseCharacter>(AbilityTarget)->GetAbilityComponent())
		{
			AppliedTargetCue = abComp->ApplyCues(TargetCue);
		}
	}
}

void UAbilityBase::RemoveCues()
{
	for (auto c : AppliedSelfCue)
	{
		c->Destroy();
	}
	AppliedSelfCue.Empty();

	for (auto c : AppliedTargetCue)
	{
		c->Destroy();
	}
	AppliedTargetCue.Empty();
}

void UAbilityBase::OnNotKeyPressed_Implementation()
{
	//UKismetSystemLibrary::PrintString(this,TEXT("키 눌리지 않음! : ") + AbilityTag.ToString());
	EndAbility();
}

void UAbilityBase::BindActionEvent()
{
	if (AbilityOwner.IsValid())
	{
		if (auto pc = AbilityOwner->GetController<AUserController>())
		{

			//이 어빌리티 중간에 다른 키 입력으로 액션을 연계하기 위해 바인드 합니다.
			for (auto action : WaitInputAction)
			{
				if (AbilityTag.MatchesTagExact(
					FGameplayTag::RequestGameplayTag("Character.WhiteTiger.Active.Attack.Normal")))
				{
					UKismetSystemLibrary::PrintString(AbilityOwner.Get(),TEXT("키 이벤트 바인드 됨 : ") + action->GetName());
				}
				UE_LOGFMT(LogInput, Warning, "키 이벤트 바인드 : {0}, {1}", AbilityTag.ToString(), action->GetName());
				//UE_LOGFMT(LogAbility, Error, "{0} {1}", __FUNCTION__, __LINE__);
				FOnKeyPressedWithAction OnKeyPressed;
				OnKeyPressed.AddUniqueDynamic(this, &UAbilityBase::OnPressedActionKey);
				
				FOnKeyReleasedWithAction OnKeyReleased;
				OnKeyReleased.AddUniqueDynamic(this, &UAbilityBase::OnReleasedActionKey);

				//키가 눌리지 않았을 때, 할 행동
				FNoKeyInput OnNotKeyPressed;
				//OnNotKeyPressed.AddUniqueDynamic(this, &UAbilityBase::EndAbility);
				OnNotKeyPressed.AddUniqueDynamic(this, &UAbilityBase::OnNotKeyPressed);
				pc->GetInputHandlerComponent()->BindEvent(FKeyPressedInfo(action, AbilityTag), OnKeyPressed,OnKeyReleased,
				                                          OnNotKeyPressed);
			}
		}
	}
}

void UAbilityBase::UnBindActionEvent() const
{
	if (AbilityOwner.IsValid())
	{
		if (auto pc = AbilityOwner->GetController<AUserController>())
		{
			UE_LOGFMT(LogInput, Warning, "키 대기 이벤트 언바인드 : {0}", AbilityTag.ToString());
			/*//이 어빌리티를 발동한 액션을 키로 해서 바인드 한 이벤트를 제거합니다.
			if(AbilityInputAction.IsValid()){
				pc->GetInputHandlerComponent()->UnBindEvent(FKeyPressedInfo(AbilityInputAction.Get(),AbilityTag));
			}*/

			//이 어빌리티 중간에 다른 키 입력으로 액션을 연계하기 위해 바인드한 이벤트를 제거합니다.
			for (auto action : WaitInputAction)
			{
				pc->GetInputHandlerComponent()->UnBindEvent(FKeyPressedInfo(action, AbilityTag));
			}
		}
	}
}

void UAbilityBase::BindTalent()
{
	if (AbilityTarget.IsValid() && AbilityTarget->IsA<ABaseCharacter>())
	{
		if (const auto atComp = Cast<ABaseCharacter>(AbilityTarget)->GetAbilityTalentComponent())
		{
			for (auto t : AbilityTalent)
			{
				if (auto copy = DuplicateObject(t.GetDefaultObject(), this))
				{
					AbilityTalentInstance.Emplace(copy);
					copy->Bind(atComp);
				}
			}
		}
	}
}

void UAbilityBase::UnBindTalent()
{
	if (AbilityTarget.IsValid() && AbilityTarget->IsA<ABaseCharacter>())
	{
		if (const auto atComp = Cast<ABaseCharacter>(AbilityTarget)->GetAbilityTalentComponent())
		{
			for (auto t : AbilityTalentInstance)
			{
				t->UnBind(atComp);
			}

			AbilityTalentInstance.Empty();
		}
	}
}

void UAbilityBase::BindDeadEvent()
{
	if (!bShouldBindDeadEvent)
	{
		return;
	}
	if (AbilityOwner.IsValid())
	{
		AbilityOwner->OnDead.AddUniqueDynamic(this, &UAbilityBase::OnAbilityOwnerDeadEvent);
	}
}

void UAbilityBase::UnBindDeadEvent()
{
	if (!bShouldBindDeadEvent)
	{
		return;
	}
	if (AbilityOwner.IsValid())
	{
		AbilityOwner->OnDead.RemoveDynamic(this, &UAbilityBase::OnAbilityOwnerDeadEvent);
	}
}

void UAbilityBase::OnAbilityOwnerDeadEvent_Implementation(AActor* Who, AActor* DeadBy, EDeadReason DeadReason)
{
	CancelAbility();
}

void UAbilityBase::BindMontageEvent()
{
	if (AbilityTarget.IsValid())
	{
		if (AbilityTarget->IsA<ABaseCharacter>())
		{
			if (auto character = Cast<ABaseCharacter>(AbilityTarget.Get()))
			{
				UE_LOGFMT(LogAbility, Warning, "몽타주 이벤트 바인드 : {0}", AbilityTag.ToString());

				character->GetMesh()->GetAnimInstance()->OnMontageBlendingOut.AddUniqueDynamic(
					this, &UAbilityBase::OnMontageBlendingOutEvent);
				character->GetMesh()->GetAnimInstance()->OnMontageEnded.AddUniqueDynamic(
					this, &UAbilityBase::OnMontageEndedEvent);
			}
		}
	}
}

void UAbilityBase::UnBindMontageEvent()
{
	if (AbilityTarget.IsValid())
	{
		if (AbilityTarget->IsA<ABaseCharacter>())
		{
			if (auto character = Cast<ABaseCharacter>(AbilityTarget.Get()))
			{
				character->GetMesh()->GetAnimInstance()->OnMontageBlendingOut.RemoveDynamic(
					this, &UAbilityBase::OnMontageBlendingOutEvent);
				character->GetMesh()->GetAnimInstance()->OnMontageEnded.RemoveDynamic(
					this, &UAbilityBase::OnMontageEndedEvent);
			}
		}
	}
}

bool UAbilityBase::IsCostEnough()
{
	if (AbilityInformation.AbilityRequirement->IsValidLowLevel())
	{
		if(!AbilityInformation.AbilityRequirement->GetDefaultObject<UAbilityRequirement>()->IsCostEnough(
			AbilityOwner.Get()))
		{
			OnCommitFailedByCost.Broadcast();
			return false;
		}
	}

	return true;
}

AActor* UAbilityBase::SpawnActor(TSubclassOf<AActor> ActorObject)
{
	if (ActorObject && AbilityOwner.IsValid())
	{
		FActorSpawnParameters spawnParam;
		spawnParam.Instigator = AbilityOwner.Get();
		spawnParam.Owner = AbilityOwner.Get();
		spawnParam.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		return AbilityOwner->GetWorld()->SpawnActor(ActorObject, nullptr, nullptr, spawnParam);
	}

	return nullptr;
}

void UAbilityBase::HandleOnTriggerHitAnimationNotEntered()
{
	if (AbilityOwner.IsValid())
	{
		if (AbilityOwner->GetAnimationHelperComponent()->GetIsTriggeredHitAnimationExitEvent() == false)
		{
			UE_LOGFMT(LogAbility, Error, "{0} {1} : 히트 애니메이션 빠져나감 이벤트가 호출되지 않았기 때문에 강제로 호출합니다.", __FUNCTION__,
			          __LINE__);
			AbilityOwner->GetAnimationHelperComponent()->OnTriggerHitAnimationExit.Broadcast(nullptr, nullptr);
		}
	}
}

float UAbilityBase::GetOwnersActionSpeed()
{
	if (AbilityOwner.IsValid())
	{
		return AbilityOwner->GetAttributeComponent()->GetActionSpeed();
	}
	return 0;
}

UWorld* UAbilityBase::GetWorld() const
{
	if (AbilityOwner.IsValid())
	{
		return AbilityOwner->GetWorld();
	}

	if (AbilityTarget.IsValid())
	{
		return AbilityTarget->GetWorld();
	}

	return nullptr;
}

void UAbilityBase::ForceEndEffects_Implementation()
{
	if (AbilityOwner.IsValid())
	{
		for (const auto& tag : ForceEndEffectTags)
		{
			GetAbilityComponent(AbilityOwner.Get())->EndEffectByTag(tag);
		}
	}
}

void UAbilityBase::ForceEndAbilities_Implementation()
{
	if (AbilityOwner.IsValid())
	{
		for (const auto& tag : ForceEndAbilityTags)
		{
			GetAbilityComponent(AbilityOwner.Get())->ForceEndAbility(tag);
		}
	}
}
