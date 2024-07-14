// Fill out your copyright notice in the Description page of Project Settings.


#include "00_EditorUtilityWidget/02_AbilityEffectHandler/AbilityEffectHandlerWidget.h"

#include "SoulLikeEditor.h"
#include "02_Library/BlueprintHelperLibrary.h"
#include "Components/Button.h"
#include "Components/CheckBox.h"
#include "Components/ComboBoxString.h"
#include "Components/DetailsView.h"
#include "Components/VerticalBox.h"
#include "Misc/MessageDialog.h"

#define EmptyOption TEXT("비어있음")

UAbilityEffectHandler::UAbilityEffectHandler()
{
	if (AbilityEffect == nullptr)
	{
		AbilityEffect = NewObject<UAbilityEffect>();
	}
}

UAbilityEffectHandler_Linetrace::UAbilityEffectHandler_Linetrace()
{
	if (AbilityEffect != nullptr)
	{
		AbilityEffect->ConditionalBeginDestroy();
		AbilityEffect = nullptr;
	}

	if(AbilityEffect == nullptr)
	{
		AbilityEffect = NewObject<UAbilityEffect_Linetrace>();
	}
}

void UAbilityEffectHandlerWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (CheckBox_Player)
	{
		CheckBox_Player->OnCheckStateChanged.AddUniqueDynamic(
			this, &UAbilityEffectHandlerWidget::OnPlayerCheckStateChanged);
	}
	if (CheckBox_Monster)
	{
		CheckBox_Monster->OnCheckStateChanged.AddUniqueDynamic(
			this, &UAbilityEffectHandlerWidget::OnMonsterCheckStateChanged);
	}
	if (CheckBox_Magic)
	{
		CheckBox_Magic->OnCheckStateChanged.AddUniqueDynamic(
			this, &UAbilityEffectHandlerWidget::OnMagicCheckStateChanged);
	}
	if (CheckBox_Physic)
	{
		CheckBox_Physic->OnCheckStateChanged.AddUniqueDynamic(
			this, &UAbilityEffectHandlerWidget::OnPhysicCheckStateChanged);
	}
	if (CheckBox_Cost)
	{
		CheckBox_Cost->OnCheckStateChanged.
		               AddUniqueDynamic(this, &UAbilityEffectHandlerWidget::OnCostCheckStateChanged);
	}

	if (VerticalBox_Filter2)
	{
		VerticalBox_Filter2->SetVisibility(ESlateVisibility::Hidden);
	}

	if (ComboBoxString_Effect)
	{
		ComboBoxString_Effect->OnSelectionChanged.AddUniqueDynamic(
			this, &UAbilityEffectHandlerWidget::OnSelectionChanged);
	}

	if (Button_Apply)
	{
		Button_Apply->OnClicked.AddUniqueDynamic(this, &UAbilityEffectHandlerWidget::OnClickedApplyButton);
	}

	AbilityEffects = UBlueprintHelperLibrary::LoadBlueprintClass<UAbilityEffect>(UAbilityEffect::StaticClass());
}

void UAbilityEffectHandlerWidget::UpdateOptionWithSearchTag()
{
	ComboBoxString_Effect->ClearOptions();
	CharacterFilteredEffect.Empty();

	for (auto iter : AbilityEffects)
	{
		const auto& tag = Cast<UAbilityEffect>(iter.Value.GetDefaultObject())->GetEffectTag();
		for (auto searchTag : SearchTag)
		{
			if (tag.ToString().Contains(searchTag))
			{
				CharacterFilteredEffect.AddUnique(iter);
				/*if(ComboBoxString_Effect->FindOptionIndex(iter.Key) == INDEX_NONE)
				{
					ComboBoxString_Effect->AddOption(iter.Key);
				}*/
			}
		}
	}
}

void UAbilityEffectHandlerWidget::OnPlayerCheckStateChanged(bool bIsChecked)
{
	if (bIsChecked)
	{
		if (!VerticalBox_Filter2->IsVisible())
		{
			VerticalBox_Filter2->SetVisibility(ESlateVisibility::Visible);
		}

		SearchTag.AddUnique("Character");
	}
	else
	{
		SearchTag.Remove("Character");
	}
	UpdateOptionWithSearchTag();
}

