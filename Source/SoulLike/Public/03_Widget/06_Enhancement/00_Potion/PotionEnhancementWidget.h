// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "00_Character/04_NPC/99_Component/PotionEnhancementComponent.h"
#include "03_Widget/97_PopUp/PopUpBasedWidget.h"
#include "Blueprint/UserWidget.h"
#include "PotionEnhancementWidget.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API UPotionEnhancementWidget : public UPopUpBasedWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta=(BindWidget))
	class UWidgetSwitcher* WidgetSwitcher;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* TextBlock_CurEnhancementHP;
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* TextBlock_NextEnhancementHP;
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* TextBlock_ButtonTextHP;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* TextBlock_CurEnhancementMP;
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* TextBlock_NextEnhancementMP;
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* TextBlock_ButtonTextMP;

public:
	UPROPERTY(meta=(BindWidget))
	class UButton* Button_HP;
	UPROPERTY(meta=(BindWidget))
	class UButton* Button_MP;


	UPROPERTY(meta=(BindWidget))
	class UButton* Button_Close;
	UPROPERTY(meta=(BindWidget))
	class UButton* Button_UpgradeHP;
	UPROPERTY(meta=(BindWidget))
	class UButton* Button_UpgradeMP;

protected:
	UPROPERTY()
	TWeakObjectPtr<class UPotionEnhancementComponent> PotionEnhancementComponent;

public:

	virtual void OnVisibilityChangedEvent(ESlateVisibility InVisibility) override;

	UFUNCTION()
	void OnClickedUpgradeHPButton();
	UFUNCTION()
	void OnClickedUpgradeMPButton();

	UFUNCTION()
	void OnClickedHPButton();
	UFUNCTION()
	void OnClickedMPButton();
	UFUNCTION()
	void OnClickedCloseButton();

	virtual void NativeConstruct() override;
	void SetPotionEnhancementComponent(class UPotionEnhancementComponent* Component);

	void Init();
};
