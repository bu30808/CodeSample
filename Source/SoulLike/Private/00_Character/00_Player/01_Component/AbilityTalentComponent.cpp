// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/00_Player/01_Component/AbilityTalentComponent.h"

#include "00_Character/00_Player/PlayerCharacter.h"
#include "02_Ability/AbilityEffect.h"
#include "00_Character/BaseCharacter.h"
#include "00_Character/01_Component/AttributeComponent.h"
#include "00_Character/03_Monster/BaseMonster.h"
#include "02_Ability/AbilityBase.h"
#include "Logging/StructuredLog.h"

// Sets default values for this component's properties
UAbilityTalentComponent::UAbilityTalentComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UAbilityTalentComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
}

float UAbilityTalentComponent::PhysicalDamageTraits(class ABaseCharacter* DamagedCharacter, ABaseCharacter* DamagedBy,
                                                    const float& Damage)
{
	float sum = 0;
	for (auto iter : OnIncreasePhysicalDamage)
	{
		if (iter.Value.IsBound())
		{
			sum += iter.Value.Execute(Damage, DamagedBy, DamagedCharacter);
		}
	}

	/*
	//일반 몬스터 물리 피해량 증가 적용
	for (auto iter : OnIncreaseCommonMonsterPhysicalDamage)
	{
		if (iter.Value.IsBound())
		{
			sum += iter.Value.Execute(Damage, DamagedBy, DamagedCharacter);
		}
	}

	//타입별 몬스터 물리 피해량 증가 적용
	for (auto iter : OnIncreaseMonsterPhysicalDamageByType)
	{
		if (iter.Value.IsBound())
		{
			sum += iter.Value.Execute(Damage, DamagedBy, DamagedCharacter);
		}
	}

	//보스 몬스터 물리 피해량 증가 적용
	for (auto iter : OnIncreaseBossMonsterPhysicalDamage)
	{
		if (iter.Value.IsBound())
		{
			sum += iter.Value.Execute(Damage, DamagedBy, DamagedCharacter);
		}
	}
	*/

	//연속공격 물리 피해량 증가 적용
	for (auto iter : OnSuccessDamageHitIncreasePhysicalDam)
	{
		if (iter.Value.IsBound())
		{
			sum += iter.Value.Execute(Damage, DamagedBy, DamagedCharacter);
		}
	}


	return sum;
}

float UAbilityTalentComponent::MagicalDamageTraits(ABaseCharacter* DamagedCharacter, ABaseCharacter* DamagedBy,
                                                   const float& Damage)
{
	float sum = 0;
	for (auto iter : OnIncreaseMagicalDamage)
	{
		if (iter.Value.IsBound())
		{
			sum += iter.Value.Execute(Damage, DamagedBy, DamagedCharacter);
		}
	}

	/*
	//일반 몬스터 마법 피해량 증가 적용
	for (auto iter : OnIncreaseCommonMonsterMagicalDamage)
	{
		if (iter.Value.IsBound())
		{
			sum += iter.Value.Execute(Damage, DamagedBy, DamagedCharacter);
		}
	}

	//타입별 몬스터 마법 피해량 증가 적용
	for (auto iter : OnIncreaseBossMonsterMagicalDamage)
	{
		if (iter.Value.IsBound())
		{
			sum += iter.Value.Execute(Damage, DamagedBy, DamagedCharacter);
		}
	}

	//보스 몬스터 마법 피해량 증가 적용
	for (auto iter : OnIncreaseMonsterMagicalDamageByType)
	{
		if (iter.Value.IsBound())
		{
			sum += iter.Value.Execute(Damage, DamagedBy, DamagedCharacter);
		}
	}
	*/

	//연속공격 마법 피해량 증가 적용
	for (auto iter : OnSuccessDamageHitIncreaseMagicalDam)
	{
		if (iter.Value.IsBound())
		{
			sum += iter.Value.Execute(Damage, DamagedBy, DamagedCharacter);
		}
	}

	return sum;
}

float UAbilityTalentComponent::PhysicalDecreaseDamageTraits(ABaseCharacter* DamagedCharacter, ABaseCharacter* DamagedBy,
                                                            const float& Damage)
{
	float sum = 0;


	for (auto iter : OnDecreasePhysicalGotHitDamage)
	{
		if (iter.Value.IsBound())
		{
			sum += iter.Value.Execute(Damage, DamagedBy, DamagedCharacter);
		}
	}


	return FMath::Clamp(sum, 0, Damage);
}

