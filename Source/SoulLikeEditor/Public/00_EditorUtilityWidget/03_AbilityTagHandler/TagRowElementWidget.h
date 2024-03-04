// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AddTagWidget.h"
#include "EditorUtilityWidget.h"
#include "GameplayTagsManager.h"
#include "RemoveTagAlertWidget.h"
#include "TagRowElementWidget.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKEEDITOR_API UTagRowElementWidget : public UEditorUtilityWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* TextBlock_Tag;
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* TextBlock_Comment;

	UPROPERTY(meta=(BindWidget))
	class UButton* Button_AddChildTag;
	UPROPERTY(meta=(BindWidget))
	class UButton* Button_RemoveTag;

	UPROPERTY(meta=(BindWidget))
	class UMenuAnchor* MenuAnchor_AddChildTag;
	UPROPERTY(meta=(BindWidget))
	class UMenuAnchor* MenuAnchor_RemoveTag;

	UPROPERTY()
	class UDataTable* CurTagTable;
	UPROPERTY()
	FOnUpdateTag OnUpdateTag;
	UPROPERTY()
	FOnRemoveTag OnRemoveTag;

	virtual void NativeConstruct() override;
	void SetData(FGameplayTagTableRow* Iter, UDataTable* TagTable);

	UFUNCTION()
	void OnClickedAddButton();
	UFUNCTION()
	void OnClickedRemoveButton();
	
	UFUNCTION()
	UUserWidget* OnAddChildTagWidget();
	UFUNCTION()
	UUserWidget* OnRemoveChildTagWidget();

	UFUNCTION()
	void OnRemoveTagEvent(const FName& RemoveTag);
};
