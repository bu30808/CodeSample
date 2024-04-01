// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "00_Character/01_Component/InventoryComponent.h"
#include "Blueprint/UserWidget.h"
#include "CharacterStatusWidget.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API UCharacterStatusWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UCharacterInfoElementWidget> InfoElementWidgetObject;

	UPROPERTY(meta=(BindWidget))
	class UVerticalBox* VerticalBox_CharacterElement;


	UPROPERTY()
	TArray<class UCharacterInfoElementWidget*> Elements;

	virtual void NativePreConstruct() override;

	virtual void NativeConstruct() override;

	//아이템 사용 후 호출됩니다.
	UFUNCTION()
	void OnUseItemEvent(class ABaseCharacter* UsedBy, const FInventoryItem& ItemInfo);
	UFUNCTION()
	void OnCharacterInformationUpdateEvent();
};