float UAbilityTalentComponent::MagicalDecreaseDamageTraits(ABaseCharacter* DamagedCharacter, ABaseCharacter* DamagedBy,
                                                           const float& Damage)
{
	float sum = 0;
	//특성을 증감할 피해량을 계산합니다.
	for (auto iter : OnDecreaseMagicalGotHitDamage)
	{
		if (iter.Value.IsBound())
		{
			sum += iter.Value.Execute(Damage, DamagedBy, DamagedCharacter);
		}
	}


	return FMath::Clamp(sum, 0, Damage);
}

float UAbilityTalentComponent::CalculateModifiedDamageWithTraits(class ABaseCharacter* DamagedCharacter,
                                                                 ABaseCharacter* DamagedBy,
                                                                 EAttackType AttackType, const float& Damage)
{
	if (DamagedBy && DamagedBy->IsA<ABaseCharacter>())
	{
		float sum = 0;
		switch (AttackType)
		{
		case EAttackType::Physical:
			sum += PhysicalDamageTraits(DamagedCharacter, DamagedBy, Damage);
			break;
		case EAttackType::Magical:
			sum += MagicalDamageTraits(DamagedCharacter, DamagedBy, Damage);
			break;
		default: ;
		}

		//확률에 따라 증가하는 피해 적용
		for (auto iter : OnIncreaseDamageByChance)
		{
			if (iter.Value.IsBound())
			{
				sum += iter.Value.Execute(Damage, DamagedBy, DamagedCharacter);
			}
		}

		//최종 피해량 증가 특성 적용
		for (auto iter : OnIncreaseResultDamage)
		{
			if (iter.Value.IsBound())
			{
				sum += iter.Value.Execute(Damage, DamagedBy, DamagedCharacter);
			}
		}

		return sum;
	}

	return 0;
}

float UAbilityTalentComponent::CalculateModifiedResultDamageWithTraits(ABaseCharacter* DamagedCharacter,
                                                                       ABaseCharacter* DamagedBy,
                                                                       EAttackType AttackType,
                                                                       const float& ResultDamage)
{
	if (DamagedBy && DamagedBy->IsA<ABaseCharacter>())
	{
		float sum = 0;
		for (auto iter : OnIncreaseResultDamage)
		{
			if (iter.Value.IsBound())
			{
				sum += iter.Value.Execute(ResultDamage, DamagedBy, DamagedCharacter);
			}
		}

		return sum;
	}

	return 0;
}

float UAbilityTalentComponent::CommonDecreaseDamageTraits(ABaseCharacter* DamagedCharacter, ABaseCharacter* DamagedBy,
	float Damage)
{
	float sum = 0;
	for (auto iter : OnDecreaseGotHitDamage)
	{
		if (iter.Value.IsBound())
		{
			sum += iter.Value.Execute(Damage, DamagedBy, DamagedCharacter);
		}
	}
	return FMath::Clamp(sum, 0, Damage);
}

float UAbilityTalentComponent::CalculateModifiedDecreaseDamageWithTraits(ABaseCharacter* DamagedCharacter,
                                                                         ABaseCharacter* DamagedBy,
                                                                         EAttackType AttackType, float Damage)
{
	if (DamagedCharacter && DamagedCharacter->IsA<ABaseCharacter>())
	{
		float sum = 0;

		sum+=CommonDecreaseDamageTraits(DamagedCharacter, DamagedBy, Damage);
		
		switch (AttackType)
		{
		case EAttackType::Physical:
			sum += PhysicalDecreaseDamageTraits(DamagedCharacter, DamagedBy, Damage);
			break;
		case EAttackType::Magical:
			sum += MagicalDecreaseDamageTraits(DamagedCharacter, DamagedBy, Damage);
			break;
		}

		return sum;
	}

	return 0;
}

float UAbilityTalentComponent::CalculateModifiedIncreaseGotHitDamageWithTraits(ABaseCharacter* DamagedCharacter,
                                                                               ABaseCharacter* DamagedBy,
                                                                               EAttackType AttackType, float Damage)
{
	if (DamagedCharacter && DamagedCharacter->IsA<ABaseCharacter>())
	{
		float sum = 0;

		for (auto iter : OnIncreaseGotHitDamage)
		{
			if (iter.Value.IsBound())
			{
				sum += iter.Value.Execute(Damage, DamagedBy, DamagedCharacter);
			}
		}

		return sum;
	}

	return 0;
}

