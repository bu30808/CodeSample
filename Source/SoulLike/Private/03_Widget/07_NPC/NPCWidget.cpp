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


FString UNPCWidget::NPCActionTypeToString(ENPCActionType ActionType)
{
	switch (ActionType)
	{
	case ENPCActionType::Talk:
		return TEXT("대화");
	case ENPCActionType::Merchant:
		return TEXT("상점");
	case ENPCActionType::LevelUp:
		return TEXT("레벨업");
	case ENPCActionType::Enhancement:
		return TEXT("강화");
	case ENPCActionType::PotionEnhancement:
		return TEXT("회복약 업그레이드");
	case ENPCActionType::OrbMatrixSlotOpen:
		return TEXT("신력 슬롯 개방");
	case ENPCActionType::TeleportBonfire:
		return TEXT("이동");
	default: ;
	}

	return TEXT("없는 액션 타입입니다.");
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
				b->SetActionName(NPCActionTypeToString(a));
				ScrollBox_ActionList->AddChild(b);
				NPCActionButtons.Add(a, b);
			}
		}

		if (auto b = CreateWidget<UNPCActionButtonWidget>(this, NPCActionButtonObject))
		{
			b->SetActionName(TEXT("나가기"));
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
