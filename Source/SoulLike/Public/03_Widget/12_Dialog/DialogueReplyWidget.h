// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Dialogue.h"
#include "Blueprint/UserWidget.h"
#include "DialogueReplyWidget.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API UDialogueReplyWidget : public UUserWidget
{
	GENERATED_BODY()

protected:

	UPROPERTY(meta=(BindWidget))
	class URichTextBlock* ReplyTextBlock;
	UPROPERTY(meta=(BindWidget))
	class UButton* Button_Reply;
	UPROPERTY(meta=(BindWidget),BlueprintReadWrite)
	class UImage* SelectedReplyBackground;
	
	UPROPERTY(Transient)
	FDialogueNode ReplyNode;
	UPROPERTY(Transient)
	bool bSelected = false;
	UPROPERTY()
	TWeakObjectPtr<class UDialogueWidget> OwnerWidget;

	UFUNCTION()
	ESlateVisibility GetSelectedReplyBackgroundVisibility();
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

public:
	void SetOwnerWidget(class UDialogueWidget* NewOwner);
	void SetReplyNode(const FDialogueNode& NewNode){ReplyNode = NewNode;}
	void SetSelected(bool bCond){bSelected=bCond;}

protected:
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	
	UFUNCTION()
	void OnClickedReplyButton();

	/*게임패드*/
	UFUNCTION()
	void SimulateClicked();
};