float UAbilityTalentComponent::CalculateModifiedRunSpeedWithTraits(float OriginalSpeed)
{
	auto speed = OriginalSpeed;
	for (auto iter : OnIncreaseRunSpeed)
	{
		if (iter.Value.IsBound())
		{
			speed += iter.Value.Execute(OriginalSpeed);
		}
	}

	return speed;
}

float UAbilityTalentComponent::CalculateModifiedWalkSpeedWithTraits(float OriginalSpeed)
{
	auto speed = OriginalSpeed;
	for (auto iter : OnIncreaseMoveSpeed)
	{
		if (iter.Value.IsBound())
		{
			speed += iter.Value.Execute(OriginalSpeed);
		}
	}

	for (auto iter : OnDecreaseMoveSpeed)
	{
		if (iter.Value.IsBound())
		{
			speed -= iter.Value.Execute(OriginalSpeed);
		}
	}

	return speed;
}

float UAbilityTalentComponent::CalculateModifiedDodgeSPWithTraits(float OriginalStamina)
{
	float sum = 0;
	for (auto iter : OnDecreaseDodgeSP)
	{
		if (iter.Value.IsBound())
		{
			sum += FMath::Clamp(iter.Value.Execute(OriginalStamina), 0, OriginalStamina);
		}
	}

	return sum;
}

float UAbilityTalentComponent::CalculateModifiedDodgeDistanceWithTraits(float OriginalForce)
{
	float sum = 0;
	for (auto iter : OnIncreaseDodgeDistance)
	{
		if (iter.Value.IsBound())
		{
			sum += iter.Value.Execute(OriginalForce);
		}
	}

	return sum;
}

float UAbilityTalentComponent::CalculateModifiedRecoverSpAmountWithTraits(float OriginalAmount)
{
	auto sum = 0.f;

	for (auto iter : OnIncreaseRecoverSPAmount)
	{
		if (iter.Value.IsBound())
		{
			sum += iter.Value.Execute(OriginalAmount);
		}
	}

	return sum;
}

float UAbilityTalentComponent::CalculateModifiedIncreaseDodgeInvincibilityTimeWithTraits(
	ABaseCharacter* Target, float OriginalTime)
{
	auto sum = 0.f;

	for (auto iter : OnIncreaseDodgeInvincibilityTime)
	{
		if (iter.Value.IsBound())
		{
			sum += iter.Value.Execute(OriginalTime);
		}
	}

	return sum;
}

float UAbilityTalentComponent::CalculateModifiedIncreaseGetExpWithTraits(float OriginalExp)
{
	auto sum = 0.f;

	for (auto iter : OnIncreaseGetExp)
	{
		if (iter.Value.IsBound())
		{
			sum += iter.Value.Execute(OriginalExp);
		}
	}

	return sum;
}

float UAbilityTalentComponent::CalculateModifiedDamageToMPWithTraits(ABaseCharacter* DamagedCharacter,
                                                                     ABaseCharacter* DamagedBy, float OriginalDamage)
{
	auto sum = 0.f;

	for (auto iter : OnReplaceDamageToMP)
	{
		if (iter.Value.IsBound())
		{
			sum += iter.Value.Execute(OriginalDamage, DamagedBy, DamagedCharacter);
		}
	}

	return sum;
}

float UAbilityTalentComponent::CalculateModifiedDodgeSuccWithTraits(APlayerCharacter* Player, float OriginalDamage)
{
	auto sum = 0.f;

	for (auto iter : OnIncreaseDamWhenSuccDodge)
	{
		if (iter.Value.IsBound())
		{
			sum += iter.Value.Execute(Player, OriginalDamage);
		}
	}

	return sum;
}

float UAbilityTalentComponent::CalculateModifiedPhysicalDefenceWithTraits(ABaseCharacter* DamagedBy)
{
	if (auto atComp = GetOwner<ABaseCharacter>()->GetAttributeComponent())
	{
		auto def = atComp->GetPhysicalDefense();
		for (auto iter : OnChangePhysicalDefence)
		{
			if (iter.Value.IsBound())
			{
				def += iter.Value.Execute(def, DamagedBy, GetOwner<ABaseCharacter>());
			}
		}

		return def;
	}

	return 0;
}

