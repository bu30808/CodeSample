// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "AddTagWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUpdateTag,class UDataTable*,Table);
/**
 * 
 */
UCLASS()
class SOULLIKEEDITOR_API UAddTagWidget : public UEditorUtilityWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(meta=(BindWidget))
	class UEditableTextBox* EditableTextBox_Tag;
	UPROPERTY(meta=(BindWidget))
	class UEditableTextBox* EditableTextBox_Comment;
	UPROPERTY(meta=(BindWidget))
	class UButton* Button_Add;

	UPROPERTY()
	class UDataTable* CurTagTable;

	UPROPERTY()
	FOnUpdateTag OnUpdateTag;

	virtual void NativeConstruct() override;
	UFUNCTION()
	void OnTextCommittedEvent(const FText& Text, ETextCommit::Type CommitMethod);
	UFUNCTION()
	void OnClickedAdd();
};
