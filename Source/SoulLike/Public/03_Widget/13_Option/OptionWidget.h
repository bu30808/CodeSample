// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "03_Widget/97_PopUp/PopUpBasedWidget.h"
#include "OptionWidget.generated.h"

UENUM()
enum class EGraphicQuality : uint8
{
	Low = 0,
	Medium,
	High,
	Epic,
	Custom
};

/**
 * 
 */
UCLASS()
class SOULLIKE_API UOptionWidget : public UPopUpBasedWidget
{
	GENERATED_BODY()

protected:

	UPROPERTY(meta=(BindWidget))
	class UButton* Button_Close;
	UPROPERTY(meta=(BindWidget))
	class UButton* Button_Quit;

	

	UPROPERTY(meta=(BindWidget))
	class UComboBoxString* ComboBoxString_Resolution;
	UPROPERTY(meta=(BindWidget))
	class UCheckBox* CheckBox_Fullscreen;
	
	UPROPERTY(meta=(BindWidget))
	class UComboBoxString* ComboBoxString_FrameLimit;
	
	UPROPERTY(meta=(BindWidget))
	class UCheckBox* CheckBox_VSync;
	UPROPERTY(meta=(BindWidget))
	class UCheckBox* CheckBox_DynamicResolution;

	
	UPROPERTY(meta=(BindWidget))
	class USlider* Slider_Mouse;
	UPROPERTY(meta=(BindWidget))
	class USlider* Slider_BGM;
	UPROPERTY(meta=(BindWidget))
	class USlider* Slider_SFX;
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* TextBlock_Mouse;
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* TextBlock_BGM;
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* TextBlock_SFX;

	UPROPERTY(meta=(BindWidget))
	class UComboBoxString* ComboBoxString_Graphic;

	UPROPERTY(meta=(BindWidget))
	class UVerticalBox* VerticalBox_CustomOption;

	UPROPERTY(meta=(BindWidget))
	class UComboBoxString* ComboBoxString_AntiQuality;
	
	UPROPERTY(meta=(BindWidget))
	class UComboBoxString* ComboBoxString_Texture;
	UPROPERTY(meta=(BindWidget))
	class UComboBoxString* ComboBoxString_Shadow;
	UPROPERTY(meta=(BindWidget))
	class UComboBoxString* ComboBoxString_PostProcess;
	UPROPERTY(meta=(BindWidget))
	class UComboBoxString* ComboBoxString_ViewDistance;
	UPROPERTY(meta=(BindWidget))
	class UComboBoxString* ComboBoxString_Effect;
	UPROPERTY(meta=(BindWidget))
	class UComboBoxString* ComboBoxString_DynamicGlobalIllumination;
	UPROPERTY(meta=(BindWidget))
	class UComboBoxString* ComboBoxString_Reflection;
	UPROPERTY(meta=(BindWidget))
	class UComboBoxString* ComboBoxString_Foliage;
	UPROPERTY(meta=(BindWidget))
	class UComboBoxString* ComboBoxString_Shading;
	UPROPERTY(meta=(BindWidget))
	class USlider* Slider_TextureMemory;
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* TextBlock_TextureMemory;
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* TextBlock_MaxTextureMemory;





	
	UPROPERTY(Transient)
	TMap<FString,FIntPoint> ResolutionOptions;

	void LoadCurrentOptions();
	void LoadCurVRAMInfo();

	virtual void NativePreConstruct() override;

	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnResolutionChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnFrameLimitChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

	UFUNCTION()
	void OnDynamicResolutionChecked(bool bIsChecked);
	UFUNCTION()
	void OnFullscreenChecked(bool bIsChecked);
	UFUNCTION()
	void OnVSyncChecked(bool bIsChecked);
	UFUNCTION()
	void OnDynamicGlobalIlluminationChecked(bool bIsChecked);

	
	UFUNCTION()
	void OnGraphicOptionChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnClickedQuitButton();
	UFUNCTION()
	void OnClickedCloseButton();
	UFUNCTION()
	void OnMouseSensitivityChanged(float Value);
	UFUNCTION()
	void OnBGMVolumeChanged(float Value);
	UFUNCTION()
	void OnSFXVolumeChanged(float Value);
	UFUNCTION()
	void OnTextureStreamingMemoryChanged(float Value);


	UFUNCTION()
	void OnAntiQualityChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnTextureChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnShadowChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnPostProcessChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnViewDistanceChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnEffectChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

	UFUNCTION()
	void OnDynamicGlobalIlluminationChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnReflectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnFoliageChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnShadingChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
	
	FString FindCurResolutionByValue(const FIntPoint& CurResolution);
	FString AntiOptionNumberToString(const int32& Option);

	class USoulLikeInstance* GetSoullikeInstance() const;
};
