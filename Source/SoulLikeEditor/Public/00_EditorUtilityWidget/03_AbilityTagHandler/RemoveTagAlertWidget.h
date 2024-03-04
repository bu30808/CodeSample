// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "RemoveTagAlertWidget.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRemoveTag,const FName&,Tag);
/**
 * 
 */
UCLASS()
class SOULLIKEEDITOR_API URemoveTagAlertWidget : public UEditorUtilityWidget
{
	GENERATED_BODY()
public:

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* TextBlock_Tag;
	UPROPERTY(meta=(BindWidget))
	class UButton* Button_OK;
	UPROPERTY(meta=(BindWidget))
	class UButton* Button_Cancel;
	
	UPROPERTY()
	FOnRemoveTag OnRemoveTag;

	virtual void NativeConstruct() override;
	UFUNCTION()
	void OnClickedOKEvent();

};