void UAbilityEffectHandlerWidget::OnMonsterCheckStateChanged(bool bIsChecked)
{
	if (bIsChecked)
	{
		if (!VerticalBox_Filter2->IsVisible())
		{
			VerticalBox_Filter2->SetVisibility(ESlateVisibility::Visible);
		}

		SearchTag.AddUnique("Monster");
	}
	else
	{
		SearchTag.Remove("Monster");
	}
	UpdateOptionWithSearchTag();
}

void UAbilityEffectHandlerWidget::OnMagicCheckStateChanged(bool bIsChecked)
{
	ComboBoxString_Effect->RemoveOption(EmptyOption);
	if (bIsChecked)
	{
		//마법 친구들을 추가합니다.
		for (auto iter : CharacterFilteredEffect)
		{
			auto effect = iter.Value.GetDefaultObject();
			if (effect->bIsHitDamageEffect && effect->AttackType == EAttackType::Magical)
			{
				if (ComboBoxString_Effect->FindOptionIndex(iter.Key) == INDEX_NONE)
				{
					ComboBoxString_Effect->AddOption(iter.Key);
				}
			}
		}

		if (ComboBoxString_Effect->GetOptionCount() == 0)
		{
			ComboBoxString_Effect->AddOption(EmptyOption);
		}
	}
	else
	{
		//마법 친구들을 제거합니다.
		for (auto iter : CharacterFilteredEffect)
		{
			auto effect = iter.Value.GetDefaultObject();
			if (effect->bIsHitDamageEffect && effect->AttackType == EAttackType::Magical)
			{
				if (ComboBoxString_Effect->FindOptionIndex(iter.Key) != INDEX_NONE)
				{
					ComboBoxString_Effect->RemoveOption(iter.Key);
				}
			}
		}
	}
}

void UAbilityEffectHandlerWidget::OnPhysicCheckStateChanged(bool bIsChecked)
{
	//태그로 찾은 목록중, 내부 변수 정보가 물리인 친구들을 찾습니다.
	ComboBoxString_Effect->RemoveOption(EmptyOption);
	if (bIsChecked)
	{
		//물리 친구들을 추가합니다.
		for (auto iter : CharacterFilteredEffect)
		{
			auto effect = iter.Value.GetDefaultObject();
			if (effect->bIsHitDamageEffect && effect->AttackType == EAttackType::Physical)
			{
				if (ComboBoxString_Effect->FindOptionIndex(iter.Key) == INDEX_NONE)
				{
					ComboBoxString_Effect->AddOption(iter.Key);
				}
			}
		}

		if (ComboBoxString_Effect->GetOptionCount() == 0)
		{
			ComboBoxString_Effect->AddOption(EmptyOption);
		}

		/*for(auto iter : CharacterFilteredEffect)
		{
			auto effect = iter.Value.GetDefaultObject();
			//마법인 친구들을 제거합니다.
			if(!effect->bIsHitDamageEffect || effect->AttackType != EAttackType::Physical)
			{
				RemovedNotPhysicOption.AddUnique(iter.Key);
				ComboBoxString_Effect->RemoveOption(iter.Key);
			}else
			{
				if(ComboBoxString_Effect->FindOptionIndex(iter.Key) == INDEX_NONE)
				{
					ComboBoxString_Effect->AddOption(iter.Key);
				}
			}
			
		}*/
	}
	else
	{
		//물리 친구들을 제거합니다.
		for (auto iter : CharacterFilteredEffect)
		{
			auto effect = iter.Value.GetDefaultObject();
			if (effect->bIsHitDamageEffect && effect->AttackType == EAttackType::Physical)
			{
				if (ComboBoxString_Effect->FindOptionIndex(iter.Key) != INDEX_NONE)
				{
					ComboBoxString_Effect->RemoveOption(iter.Key);
				}
			}
		}
		/*for(const auto& option : RemovedNotPhysicOption)
		{
			if(ComboBoxString_Effect->FindOptionIndex(option) == INDEX_NONE)
			{
				ComboBoxString_Effect->AddOption(option);
			}
		}*/
	}
}

