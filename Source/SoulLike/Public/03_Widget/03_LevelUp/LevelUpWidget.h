// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "03_Widget/97_PopUp/PopUpBasedWidget.h"
#include "Blueprint/UserWidget.h"
#include "LevelUpWidget.generated.h"

enum class EAttributePointType : uint8;
enum class EAttributeType : uint8;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnClosed);

/**
 * 
 */
UCLASS()
class SOULLIKE_API ULevelUpWidget : public UPopUpBasedWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta=(BindWidget))
	class UButton* Button_Close;

protected:
	UPROPERTY(meta=(BindWidget))
	class ULevelUpPreviewWidget* UMG_LevelUpPreview;
	UPROPERTY(meta=(BindWidget))
	class UButton* Button_OK;
	UPROPERTY(meta=(BindWidget))
	class UButton* Button_Cancel;


	UPROPERTY(meta=(BindWidget), BlueprintReadOnly)
	class UImage* Image_Level;
	UPROPERTY(meta=(BindWidget), BlueprintReadOnly)
	class UImage* Image_Exp;
	UPROPERTY(meta=(BindWidget), BlueprintReadOnly)
	class UImage* Image_NextExp;
	UPROPERTY(meta=(BindWidget), BlueprintReadOnly)
	class UImage* Image_MaxHP;
	UPROPERTY(meta=(BindWidget), BlueprintReadOnly)
	class UImage* Image_MaxSP;
	UPROPERTY(meta=(BindWidget), BlueprintReadOnly)
	class UImage* Image_MaxMP;
	UPROPERTY(meta=(BindWidget), BlueprintReadOnly)
	class UImage* Image_Str;
	UPROPERTY(meta=(BindWidget), BlueprintReadOnly)
	class UImage* Image_Dex;
	UPROPERTY(meta=(BindWidget), BlueprintReadOnly)
	class UImage* Image_Int;
	UPROPERTY(meta=(BindWidget), BlueprintReadOnly)
	class UImage* Image_Will;


	UPROPERTY(EditAnywhere)
	TSubclassOf<class USimpleToolTipWidget> SimpleToolTipWidgetObject;

	UPROPERTY()
	TMap<EAttributePointType, class UAttributePreviewWidget*> AttributePreviewWidgets;

	UPROPERTY()
	TWeakObjectPtr<class ULevelUpSubsystem> LevelUpSubsystem;


	//몇 레벨이나 올릴 것인가 저장하는 변수입니다. 능력치 상승 버튼을 누를때마다 경험치량을 계산해서 1씩 증가시킵니다.
	UPROPERTY(Transient)
	int32 Level_WantToUP = 0;


	void SetUp();

	virtual void NativeConstruct() override;
	virtual void NativePreConstruct() override;

	void Reset();
	bool CanLevelUp() const;


	UFUNCTION()
	void Init();

	UFUNCTION()
	void OnClickedCloseButton();
	UFUNCTION()
	void OnClickedCancelButton();
	UFUNCTION()
	void OnClickedOKButton();

	//증가하는 스텟 수치 미리보기
	//즉 힘 12 -> 13 이런식으로 표시하는 위젯에 스텟 증가 버튼을 눌렀을 떄 호출되는 이벤트입니다.
	UFUNCTION()
	void OnClickedAttributeAddButton();

	void HiddenAddButton();

	class USimpleToolTipWidget* CreateSimpleToolTipWidget(FString ToolTipTextToSet);

public:
	//이 위젯이 닫길떄 호출됩니다.
	UPROPERTY(BlueprintAssignable)
	FOnClosed OnClosed;
};
