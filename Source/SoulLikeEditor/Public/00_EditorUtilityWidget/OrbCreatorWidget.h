// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "95_OrbCreator/OrbMatrix.h"
#include "OrbCreatorWidget.generated.h"

enum class EOrbMatrixSlotType : uint8;
enum class EOrbMatrixSlotLineDirection : uint8;


/**
 * 
 */
UCLASS()
class SOULLIKEEDITOR_API UOrbCreatorWidget : public UEditorUtilityWidget
{
	GENERATED_BODY()

public:
	/*UPROPERTY(meta=(BindWidget))
	class UButton* Button_Load;
	UPROPERTY(meta=(BindWidget))
	class UButton* Button_New;*/
	UPROPERTY(meta=(BindWidget))
	class UComboBoxString* ComboBoxString_OrbMatrixList;

	UPROPERTY(meta=(BindWidget))
	class UUniformGridPanel* UniformGridPanel_Orb;
	UPROPERTY(meta=(BindWidget))
	class UEditableTextBox* EditableTextBox_Matrix;
	UPROPERTY(meta=(BindWidget))
	class UButton* Button_Generate;

	UPROPERTY(meta=(BindWidget))
	class UHorizontalBox* HorizontalBox_SlotType;
	UPROPERTY(meta=(BindWidget))
	class UCheckBox* CheckBox_Physical;
	UPROPERTY(meta=(BindWidget))
	class UCheckBox* CheckBox_Defence;
	UPROPERTY(meta=(BindWidget))
	class UCheckBox* CheckBox_Magical;
	UPROPERTY(meta=(BindWidget))
	class UCheckBox* CheckBox_Free;
	UPROPERTY(meta=(BindWidget))
	class UCheckBox* CheckBox_Line;
	UPROPERTY(meta=(BindWidget))
	class UCheckBox* CheckBox_Empty;
	UPROPERTY(meta=(BindWidget))
	class UCheckBox* CheckBox_Lock;

	UPROPERTY()
	TArray<UCheckBox*> CheckBoxes;


	UPROPERTY(meta=(BindWidget))
	class UHorizontalBox* HorizontalBox_Line;
	UPROPERTY(meta=(BindWidget))
	class UHorizontalBox* HorizontalBox_Lock;
	UPROPERTY(meta=(BindWidget))
	class UButton* Button_LeftDownToRightUp;
	UPROPERTY(meta=(BindWidget))
	class UButton* Button_LeftUpToRightDown;
	UPROPERTY(meta=(BindWidget))
	class UButton* Button_LeftToRight;
	UPROPERTY(meta=(BindWidget))
	class UButton* Button_UpToDown;

	UPROPERTY(meta=(BindWidget))
	class UButton* Button_Save;
	UPROPERTY(meta=(BindWidget))
	class UButton* Button_Clear;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UOrbMatrixButtonWidget> OrbMatrixButtonObject;

	//배열 크기
	UPROPERTY()
	int32 Matrix;
	UPROPERTY()
	TWeakObjectPtr<class UOrbMatrixButtonWidget> CurClickedButton;
	UPROPERTY()
	TMap<FString, class UOrbMatrix*> OrbSlotLineObjects;
	UPROPERTY()
	TWeakObjectPtr<class UOrbMatrix> CurSelectedObject;

	TArray<TArray<class UOrbMatrixButtonWidget*>> ButtonArray;


	UFUNCTION()
	void OnTextChangedEvent(const FText& Text);

	//매트릭스의 버튼을 누르면 할 행동을 정의합니다.
	UFUNCTION()
	void OnClickedOrbMatrixButton();

	UFUNCTION()
	void OnClickedButtonGenerateEvent();
	UFUNCTION()
	void OnClickedButtonClearEvent();

	UFUNCTION()
	void OnCheckPhysical(bool bIsChecked);
	UFUNCTION()
	void OnCheckDefence(bool bIsChecked);
	UFUNCTION()
	void OnCheckMagical(bool bIsChecked);
	UFUNCTION()
	void OnCheckFree(bool bIsChecked);
	UFUNCTION()
	void OnCheckLine(bool bIsChecked);
	UFUNCTION()
	void OnCheckEmpty(bool bIsChecked);
	UFUNCTION()
	void OnCheckLock(bool bIsChecked);

	void SetUnCheckOthers(class UCheckBox* One);

	UFUNCTION()
	void OnClickedLeftToRight();
	UFUNCTION()
	void OnClickedUpToDown();
	UFUNCTION()
	void OnClickedLeftDownToRightUp();
	UFUNCTION()
	void OnClickedLeftUpToRightDown();


	UFUNCTION()
	void OnOrbLineListSelectedChangedEvent(FString SelectedItem, ESelectInfo::Type SelectionType);

	void SetSelectedObjectInfo(UOrbMatrix* Value);
	void CreateGrid();

	//DFS로 슬롯 타입을 순서대로 저장합니다.
	void GetLine(int32 i, int32 j, TArray<FOrbMatrixElementInfo>& LineArray, const EOrbMatrixSlotType& SlotType);

	UFUNCTION()
	void OnClickedSaveButton();

	UFUNCTION()
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

	void SetButtonInfo(UOrbMatrixButtonWidget* Button);
	TArray<UObject*> LoadOrbMatrix();
};