void UAbilityEffectHandlerWidget::OnCostCheckStateChanged(bool bIsChecked)
{
	ComboBoxString_Effect->RemoveOption(EmptyOption);
	if (bIsChecked)
	{
		//코스트 친구들을 추가합니다.
		for (auto iter : CharacterFilteredEffect)
		{
			auto effect = iter.Value.GetDefaultObject();
			if (effect->GetEffectTag().ToString().Contains("Cost"))
			{
				if (ComboBoxString_Effect->FindOptionIndex(iter.Key) == INDEX_NONE)
				{
					ComboBoxString_Effect->AddOption(iter.Key);
				}
			}
		}

		if (ComboBoxString_Effect->GetOptionCount() == 0)
		{
			ComboBoxString_Effect->AddOption(EmptyOption);
		}
	}
	else
	{
		//코스트 친구들을 제거합니다.
		for (auto iter : CharacterFilteredEffect)
		{
			auto effect = iter.Value.GetDefaultObject();
			if (effect->GetEffectTag().ToString().Contains("Cost"))
			{
				if (ComboBoxString_Effect->FindOptionIndex(iter.Key) != INDEX_NONE)
				{
					ComboBoxString_Effect->RemoveOption(iter.Key);
				}
			}
		}
	}
}

void UAbilityEffectHandlerWidget::OnSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	if (ComboBoxString_Effect)
	{
		if (ComboBoxString_Effect->FindOptionIndex(SelectedItem) != INDEX_NONE)
		{


			if (AbilityEffects.Contains(SelectedItem))
			{
				SelectedEffect = AbilityEffects[SelectedItem];
				if(SelectedEffect->GetDefaultObject()->IsA<UAbilityEffect_Linetrace>())
				{
					if (EffectHandler != nullptr)
					{
						if(!EffectHandler->IsA<UAbilityEffectHandler_Linetrace>())
						{
							EffectHandler->ConditionalBeginDestroy();
							EffectHandler = nullptr;
							EffectHandler = NewObject<UAbilityEffectHandler_Linetrace>();
							
						}
					}

					if(EffectHandler ==nullptr)
					{
						EffectHandler = NewObject<UAbilityEffectHandler_Linetrace>();
					}
					
					CopyOriginalToTarget<UAbilityEffect_Linetrace>(Cast<UAbilityEffect_Linetrace>(SelectedEffect->GetDefaultObject()), Cast<UAbilityEffect_Linetrace>(EffectHandler->AbilityEffect));
					DetailsView_Effect->SetObject(EffectHandler);
				}else
				{
					if (EffectHandler != nullptr)
					{
						if(EffectHandler->IsA<UAbilityEffectHandler_Linetrace>())
						{
							EffectHandler->ConditionalBeginDestroy();
							EffectHandler = nullptr;
							EffectHandler = NewObject<UAbilityEffectHandler>();
						}
					}

					if(EffectHandler ==nullptr)
					{
						EffectHandler = NewObject<UAbilityEffectHandler>();
					}
					
					auto effect = SelectedEffect.GetDefaultObject();
					CopyOriginalToTarget<UAbilityEffect>(effect, EffectHandler->AbilityEffect);
					DetailsView_Effect->SetObject(EffectHandler);
				}
				
			
			}
			else
			{
				FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(TEXT("가져온 이팩트 중에 선택한 대상이 없습니다.")));
			}
		}
	}
}

void UAbilityEffectHandlerWidget::OnClickedApplyButton()
{
	if (SelectedEffect != nullptr)
	{
		//CopyOriginalToTarget<>(EffectHandler->AbilityEffect, SelectedEffect.GetDefaultObject());

		if (SelectedEffect->MarkPackageDirty())
		{
			FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(TEXT("성공적으로 적용되었습니다. 저장하지 않으면 날아갑니다.")));
		}
	}
}
