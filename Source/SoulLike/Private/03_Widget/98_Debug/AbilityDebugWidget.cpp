// Fill out your copyright notice in the Description page of Project Settings.


#include "03_Widget/98_Debug/AbilityDebugWidget.h"

#include "00_Character/BaseCharacter.h"
#include "00_Character/00_Player/PlayerCharacter.h"
#include "00_Character/01_Component/AbilityComponent.h"
#include "Components/ListView.h"
#include "Components/TextBlock.h"

void UAbilityDebugWidget::NativeConstruct()
{
	Super::NativeConstruct();

#ifdef WITH_EDITOR
	if (auto abComp = GetOwningPlayerPawn<ABaseCharacter>()->GetAbilityComponent())
	{
		//abComp->OnAddAbility.AddUniqueDynamic(this, &UAbilityDebugWidget::OnAddAbilityEvent);
		abComp->OnRemoveAbility.AddUniqueDynamic(this, &UAbilityDebugWidget::OnRemoveAbilityEvent);

		abComp->OnActivatedAbility.AddUniqueDynamic(this, &UAbilityDebugWidget::OnActivatedAbilityEvent);
		abComp->OnEndAbility.AddUniqueDynamic(this, &UAbilityDebugWidget::OnEndAbilityEvent);

		abComp->OnApplyEffect.AddUniqueDynamic(this, &UAbilityDebugWidget::OnApplyEffectEvent);
		abComp->OnEndEffect.AddUniqueDynamic(this, &UAbilityDebugWidget::OnEndEffectEvent);

		abComp->OnAddCue.AddUniqueDynamic(this, &UAbilityDebugWidget::OnAddCueEvent);
		abComp->OnRemoveCue.AddUniqueDynamic(this, &UAbilityDebugWidget::OnRemoveCueEvent);

		GetOwningPlayerPawn<ABaseCharacter>()->OnTriggerIgnoreMoveInput.AddUObject(
			this, &UAbilityDebugWidget::OnTriggerIgnoreMoveInputEvent);
		GetOwningPlayerPawn<APlayerCharacter>()->OnChangePlayerState.AddUniqueDynamic(
			this, &UAbilityDebugWidget::OnChangePlayerStateEvent);
	}
#endif
}

void UAbilityDebugWidget::OnAddAbilityEvent(const FAbilityInformation& AbilityInformation)
{
	/*if (AvailableAbilitiesList)
	{
		auto data = NewObject<UAbilityDebuggingData>();
		data->AbilityInformation = AbilityInformation;

		AvailableAbilitiesList->AddItem(data);
		AvailableAbilities.Add(AbilityInformation.AbilityTag, data);
	}*/
}

void UAbilityDebugWidget::OnRemoveAbilityEvent(const FGameplayTag& AbilityTag)
{
	if (AvailableAbilitiesList)
	{
		if (AvailableAbilities.Contains(AbilityTag))
		{
			AvailableAbilitiesList->RemoveItem(AvailableAbilities[AbilityTag]);
			AvailableAbilities.Remove(AbilityTag);
		}
	}
}

void UAbilityDebugWidget::OnActivatedAbilityEvent(const FAbilityInformation& AbilityInformation)
{
	if (ActivatedAbilitiesList)
	{
		auto data = NewObject<UAbilityDebuggingData>();
		data->AbilityInformation = AbilityInformation;

		ActivatedAbilities.Add(AbilityInformation.AbilityTag, data);
		ActivatedAbilitiesList->AddItem(data);
	}
}

void UAbilityDebugWidget::OnEndAbilityEvent()
{
	if (ActivatedAbilitiesList && GetOwningPlayerPawn<ABaseCharacter>() != nullptr)
	{
		auto abComp = GetOwningPlayerPawn<ABaseCharacter>()->GetAbilityComponent();
		auto tags = abComp->GetActivatedAbilityTags();

		for (auto iter : ActivatedAbilities)
		{
			if (tags.HasTagExact(iter.Key) == false)
			{
				ActivatedAbilitiesList->RemoveItem(ActivatedAbilities[iter.Key]);
				ActivatedAbilities.Remove(iter.Key);
				break;
			}
		}
	}
}

void UAbilityDebugWidget::OnApplyEffectEvent(const FGameplayTagContainer& EffectTag)
{
	if (AppliedEffectsList)
	{
		AppliedEffectsList->ClearListItems();

		for (auto iter : EffectTag)
		{
			if (auto data = NewObject<UAbilityEffectDebuggingData>())
			{
				data->AbilityEffectTag = iter;
				AppliedEffectsList->AddItem(data);
			}
		}
	}
}

void UAbilityDebugWidget::OnEndEffectEvent(const FGameplayTagContainer& EffectTag)
{
	if (AppliedEffectsList)
	{
		AppliedEffectsList->ClearListItems();

		for (auto iter : EffectTag)
		{
			if (auto data = NewObject<UAbilityEffectDebuggingData>())
			{
				data->AbilityEffectTag = iter;
				AppliedEffectsList->AddItem(data);
			}
		}
	}
}

void UAbilityDebugWidget::OnAddCueEvent(const FGameplayTag& CueTag)
{
	if (AppliedCueList)
	{
		if (!AddedCues.Contains(CueTag))
		{
			auto data = NewObject<UAbilityCueDebuggingData>();
			data->CueTag = CueTag;
			AddedCues.Add(CueTag, data);
			AppliedCueList->AddItem(data);
		}
	}
}

void UAbilityDebugWidget::OnRemoveCueEvent(const FGameplayTag& CueTag)
{
	if (AppliedCueList)
	{
		if (AddedCues.Contains(CueTag))
		{
			AppliedCueList->RemoveItem(AddedCues[CueTag]);
			AddedCues.Remove(CueTag);
		}
	}
}

void UAbilityDebugWidget::OnTriggerIgnoreMoveInputEvent(const TMap<FIgnoreInputHandler, uint8>& IgnoreMoveInputMap)
{
	if (AppliedIgnoreInputList)
	{
		AppliedIgnoreInputList->ClearListItems();
		for (auto iter : IgnoreMoveInputMap)
		{
			if (auto data = NewObject<UIgnoreMoveInputDebuggingData>())
			{
				data->By = iter.Key.AccruedActor;
				data->Tag = iter.Key.AccruedTag;
				data->Count = iter.Value;

				AppliedIgnoreInputList->AddItem(data);
			}
		}
	}
}

void UAbilityDebugWidget::OnChangePlayerStateEvent(ECombatState State)
{
	TextBlock_PlayerState->SetText(FText::FromString(StaticEnum<ECombatState>()->GetValueAsString(State)));
}
