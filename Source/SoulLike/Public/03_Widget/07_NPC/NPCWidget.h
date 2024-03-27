// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "03_Widget/97_PopUp/PopUpBasedWidget.h"
#include "Blueprint/UserWidget.h"
#include "NPCWidget.generated.h"

enum class ENPCActionType : uint8;


/**
 * 
 */
UCLASS()
class SOULLIKE_API UNPCWidget : public UPopUpBasedWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* TextBlock_NPCName;
	UPROPERTY(meta=(BindWidget))
	class URichTextBlock* RichTextBlock_Conv;
	UPROPERTY(meta=(BindWidget))
	class UScrollBox* ScrollBox_ActionList;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UNPCActionButtonWidget> NPCActionButtonObject;
	UPROPERTY()
	TMap<ENPCActionType, class UNPCActionButtonWidget*> NPCActionButtons;

	UPROPERTY()
	TWeakObjectPtr<class ANPCBase> NPCOwner;
	UPROPERTY()
	class UNPCActionButtonWidget* ExitButton;

	FText NPCActionTypeToText(ENPCActionType ActionType);

	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;


public:
	void CreateActionList();
	void SetNPC(class ANPCBase* NPC);
	void SetNPCName(const FName& NewName);
	void BindAction();
};
