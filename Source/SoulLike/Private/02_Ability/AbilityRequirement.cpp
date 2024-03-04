// Fill out your copyright notice in the Description page of Project Settings.


#include "02_Ability/AbilityRequirement.h"

#include "00_Character/BaseCharacter.h"
#include "00_Character/01_Component/AbilityComponent.h"
#include "00_Character/01_Component/AttributeComponent.h"
#include "02_Ability/AbilityEffect.h"
#include "Logging/StructuredLog.h"

bool UAbilityRequirement::IsActivateAbility(const FGameplayTag& AbilityTag, ABaseCharacter* AbilityOwner, UAbilityBase* Ability)
{
	bool bCanActivate = IsCostEnough(AbilityOwner);
	if (bCanActivate == false)
	{
		UE_LOGFMT(LogTemp, Error, "{0} : 사용하기 위한 코스트가 충분하지 않습니다.", AbilityTag.ToString());
		Ability->OnCommitFailedByCost.Broadcast();
		return bCanActivate;
	}

	bCanActivate &= !IsCooldownRunning(AbilityOwner);
	if (bCanActivate == false)
	{
		UE_LOGFMT(LogTemp, Error, "{0} : 쿨타임이 적용중입니다.", AbilityTag.ToString());
		Ability->OnCommitFailedByCooldown.Broadcast();
		return bCanActivate;
	}


	bCanActivate &= HasAllEffectTagsToNeed(AbilityOwner);
	if (bCanActivate == false)
	{
		UE_LOGFMT(LogTemp, Error, "{0} : 발동에 필요한 이팩트 태그를 충족하지 않습니다.{1} {2}", AbilityTag.ToString(),__FUNCTION__,__LINE__);
		return bCanActivate;
	}


	bCanActivate &= !HasAnyEffectTagsToNOTNeed(AbilityOwner);
	if (bCanActivate == false)
	{
		UE_LOGFMT(LogTemp, Error, "{0} : 발동에 필요하지 않은 이팩트 태그를 소유중입니다.{1} {2}", AbilityTag.ToString(),__FUNCTION__,__LINE__);
		return bCanActivate;
	}

	bCanActivate &= HasAllActivatedAbilityTagsToNeed(AbilityOwner);
	if (bCanActivate == false)
	{
		UE_LOGFMT(LogTemp, Error, "{0} : 발동에 필요한 어빌리티 태그를 충족하지 않습니다.{1} {2}", AbilityTag.ToString(),__FUNCTION__,__LINE__);
		return bCanActivate;
	}

	bCanActivate &= !HasAnyActivatedAbilityTagsToNOTNeed(AbilityOwner);
	if (bCanActivate == false)
	{
		UE_LOGFMT(LogTemp, Error, "{0} : 발동에 필요하지 않은 어빌리티 태그를 소유중입니다.{1} {2}", AbilityTag.ToString(),__FUNCTION__,__LINE__);
		return bCanActivate;
	}

	return bCanActivate;
	/*return IsCostEnough(AbilityOwner) && !IsCooldownRunning(AbilityOwner) &&
		HasAllEffectTagsToNeed(AbilityOwner) &&
		!HasAnyEffectTagsToNOTNeed(AbilityOwner) && HasAllActivatedTagsToNeed(AbilityOwner) &&
		!HasActivatedTagsToNOTNeed(AbilityOwner);*/
}