float UAbilityTalentComponent::CalculateModifiedMagicalDefenceWithTraits(ABaseCharacter* DamagedBy)
{
	if (auto atComp = GetOwner<ABaseCharacter>()->GetAttributeComponent())
	{
		auto def = atComp->GetMagicalDefense();
		for (auto iter : OnChangeMagicalDefence)
		{
			if (iter.Value.IsBound())
			{
				def += iter.Value.Execute(def, DamagedBy, GetOwner<ABaseCharacter>());
			}
		}

		return def;
	}

	return 0;
}

float UAbilityTalentComponent::CalculateModifiedDefenceByMonsterTypeWithTraits(float Defence, ABaseCharacter* DamagedBy)
{
	if (auto atComp = GetOwner<ABaseCharacter>()->GetAttributeComponent())
	{
		for (auto iter : OnChangeDefenceByMonsterType)
		{
			if (iter.Value.IsBound())
			{
				Defence += iter.Value.Execute(Defence, DamagedBy, GetOwner<ABaseCharacter>());
			}
		}

		return Defence;
	}

	return 0;
}


void UAbilityTalentComponent::BroadcastOnSuccessDodge(ABaseCharacter* Character)
{
	for (auto iter : OnSuccessDodge)
	{
		if (iter.Value.IsBound())
		{
			iter.Value.Execute(Character);
		}
	}
}

void UAbilityTalentComponent::BroadcastOnSuccessHit(ABaseCharacter* DamagedBy, ABaseCharacter* DamagedCharacter)
{
	UE_LOGFMT(LogTemp, Warning, "{0}가 {1}에 대해 피해특성 적용하기 직전입니다.", DamagedBy->GetActorNameOrLabel(),
	          DamagedCharacter->GetActorNameOrLabel());
	for (auto iter : OnSuccessDamageHit)
	{
		if (iter.Value.IsBound())
		{
			iter.Value.ExecuteIfBound(DamagedBy, DamagedCharacter);
		}
	}
}

void UAbilityTalentComponent::BroadcastOnGotHit(ABaseCharacter* DamagedCharacter, ABaseCharacter* DamagedBy,
                                                float OriginalDamage)
{
	for (auto iter : OnGotHit)
	{
		if (iter.Value.IsBound())
		{
			iter.Value.Execute(DamagedCharacter, DamagedBy, OriginalDamage);
		}
	}
}

float UAbilityTalentComponent::BroadcastChangeHealAmount(ABaseCharacter* HealedCharacter, float RecoverAmount)
{
	auto sum = 0.f;

	for (auto iter : OnChangeHealAmount)
	{
		if (iter.Value.IsBound())
		{
			sum += iter.Value.Execute(RecoverAmount, HealedCharacter);
		}
	}

	return sum;
}

void UAbilityTalentComponent::BroadcastOnKillMonster(ABaseCharacter* Killer, ABaseCharacter* Monster)
{
	if (Monster != nullptr)
	{
		for (auto iter : OnKillMonster)
		{
			iter.Value.ExecuteIfBound(Cast<APlayerCharacter>(Killer), Cast<ABaseMonster>(Monster));
		}
	}
}

float UAbilityTalentComponent::BroadcastOnDodgeMinimumSP(float OriginalSP)
{
	if (OnDodgeMinimumSP.IsBound())
	{
		return OnDodgeMinimumSP.Execute(OriginalSP);
	}

	return 0;
}

float UAbilityTalentComponent::CalculateModifiedRunSPWithTraits(float OriginalStamina)
{
	auto sum = 0.f;

	for (auto iter : OnDecreaseRunSP)
	{
		if (iter.Value.IsBound())
		{
			sum += iter.Value.Execute(OriginalStamina);
		}
	}

	return sum;
}

/*
float UAbilityTalentComponent::CalculateModifiedDamagePtoMWithTraits(float OriginalP_Damage)
{
	auto sum = 0.f;
	for(auto iter : OnChangePtoM)
	{
		if(iter.Value.IsBound())
		{
			sum+=iter.Value.Execute(OriginalP_Damage);
		}
	}
	return sum;
}
*/
