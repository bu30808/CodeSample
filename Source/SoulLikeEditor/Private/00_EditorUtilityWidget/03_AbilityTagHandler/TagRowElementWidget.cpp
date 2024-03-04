// Fill out your copyright notice in the Description page of Project Settings.


#include "00_EditorUtilityWidget/03_AbilityTagHandler/TagRowElementWidget.h"

#include "00_EditorUtilityWidget/03_AbilityTagHandler/AddTagWidget.h"
#include "00_EditorUtilityWidget/03_AbilityTagHandler/RemoveTagAlertWidget.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/MenuAnchor.h"
#include "Components/TextBlock.h"

void UTagRowElementWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if(Button_AddChildTag)
	{
		Button_AddChildTag->OnClicked.AddUniqueDynamic(this,&UTagRowElementWidget::OnClickedAddButton);
	}

	if(Button_RemoveTag)
	{
		Button_RemoveTag->OnClicked.AddUniqueDynamic(this,&UTagRowElementWidget::OnClickedRemoveButton);
	}

	if(MenuAnchor_AddChildTag)
	{
		MenuAnchor_AddChildTag->OnGetUserMenuContentEvent.BindDynamic(this,&UTagRowElementWidget::OnAddChildTagWidget);
	}
	
	if(MenuAnchor_RemoveTag)
	{
		MenuAnchor_RemoveTag->OnGetUserMenuContentEvent.BindDynamic(this,&UTagRowElementWidget::OnRemoveChildTagWidget);
	}
	
	OnRemoveTag.AddUniqueDynamic(this,&UTagRowElementWidget::OnRemoveTagEvent);
}

void UTagRowElementWidget::SetData(FGameplayTagTableRow* Iter, UDataTable* TagTable)
{
	TextBlock_Tag->SetText(FText::FromName(Iter->Tag));
	TextBlock_Comment->SetText(FText::FromString(Iter->DevComment));

	CurTagTable = TagTable;
}

void UTagRowElementWidget::OnClickedAddButton()
{
	if(MenuAnchor_AddChildTag)
	{
		MenuAnchor_AddChildTag->Open(true);
	}
}

void UTagRowElementWidget::OnClickedRemoveButton()
{
	if(MenuAnchor_RemoveTag)
	{
		MenuAnchor_RemoveTag->Open(true);
	}
}

UUserWidget* UTagRowElementWidget::OnAddChildTagWidget()
{
	auto widget = CreateWidget<UAddTagWidget>(this,MenuAnchor_AddChildTag->MenuClass);

	widget->CurTagTable = CurTagTable;
	widget->OnUpdateTag = OnUpdateTag;
	
	FString preview = TextBlock_Tag->GetText().ToString() + ".";
	widget->EditableTextBox_Tag->SetText(FText::FromString(preview));
	
	return widget;
}

UUserWidget* UTagRowElementWidget::OnRemoveChildTagWidget()
{
	auto widget = CreateWidget<URemoveTagAlertWidget>(this,MenuAnchor_RemoveTag->MenuClass);
	widget->TextBlock_Tag->SetText(TextBlock_Tag->GetText());
	widget->OnRemoveTag = OnRemoveTag;
	widget->Button_Cancel->OnClicked.AddUniqueDynamic(MenuAnchor_RemoveTag,&UMenuAnchor::Close);
	return widget;
}

void UTagRowElementWidget::OnRemoveTagEvent(const FName& RemoveTag)
{
	if(CurTagTable)
	{
		CurTagTable->RemoveRow(RemoveTag);
		CurTagTable->MarkPackageDirty();
		MenuAnchor_RemoveTag->Close();
	}
}
	
