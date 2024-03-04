// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Blueprint/UserWidget.h"
#include "DebugElementWidget.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API UDebugElementWidget : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

public:
	UPROPERTY(meta=(BindWidget))
	class UImage* Image_Ability;
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* TextBlock_Tag;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class USimpleToolTipWidget> ToolTipWidgetObject;

	virtual void NativeConstruct() override;
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
};

UCLASS()
class SOULLIKE_API UDebugIgnoreMoveInputWidget : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
public:
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* TextBlock_Tag;
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* TextBlock_Who;
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* TextBlock_Count;
	
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
};