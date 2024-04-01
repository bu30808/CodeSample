// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AlertWidget.h"
#include "03_Widget/97_PopUp/PopUpBasedWidget.h"
#include "OKCancelWidget.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API UOKCancelWidget : public UPopUpBasedWidget
{
	GENERATED_BODY()
protected:
	UPROPERTY(meta=(BindWidget))
	class URichTextBlock* RichTextBlock_Msg;

	UPROPERTY(meta=(BindWidget))
	class UButton* Button_Ok;

	UPROPERTY(meta=(BindWidget))
	class UButton* Button_Cancel;
	
	UPROPERTY(EditAnywhere)
	TMap<EAlertMsgType, FLinearColor> Colors;

	FOnButtonClicked OnClickedOK;
	FOnButtonClicked OnClickedCancel;

	virtual void NativeConstruct() override;
	UFUNCTION()
	void OnClickedOKButton();
	UFUNCTION()
	void OnClickedCancelButton();
public:
	void SetAlertMsg(EAlertMsgType AlertMsgType, FText Msg, const FOnButtonClicked& OnOkClickedEvent,const FOnButtonClicked& OnCancelClickedEvent);
};
