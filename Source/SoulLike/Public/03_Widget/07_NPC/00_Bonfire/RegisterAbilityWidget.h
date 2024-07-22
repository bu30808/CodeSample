// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "02_Ability/AbilityBase.h"
#include "03_Widget/97_PopUp/PopUpBasedWidget.h"
#include "RegisterAbilityWidget.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API URegisterAbilityWidget : public UPopUpBasedWidget
{
	GENERATED_BODY()

	friend class URegisterAbilityComponent;
protected:
	UPROPERTY(meta=(BindWidget))
	class UButton* Button_Close;
	UPROPERTY(meta=(BindWidget))
	class UWrapBox* WrapBox_AbilityList;
	UPROPERTY(meta=(BindWidget))
	class UWrapBox* WrapBox_MemorySlot;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UAbilityQuickSlotButtonWidget> AbilityQuickSlotButtonClass;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UAbilityButtonWidget> AbilityButtonClass;

	void CreateAbilityList();
	void CreateMemorySlot();
	void ReloadRegisterAbility();
	UAbilityQuickSlotButtonWidget* CreateAbilityQuickSlot(const int32 Index);
	UAbilityButtonWidget* CreateAbilityButton(const FAbilityInformation& AbilityInformation);
	UAbilityButtonWidget* GetAbilityButtonFromTag(const FGameplayTag& AbilityTag);

	
	virtual void NativeConstruct() override;
	
	virtual void OnVisibilityChangedEvent(ESlateVisibility InVisibility) override;
	
	void CreateRegisterAbilityList();
	
	UFUNCTION()
	void OnClickedCloseButton();
	UFUNCTION()
	void OnAlreadyRegisteredAbilityDroppedEvent(const FGameplayTag& AbilityTag);
	UFUNCTION()
	void OnDroppedAbilityEvent(const FAbilityInformation& AbilityInformation);
	UFUNCTION()
	void OnClearAbilityQuickSlotEvent(const FGameplayTag& AbilityTag);
};
