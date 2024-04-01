// Fill out your copyright notice in the Description page of Project Settings.


#include "96_Library/AbilityHelperLibrary.h"

#include "AIController.h"
#include "GameplayTagContainer.h"
#include "NavigationSystem.h"
#include "00_Character/00_Player/00_Controller/UserController.h"
#include "00_Character/01_Component/AbilityComponent.h"
#include "00_Character/01_Component/AttributeComponent.h"
#include "00_Character/03_Monster/BaseMonster.h"
#include "00_Character/03_Monster/00_Controller/MonsterAIController.h"
#include "04_Item/01_Equipment/00_Spirit/EquipmentItemActor_Spirit.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Logging/StructuredLog.h"

bool UAbilityHelperLibrary::IsAttackAbilityTag(const FGameplayTag& Tag)
{
	const FString tagString = Tag.ToString();
	UE_LOGFMT(LogTemp, Log, "공격 어빌리티 태그입니까? : {0}", tagString);
	return tagString.Contains("Attack");
}

bool UAbilityHelperLibrary::IsNormalAttackAbilityTag(const FGameplayTag& Tag)
{
	const FString tagString = Tag.ToString();
	UE_LOGFMT(LogTemp, Log, "일반공격 어빌리티 태그입니까? : {0}", tagString);
	TArray<FString> token;
	tagString.ParseIntoArray(token,TEXT("."));
	return token[token.Num() - 1].Contains("Normal");
}

bool UAbilityHelperLibrary::IsStrongAttackAbilityTag(const FGameplayTag& Tag)
{
	const FString tagString = Tag.ToString();
	UE_LOGFMT(LogTemp, Log, "강공격 어빌리티 태그입니까? : {0}", tagString);
	TArray<FString> token;
	tagString.ParseIntoArray(token,TEXT("."));
	return token[token.Num() - 1].Contains("Strong");
}

bool UAbilityHelperLibrary::IsDodgeAbilityTag(const FGameplayTag& Tag)
{
	const FString tagString = Tag.ToString();
	TArray<FString> token;
	tagString.ParseIntoArray(token,TEXT("."));

	return token[token.Num() - 1].Equals("Dodge");
}

bool UAbilityHelperLibrary::IsDodgeContAbilityTag(const FGameplayTag& Tag)
{
	const FString tagString = Tag.ToString();
	TArray<FString> token;
	tagString.ParseIntoArray(token,TEXT("."));

	return token[token.Num() - 2].Equals("Dodge") && token[token.Num() - 1].Equals("Cont");
}

FGameplayTag UAbilityHelperLibrary::GetNormalAttackTag(UInventoryComponent* InventoryComponent)
{
	if (InventoryComponent)
	{
		const auto& item = InventoryComponent->GetEquippedWeapon();
		return Cast<AEquipmentItemActor_Spirit>(item.GetSpawndItemActor())->GetNormalAttackTag();
	}

	return FGameplayTag();
}

FGameplayTag UAbilityHelperLibrary::GetStrongAttackTag(UInventoryComponent* InventoryComponent)
{
	if (InventoryComponent)
	{
		const auto& item = InventoryComponent->GetEquippedWeapon();
		return Cast<AEquipmentItemActor_Spirit>(item.GetSpawndItemActor())->GetStrongAttackTag();
	}

	return FGameplayTag();
}

FGameplayTag UAbilityHelperLibrary::GetDodgeTag(UInventoryComponent* InventoryComponent)
{
	if (InventoryComponent)
	{
		const auto& item = InventoryComponent->GetEquippedWeapon();
		return Cast<AEquipmentItemActor_Spirit>(item.GetSpawndItemActor())->GetDodgeTag();
	}

	return FGameplayTag();
}

FGameplayTag UAbilityHelperLibrary::GetDodgeContTag(UInventoryComponent* InventoryComponent)
{
	if (InventoryComponent)
	{
		const auto& item = InventoryComponent->GetEquippedWeapon();
		return Cast<AEquipmentItemActor_Spirit>(item.GetSpawndItemActor())->GetDodgeContTag();
	}

	return FGameplayTag();
}

