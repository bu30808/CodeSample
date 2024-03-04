// Fill out your copyright notice in the Description page of Project Settings.


#include "03_Widget/06_Enhancement/00_Potion/PotionEnhancementWidget.h"

#include "00_Character/BaseCharacter.h"
#include "04_Item/ItemActor.h"
#include "04_Item/00_Consume/00_Potion/PotionItemActor.h"
#include "96_Library/WidgetHelperLibrary.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"

void UPotionEnhancementWidget::OnVisibilityChangedEvent(ESlateVisibility InVisibility)
{
	Super::OnVisibilityChangedEvent(InVisibility);

	if (IsVisible())
	{
		if (WidgetSwitcher)
		{
			WidgetSwitcher->SetActiveWidgetIndex(0);
		}

		Init();
	}
}

void UPotionEnhancementWidget::OnClickedUpgradeHPButton()
{
	if (PotionEnhancementComponent.IsValid())
	{
		if (PotionEnhancementComponent->CanEnhancePotion(
			GetOwningPlayerPawn<ABaseCharacter>()->GetInventoryComponent(), EEnhancedPotionType::HP))
		{
			PotionEnhancementComponent->UpgradePotion(GetOwningPlayerPawn<ABaseCharacter>()->GetInventoryComponent(),
			                                          EEnhancedPotionType::HP);
			Init();
		}
	}
}

void UPotionEnhancementWidget::OnClickedUpgradeMPButton()
{
	if (PotionEnhancementComponent.IsValid())
	{
		if (PotionEnhancementComponent->CanEnhancePotion(
			GetOwningPlayerPawn<ABaseCharacter>()->GetInventoryComponent(), EEnhancedPotionType::MP))
		{
			PotionEnhancementComponent->UpgradePotion(GetOwningPlayerPawn<ABaseCharacter>()->GetInventoryComponent(),
			                                            EEnhancedPotionType::MP);
			Init();
		}
	}
}

void UPotionEnhancementWidget::OnClickedHPButton()
{
	WidgetSwitcher->SetActiveWidgetIndex(0);
}

void UPotionEnhancementWidget::OnClickedMPButton()
{
	WidgetSwitcher->SetActiveWidgetIndex(1);
}

void UPotionEnhancementWidget::OnClickedCloseButton()
{
	UWidgetHelperLibrary::CloseWidgetSetting(GetOwningPlayer(), this);
}

void UPotionEnhancementWidget::NativeConstruct()
{
	bUseAsPopUp = true;
	bRemovable = true;
	Super::NativeConstruct();

	if (WidgetSwitcher)
	{
		WidgetSwitcher->SetActiveWidgetIndex(0);
	}

	Init();

	Button_UpgradeHP->OnClicked.AddUniqueDynamic(this, &UPotionEnhancementWidget::OnClickedUpgradeHPButton);
	Button_UpgradeMP->OnClicked.AddUniqueDynamic(this, &UPotionEnhancementWidget::OnClickedUpgradeMPButton);
	Button_HP->OnClicked.AddUniqueDynamic(this, &UPotionEnhancementWidget::OnClickedHPButton);
	Button_MP->OnClicked.AddUniqueDynamic(this, &UPotionEnhancementWidget::OnClickedMPButton);

	Button_Close->OnClicked.AddUniqueDynamic(this, &UPotionEnhancementWidget::OnClickedCloseButton);

	UWidgetHelperLibrary::ShowTutorialWidget(GetOwningPlayer(),
	                                         FGameplayTag::RequestGameplayTag("Tutorial.Enhancement.Potion"));
}

void UPotionEnhancementWidget::SetPotionEnhancementComponent(UPotionEnhancementComponent* Component)
{
	PotionEnhancementComponent = Component;
}

void UPotionEnhancementWidget::Init()
{
	//HP 포션 설정
	{
		if (auto player = GetOwningPlayerPawn<ABaseCharacter>())
		{
			if (auto potion = player->GetInventoryComponent()->GetItemByTag(
				FGameplayTag::RequestGameplayTag("Item.Consume.Potion.hp")))
			{
				if (auto potionActor = Cast<APotionItemActor>(potion->GetSpawndItemActor()))
				{
					const auto cur = potionActor->GetPotionEnhancement().CurEnhancement;
					const auto next = FMath::Clamp(cur + 1, 0, potionActor->GetPotionEnhancement().GetMaxEnhancement());
					TextBlock_CurEnhancementHP->SetText(FText::AsNumber(cur));
					TextBlock_NextEnhancementHP->SetText(FText::AsNumber(next));

					TextBlock_ButtonTextHP->SetText(FText::FromString(TEXT("강화 최대치 도달")));
				}
			}
		}

		if (PotionEnhancementComponent.IsValid())
		{
			bool bAvailable = PotionEnhancementComponent->CanEnhancePotion(
				GetOwningPlayerPawn<ABaseCharacter>()->GetInventoryComponent(), EEnhancedPotionType::HP);
			Button_UpgradeHP->SetIsEnabled(bAvailable);

			if (bAvailable)
			{
				TextBlock_ButtonTextHP->SetText(FText::FromString(TEXT("업그레이드")));
			}
			else
			{
				TextBlock_ButtonTextHP->SetText(FText::FromString(TEXT("활력의 결정이 모자라거나, 강화 최대수치에 도달했습니다.")));
			}
		}
	}

	//MP포션 설정
	{
		if (auto player = GetOwningPlayerPawn<ABaseCharacter>())
		{
			if (auto potion = player->GetInventoryComponent()->GetItemByTag(
				FGameplayTag::RequestGameplayTag("Item.Consume.Potion.mp")))
			{
				if (auto potionActor = Cast<APotionItemActor>(potion->GetSpawndItemActor()))
				{
					const auto cur = potionActor->GetPotionEnhancement().CurEnhancement;
					const auto next = FMath::Clamp(cur + 1, 0, potionActor->GetPotionEnhancement().GetMaxEnhancement());
					TextBlock_CurEnhancementMP->SetText(FText::AsNumber(cur));
					TextBlock_NextEnhancementMP->SetText(FText::AsNumber(next));

					TextBlock_ButtonTextMP->SetText(FText::FromString(TEXT("강화 최대치 도달")));
				}
			}
		}

		if (PotionEnhancementComponent.IsValid())
		{
			bool bAvailable = PotionEnhancementComponent->CanEnhancePotion(
				GetOwningPlayerPawn<ABaseCharacter>()->GetInventoryComponent(), EEnhancedPotionType::MP);
			Button_UpgradeMP->SetIsEnabled(bAvailable);

			if (bAvailable)
			{
				TextBlock_ButtonTextMP->SetText(FText::FromString(TEXT("업그레이드")));
			}
			else
			{
				TextBlock_ButtonTextMP->SetText(FText::FromString(TEXT("신성한 결정이 모자라거나, 강화 최대수치에 도달했습니다.")));
			}
		}
	}
}
