// Fill out your copyright notice in the Description page of Project Settings.


#include "00_EditorUtilityWidget/03_AbilityTagHandler/AddTagWidget.h"

#include "GameplayTagsManager.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Misc/MessageDialog.h"

void UAddTagWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if(EditableTextBox_Tag)
	{
		EditableTextBox_Tag->OnTextCommitted.AddUniqueDynamic(this,&UAddTagWidget::OnTextCommittedEvent);
	}

	if(EditableTextBox_Comment)
	{
		EditableTextBox_Comment->OnTextCommitted.AddUniqueDynamic(this,&UAddTagWidget::OnTextCommittedEvent);
	}

	if(Button_Add)
	{
		Button_Add->OnClicked.AddUniqueDynamic(this,&UAddTagWidget::OnClickedAdd);
	}
}

void UAddTagWidget::OnTextCommittedEvent(const FText& Text, ETextCommit::Type CommitMethod)
{
	if(CommitMethod == ETextCommit::OnEnter)
	{
		OnClickedAdd();
	}
}

void UAddTagWidget::OnClickedAdd()
{
	if(CurTagTable)
	{
		FString createTag = EditableTextBox_Tag->GetText().ToString();
		if(CurTagTable->FindRow<FGameplayTagTableRow>(FName(createTag),"")==nullptr)
		{
			FGameplayTagTableRow newData;
			newData.Tag = FName(createTag);
			newData.DevComment = EditableTextBox_Comment->GetText().ToString();
			
			CurTagTable->AddRow(FName(createTag),newData);
			if(!CurTagTable->MarkPackageDirty())
			{
				FMessageDialog::Open(EAppMsgType::Ok,FText::FromString(TEXT("정보 반영에 실패했습니다.")));
			}
			OnUpdateTag.Broadcast(CurTagTable);
			RemoveFromParent();
		}else
		{
			FMessageDialog::Open(EAppMsgType::Ok,FText::FromString(TEXT("이미 존재하는 태그입니다.")));
		}
		
	}else
	{
		FMessageDialog::Open(EAppMsgType::Ok,FText::FromString(TEXT("데이터 테이블이 유효하지 않습니다.")));
	}
}
