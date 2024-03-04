// Fill out your copyright notice in the Description page of Project Settings.


#include "00_EditorUtilityWidget/03_AbilityTagHandler/RemoveTagAlertWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

void URemoveTagAlertWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if(Button_OK)
	{
		Button_OK->OnClicked.AddUniqueDynamic(this,&URemoveTagAlertWidget::OnClickedOKEvent);
	}
	
}

void URemoveTagAlertWidget::OnClickedOKEvent()
{
	OnRemoveTag.Broadcast(FName(TextBlock_Tag->GetText().ToString()));
}

