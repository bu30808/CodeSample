// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "03_Widget/97_PopUp/PopUpBasedWidget.h"
#include "Blueprint/UserWidget.h"
#include "AlertWidget.generated.h"


UENUM(BlueprintType)
enum class EAlertMsgType :uint8
{
	Normal,
	Warning,
	Error,
	NoUseDeco
};

DECLARE_DYNAMIC_DELEGATE(FOnButtonClicked);

/**
 * 
 */
UCLASS()
class SOULLIKE_API UAlertWidget : public UPopUpBasedWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta=(BindWidget))
	class URichTextBlock* RichTextBlock_Msg;

	UPROPERTY(meta=(BindWidget))
	class UButton* Button_Ok;

	UPROPERTY(EditAnywhere)
	TMap<EAlertMsgType, FLinearColor> Colors;

	FOnButtonClicked OnClicked;

	virtual void NativeConstruct() override;
	UFUNCTION()
	void OnClickedOKButton();

public:
	void SetAlertMsg(EAlertMsgType AlertMsgType, FText Msg, const FOnButtonClicked& OnClickedEvent);
};
