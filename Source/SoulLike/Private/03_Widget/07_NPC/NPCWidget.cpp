// Fill out your copyright notice in the Description page of Project Settings.


#include "03_Widget/07_NPC/NPCWidget.h"

#include "00_Character/04_NPC/NPCBase.h"
#include "03_Widget/07_NPC/NPCActionButtonWidget.h"
#include "96_Library/WidgetHelperLibrary.h"
#include "Components/Button.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Logging/StructuredLog.h"

#define LOCTEXT_NAMESPACE "NPCWidget"

namespace GlobalNPCWidgetText
{
	const FText talkText = NSLOCTEXT("EnhancementButtonWidget","TalkText","대화");
	const FText merchantText = NSLOCTEXT("EnhancementButtonWidget","MerchantText","상점");
	const FText levelUpText = NSLOCTEXT("EnhancementButtonWidget","LevelUpText","레벨업");
	const FText enhancementText = NSLOCTEXT("EnhancementButtonWidget","EnhancementText","강화");
	const FText potionEnhancementText = NSLOCTEXT("EnhancementButtonWidget","PotionEnhancementText","회복약 업그레이드");
	const FText orbMatrixSlotOpenText = NSLOCTEXT("EnhancementButtonWidget","OrbMatrixSlotOpenText","신력 슬롯 개방");
	const FText teleportBonfireText = NSLOCTEXT("EnhancementButtonWidget","TeleportBonfireText","이동");
	const FText exitText = NSLOCTEXT("EnhancementButtonWidget","ExitText","나가기");
	const FText noText = NSLOCTEXT("EnhancementButtonWidget","NoText","없는 액션 타입입니다.");
}
#undef LOCTEXT_NAMESPACE

FText UNPCWidget::NPCActionTypeToText(ENPCActionType ActionType)
{
	switch (ActionType)
	{
	case ENPCActionType::Talk:
		return GlobalNPCWidgetText::talkText;
	case ENPCActionType::Merchant:
		return GlobalNPCWidgetText::merchantText;
	case ENPCActionType::LevelUp:
		return GlobalNPCWidgetText::levelUpText;
	case ENPCActionType::Enhancement:
		return GlobalNPCWidgetText::enhancementText;
	case ENPCActionType::PotionEnhancement:
		return GlobalNPCWidgetText::potionEnhancementText;
	case ENPCActionType::OrbMatrixSlotOpen:
		return GlobalNPCWidgetText::orbMatrixSlotOpenText;
	case ENPCActionType::TeleportBonfire:
		return GlobalNPCWidgetText::teleportBonfireText;
	default: ;
	}

	return GlobalNPCWidgetText::noText;
}


void UNPCWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
}

void UNPCWidget::NativeConstruct()
{
	bUseAsPopUp = true;
	bRemovable = true;
	Super::NativeConstruct();
	
}


void UNPCWidget::CreateActionList()
{
	if (NPCActionButtonObject && NPCOwner.IsValid())
	{
		auto NPCActions = NPCOwner->GetNPCActions();

		ScrollBox_ActionList->ClearChildren();
		NPCActionButtons.Empty();
		for (auto a : NPCActions)
		{
			if (auto b = CreateWidget<UNPCActionButtonWidget>(this, NPCActionButtonObject))
			{
				b->SetActionName(NPCActionTypeToText(a));
				ScrollBox_ActionList->AddChild(b);
				NPCActionButtons.Add(a, b);
			}
		}

		if (auto b = CreateWidget<UNPCActionButtonWidget>(this, NPCActionButtonObject))
		{
			b->SetActionName(GlobalNPCWidgetText::exitText);
			ScrollBox_ActionList->AddChild(b);
			ExitButton = b;
		}
	}
}

void UNPCWidget::SetNPC(ANPCBase* NPC)
{
	NPCOwner = NPC;
}

void UNPCWidget::SetNPCName(const FName& NewName)
{
	TextBlock_NPCName->SetText(FText::FromName(NewName));
}

void UNPCWidget::BindAction()
{
	if (NPCOwner.IsValid())
	{
		for (auto iter : NPCActionButtons)
		{
			iter.Value->Button->OnClicked = NPCOwner->GetAction(iter.Key);
		}

		ExitButton->Button->OnClicked.AddUniqueDynamic(NPCOwner.Get(), &ANPCBase::ExitEvent);
	}
}
