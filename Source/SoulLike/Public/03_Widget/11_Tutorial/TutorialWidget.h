// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "03_Widget/97_PopUp/PopUpBasedWidget.h"
#include "Blueprint/UserWidget.h"
#include "Engine/DataTable.h"
#include "TutorialWidget.generated.h"

USTRUCT(BlueprintType)
struct FTutorialInformation : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<class UTexture2D> TutorialImage;
	UPROPERTY(EditAnywhere, meta=(MultiLine))
	FText TutorialMsg;
};

/**
 * 
 */
UCLASS()
class SOULLIKE_API UTutorialWidget : public UPopUpBasedWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere)
	class UDataTable* TutorialDataTable;

	UPROPERTY(meta=(BindWidget), BlueprintReadOnly)
	class UImage* Image_Tuto;
	UPROPERTY(meta=(BindWidget), BlueprintReadOnly)
	class UTextBlock* TextBlock_Msg;

	UPROPERTY(BlueprintReadWrite)
	AActor* TutorialObjectPtr;

public:
	virtual void OnVisibilityChangedEvent(ESlateVisibility InVisibility) override;

	virtual void NativeConstruct() override;

	void SetTutorial(FGameplayTag Tag, class AActor* TutorialObject);
};
