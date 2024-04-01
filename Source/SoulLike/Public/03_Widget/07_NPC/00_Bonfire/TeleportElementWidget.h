// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "00_Character/04_NPC/Bonfire.h"
#include "Blueprint/UserWidget.h"
#include "TeleportElementWidget.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API UTeleportElementWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta=(BindWidget))
	class UImage* Image_CurLocation;
	UPROPERTY(meta=(BindWidget))
	class UHorizontalBox* HorizontalBox_NotFound;
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* TextBlock_NotFound;
	UPROPERTY(meta=(BindWidget))
	class UButton* Button_Teleport;
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* TextBlock_LocationName;
	UPROPERTY()
	FBonfireInformation TeleportInformation;
	UPROPERTY()
	TWeakObjectPtr<class UBonfireTeleportWidget> ParentsWidget;
	UPROPERTY()
	FButtonStyle OriginalButtonStyle;
	UPROPERTY(EditAnywhere)
	FButtonStyle SelectedButtonStyle;

	UFUNCTION()
	void OnHovered();

public:
	UFUNCTION()
	void OnClicked();

	virtual void NativeConstruct() override;
	virtual void SetIsEnabled(bool bInIsEnabled) override;

	void SetInfo(const FBonfireInformation& Info, class UBonfireTeleportWidget* OwnerWidget);
	void ShowCurLocation();
	void SetSelected();
	void SetUnSelected();

	const FBonfireInformation& GetTeleportInformation() { return TeleportInformation; }
};
