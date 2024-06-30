// Fill out your copyright notice in the Description page of Project Settings.


#include "03_Widget/12_Dialog/DialogueReplyWidget.h"

#include "03_Widget/12_Dialog/DialogueWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/RichTextBlock.h"

ESlateVisibility UDialogueReplyWidget::GetSelectedReplyBackgroundVisibility()
{
	if(bSelected)
	{
		return ESlateVisibility::HitTestInvisible;
	}
	
	return ESlateVisibility::Collapsed;
}

void UDialogueReplyWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	if(SelectedReplyBackground)
	{
		SelectedReplyBackground->VisibilityDelegate.BindDynamic(this,&UDialogueReplyWidget::GetSelectedReplyBackgroundVisibility);
	}
}

void UDialogueReplyWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if(OwnerWidget.IsValid())
	{
		ReplyTextBlock->SetText(OwnerWidget->ReplaceVarStrings(ReplyNode.Text));
		Button_Reply->OnClicked.AddUniqueDynamic(this,&UDialogueReplyWidget::OnClickedReplyButton);
	}
}

void UDialogueReplyWidget::SetOwnerWidget(UDialogueWidget* NewOwner)
{
	OwnerWidget = NewOwner;
}

void UDialogueReplyWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{

	if(OwnerWidget.IsValid())
	{
		OwnerWidget->MouseSelectReply(this);
	}
}

void UDialogueReplyWidget::OnClickedReplyButton()
{
	if(OwnerWidget.IsValid())
	{
		OwnerWidget->PlayPlayerReply(ReplyNode);
	}
}

void UDialogueReplyWidget::SimulateClicked()
{
	OnClickedReplyButton();
}

