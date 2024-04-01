// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "00_Character/04_NPC/Bonfire.h"
#include "03_Widget/97_PopUp/PopUpBasedWidget.h"
#include "Blueprint/UserWidget.h"
#include "BonfireTeleportWidget.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API UBonfireTeleportWidget : public UPopUpBasedWidget
{
	GENERATED_BODY()

	friend class UBonfireComponent;

protected:
	UPROPERTY(meta=(BindWidget))
	class UImage* Image_Location;

	UPROPERTY(meta=(BindWidget))
	class UScrollBox* ScrollBox_TeleportList;

	UPROPERTY(meta=(BindWidget))
	class UButton* Button_OK;
	UPROPERTY(meta=(BindWidget))
	class UButton* Button_Cancel;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UTeleportElementWidget> TeleportElementWidgetObject;
	UPROPERTY()
	TWeakObjectPtr<class UTeleportElementWidget> SelectedElement;

	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnClickedOKEvent();
	UFUNCTION()
	void OnClickedCancelEvent();

	void CreateList(class UBonfireComponent* BonfireComponent);

	//해당 포인트가 활성화 되어있는 포인트인지 확인합니다.
	bool IsActivated(const FString& LevelName, const FString& SafeName);

public:
	void SetPreviewImage(const TSoftObjectPtr<UTexture2D>& LocationImage);
	void SetSelectedInformation(class UTeleportElementWidget* TeleportElementWidget);
};