bool UAbilityRequirement::IsCostEnough_Implementation(ABaseCharacter* AbilityOwner)
{
	if (Cost != nullptr)
	{
		if (auto attributeComponent = AbilityOwner->GetAttributeComponent())
		{
			//단순하게 적용하려는 값 보다 크거나 같은 값을 가지고 있는지만 확인합니다.
			auto attEffect = Cost->GetDefaultObject<UAbilityEffect>()->GetAttributeEffects();
			for (auto e : attEffect)
			{
				const auto& value = e.ApplyValue;
				switch (e.Attribute)
				{
				case EAttributeType::NONE: break;
				case EAttributeType::HP:
					return attributeComponent->GetHP() >= value;
				case EAttributeType::SP:
					return attributeComponent->GetSP() >= value;
				case EAttributeType::MP:
					return attributeComponent->GetMP() >= value;
				case EAttributeType::MaxHP:
					return attributeComponent->GetMaxHP() >= value;
				case EAttributeType::MaxSP:
					return attributeComponent->GetMaxSP() >= value;
				case EAttributeType::MaxMP:
					return attributeComponent->GetMaxMP() >= value;
				case EAttributeType::RecoverHP:
					return attributeComponent->GetRecoverHP() >= value;
				case EAttributeType::RecoverSP:
					return attributeComponent->GetRecoverSP() >= value;
				case EAttributeType::RecoverMP:
					return attributeComponent->GetRecoverHitMP() >= value;
				case EAttributeType::PhysicalAttack:
					return attributeComponent->GetPhysicalAttack() >= value;
				case EAttributeType::MagicalAttack:
					return attributeComponent->GetMagicalAttack() >= value;
				case EAttributeType::PhysicalDefense:
					return attributeComponent->GetPhysicalDefense() >= value;
				case EAttributeType::MagicalDefense:
					return attributeComponent->GetMagicalDefense() >= value;
				case EAttributeType::Endurance:
					return attributeComponent->GetEndurance() >= value;
				case EAttributeType::Strength:
					return attributeComponent->GetStrength() >= value;
				case EAttributeType::Dexterity:
					return attributeComponent->GetDexterity() >= value;
				case EAttributeType::Intelligence:
					return attributeComponent->GetIntelligence() >= value;
				case EAttributeType::Willpower:
					return attributeComponent->GetWillpower() >= value;

				default: ;
				}
			}
		}
	}

	return true;
}

bool UAbilityRequirement::IsCooldownRunning(ABaseCharacter* AbilityOwner)
{
	if (Cooldown == nullptr)
	{
		return false;
	}

	if (auto abComp = GetAbilityComponent(AbilityOwner))
	{
		return abComp->HasEffectTag(Cooldown->GetDefaultObject<UAbilityEffect>()->GetEffectTag());
	}

	return false;
}

bool UAbilityRequirement::HasAllEffectTagsToNeed(ABaseCharacter* AbilityOwner)
{
	return GetAbilityComponent(AbilityOwner)->HasAllEffectTag(NeedEffectTags);
}

bool UAbilityRequirement::HasAnyEffectTagsToNOTNeed(ABaseCharacter* AbilityOwner)
{
	return GetAbilityComponent(AbilityOwner)->HasAnyEffectTag(NotNeedEffectTags);
}

bool UAbilityRequirement::HasAllActivatedAbilityTagsToNeed(ABaseCharacter* AbilityOwner)
{
	return GetAbilityComponent(AbilityOwner)->HasAllActivateAbilityTags(NeedActivatedAbilityTags);
}

bool UAbilityRequirement::HasAnyActivatedAbilityTagsToNOTNeed(ABaseCharacter* AbilityOwner)
{
	return GetAbilityComponent(AbilityOwner)->HasAnyActivateAbilityTags(NotNeedActivatedAbilityTags);
}

/*
void UAbilityRequirement::ApplyCost(ABaseCharacter* AbilityOwner)
{
	if(auto comp = GetAbilityComponent(AbilityOwner))
	{
		if(Cost!=nullptr){
			comp->ProcessEffect(Cost);
		}
	}
}

void UAbilityRequirement::ApplyCooldown(ABaseCharacter* AbilityOwner)
{
	if(auto comp = GetAbilityComponent(AbilityOwner))
	{
		if(Cooldown!=nullptr){
			comp->ProcessEffect(Cooldown);
		}
	}
}
*/


UAbilityComponent* UAbilityRequirement::GetAbilityComponent(ABaseCharacter* AbilityOwner)
{
	return AbilityOwner->GetAbilityComponent();
}