ABaseCharacter* UAbilityHelperLibrary::GetTargetFromBlackboard(AActor* AIMonster, FName Key)
{
	if (AIMonster->IsA<ABaseCharacter>())
	{
		if (const auto aiCon = Cast<AAIController>(Cast<ABaseCharacter>(AIMonster)->GetController()))
		{
			if (const auto bbComp = aiCon->GetBlackboardComponent())
			{
				if (const auto target = bbComp->GetValueAsObject(Key))
				{
					return Cast<ABaseCharacter>(target);
				}
			}
		}
	}


	return nullptr;
}

ABaseCharacter* UAbilityHelperLibrary::GetTargetFromBlackboard_AICon(AAIController* AICon, FName Key)
{
	if (AICon)
	{
		UE_LOGFMT(LogTemp, Error, "{0}", AICon->GetActorNameOrLabel());

		return GetTargetFromBlackboard(AICon->GetPawn(), Key);
	}
	return nullptr;
}

bool UAbilityHelperLibrary::IsDead(AActor* Actor)
{
	if (auto character = Cast<ABaseCharacter>(Actor))
	{
		return character->IsDead();
	}
	return false;
}

UGameInstance* UAbilityHelperLibrary::GetGameInstance(ABaseCharacter* Character)
{
	return UGameplayStatics::GetGameInstance(Character);
}

FGenericTeamId UAbilityHelperLibrary::GetTeam(ABaseCharacter* Character)
{
	if (Character->IsA<APlayerCharacter>())
	{
		return Cast<AUserController>(Character->GetController())->GetGenericTeamId();
	}

	if (Character->IsA<ABaseMonster>())
	{
		return Cast<AMonsterAIController>(Character->GetController())->GetGenericTeamId();
	}

	return FGenericTeamId::NoTeam;
}

FGameplayTag UAbilityHelperLibrary::GetStatusEffectTag(EStatusEffect StatusEffect)
{
	switch (StatusEffect)
	{
	case EStatusEffect::POISON:
		return FGameplayTag::RequestGameplayTag("Common.StatusEffect.Poison.Effect");
	case EStatusEffect::DEADLY_POISON:
		return FGameplayTag::RequestGameplayTag("Common.StatusEffect.DeadlyPoison.Effect");
	case EStatusEffect::BURN:
		return FGameplayTag::RequestGameplayTag("Common.StatusEffect.Burn.Effect");
	case EStatusEffect::CHILL:
		return FGameplayTag::RequestGameplayTag("Common.StatusEffect.Chill.Effect");
	case EStatusEffect::FREEZE:
		break;
	case EStatusEffect::BLEED:
		return FGameplayTag::RequestGameplayTag("Common.StatusEffect.Bleed.Effect");
	case EStatusEffect::PETRIFACTION:
		return FGameplayTag::RequestGameplayTag("Common.StatusEffect.Petrifaction.Effect");
	case EStatusEffect::MAX:
		break;
	}

	return FGameplayTag::EmptyTag;
}

bool UAbilityHelperLibrary::IsMovementBlockedByStatusEffect(ABaseCharacter* Character)
{
	const TArray arr = {
		FGameplayTag::RequestGameplayTag("Common.StatusEffect.Freeze.Effect"),
		FGameplayTag::RequestGameplayTag("Common.StatusEffect.Petrifaction.Effect")
	};

	const auto& tags = FGameplayTagContainer::CreateFromArray(arr);
	return Character->GetAbilityComponent()->HasAnyEffectTag(tags);
}

bool UAbilityHelperLibrary::IsStatusEffectActive(ABaseCharacter* Character, EStatusEffect StatusEffect)
{
	return Character->GetAbilityComponent()->HasEffectTag(GetStatusEffectTag(StatusEffect));
}

void UAbilityHelperLibrary::PlaySound2D(UObject* WorldContext, USoundBase* Sound)
{
	if (WorldContext)
	{
		UGameplayStatics::PlaySound2D(WorldContext, Sound);
	}
}
