// Fill out your copyright notice in the Description page of Project Settings.


#include "03_Widget/11_Tutorial/TutorialWidget.h"

#include "96_Library/WidgetHelperLibrary.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Logging/StructuredLog.h"

void UTutorialWidget::OnVisibilityChangedEvent(ESlateVisibility InVisibility)
{
	Super::OnVisibilityChangedEvent(InVisibility);

	if (!IsVisible())
	{
		UGameplayStatics::SetGamePaused(this,false);
		if (TutorialObjectPtr != nullptr)
		{
			TutorialObjectPtr->Destroy();
			TutorialObjectPtr = nullptr;
		}

		OnClosedTutorialWidget.ExecuteIfBound();
	}
}

void UTutorialWidget::OnClickedOKButton()
{
	SetVisibility(ESlateVisibility::Collapsed);
}

void UTutorialWidget::NativeConstruct()
{
	bUseAsPopUp = true;

	Super::NativeConstruct();

	Button_OK->OnClicked.AddUniqueDynamic(this,&UTutorialWidget::OnClickedOKButton);
	
	OnVisibilityChanged.AddUniqueDynamic(this, &UTutorialWidget::OnVisibilityChangedEvent);
	UGameplayStatics::SetGamePaused(this,true);
}

void UTutorialWidget::SetTutorial(FGameplayTag Tag, AActor* TutorialObject)
{
	TutorialObjectPtr = TutorialObject;

	if (auto info = TutorialDataTable->FindRow<FTutorialInformation>(Tag.GetTagName(), ""))
	{
		Image_Tuto->SetBrushFromSoftTexture(info->TutorialImage, true);
		TextBlock_Msg->SetText(info->TutorialMsg);
	}
	else
	{
		UE_LOGFMT(LogTemp, Error, "튜토리얼 데이터를 찾을 수 없습니다!!!");
	}
}
