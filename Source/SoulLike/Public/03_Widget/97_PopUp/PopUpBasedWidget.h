// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PopUpBasedWidget.generated.h"

/**
 * 팝업을 위한 위젯입니다.
 * bRemovable 참이면, 팝업이 제거될때 RemoveFromParents가 호출됩니다.
 */
UCLASS()
class SOULLIKE_API UPopUpBasedWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere)
	bool bUseAsPopUp = false;

	//이 값이 참이면 RemoveFromParents 합니다.
	UPROPERTY(EditAnywhere)
	bool bRemovable = false;

	virtual void NativeConstruct() override;

	UFUNCTION()
	virtual void OnVisibilityChangedEvent(ESlateVisibility InVisibility);
public:
	UFUNCTION()
	void SetFocusOnThisWidget();
};
