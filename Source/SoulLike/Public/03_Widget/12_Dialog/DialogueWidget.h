// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DialogueReplyWidget.h"
#include "DialogueUserWidget.h"
#include "DialogueWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDialogueClose,class UDialogue*,Dialogue,AActor*,NPC);

/**
 * 
 */
UCLASS()
class SOULLIKE_API UDialogueWidget : public UDialogueUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta=(BindWidget),BlueprintReadWrite)
	class URichTextBlock* NPCText;
	UPROPERTY(meta=(BindWidget),BlueprintReadWrite)
	class UVerticalBox* PlayerVBox;
	UPROPERTY(meta=(BindWidget),BlueprintReadWrite)
	class UTextBlock* SpeakerName;

	UPROPERTY(meta=(BindWidget),BlueprintReadWrite)
	class UOverlay* NextButtonContainer;
	UPROPERTY(meta=(BindWidget),BlueprintReadWrite)
	class UButton* NextButton;
	UPROPERTY(meta=(BindWidget),BlueprintReadWrite)
	class UTextBlock* NextText;
	
	UPROPERTY()
	class UDialogue* InDialogue;
	UPROPERTY(Transient)
	int32 CurrentlySelectedReply;

	UPROPERTY()
	class UAudioComponent* Sound2DComponent;


	UPROPERTY(BlueprintAssignable)
	FOnDialogueClose OnDialogueClose;
	
	UPROPERTY(Transient)
	FDialogueNode CurrentPlayerNode;
	UPROPERTY(Transient)
	FDialogueNode CurNPCNode;
	
	FTimerHandle ToNPCReplyTimerHandle;


	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

	UFUNCTION()
	FSlateColor GetContinueColorAndOpacity();
	UFUNCTION()
	void OnClickedNextButton();


public:
	UFUNCTION(BlueprintCallable)
	void SetDialogue(class UDialogue* NewDialogue){ InDialogue = NewDialogue;}

	void MouseSelectReply(const class UDialogueReplyWidget* DialogueReplyWidget);
	void PlayPlayerReply(const FDialogueNode& PlayerNode);

protected:


	UFUNCTION()
	void ToNPCReply(const FDialogueNode& PlayerReply,bool bFireEvents);
	
	void LaunchEvents(const FDialogueNode& Node);
	bool CheckConditions(const FDialogueNode& Node);
	void DisplayNPCNode(const FDialogueNode& NPCNode);
	void DisplayReplies(const TArray<FDialogueNode>& Replies);
	void HightlightSelectedReply();
	
	void PlayDialogueSound(class USoundBase* Sound);
	void StopDialogueSound();
	
	void ShowContinueButtonWithText(const FText& Text);
	
	class UDialogueReplyWidget* CreateReplyWidget(const FDialogueNode& Node);

	UFUNCTION(BlueprintCallable)
	void JumpToNPCNode(const int32& NodeID);

	/*게임패드*/

	UPROPERTY(Transient)
	bool bIsSelectedReplyKeyDown;


	void SimulateNextClicked();
	void SimulateNextHoverd();
	void GamepadReply(const int32& Index);

	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	virtual FReply NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
};
