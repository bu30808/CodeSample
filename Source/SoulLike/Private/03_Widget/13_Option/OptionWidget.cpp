// Fill out your copyright notice in the Description page of Project Settings.


#include "03_Widget/13_Option/OptionWidget.h"

#include "96_Library/UserSettingLibrary.h"
#include "96_Library/WidgetHelperLibrary.h"
#include "98_GameInstance/SoulLikeInstance.h"
#include "Components/Button.h"
#include "Components/CheckBox.h"
#include "Components/ComboBoxString.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "GameFramework/GameUserSettings.h"

void UOptionWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	if(ComboBoxString_Resolution)
	{
		ComboBoxString_Resolution->ClearOptions();
		ComboBoxString_Resolution->AddOption("WindowedFullscreen");
		ResolutionOptions = UUserSettingLibrary::GetResolutionOptions();
		for(const auto& iter : ResolutionOptions)
		{
			ComboBoxString_Resolution->AddOption(iter.Key);
		}
	}
	
	LoadCurVRAMInfo();
}

void UOptionWidget::LoadCurrentOptions()
{
	auto setting = UGameUserSettings::GetGameUserSettings();
	setting->ApplySettings(false);
	
	ComboBoxString_Resolution->SetSelectedOption(FindCurResolutionByValue(setting->GetScreenResolution()));
	CheckBox_Fullscreen->SetIsChecked(setting->GetFullscreenMode() == EWindowMode::Fullscreen);

	auto scaleLevel = setting->GetOverallScalabilityLevel();
	if(scaleLevel!=-1)
	{
		ComboBoxString_Graphic->SetSelectedIndex(scaleLevel);
	}else
	{
		ComboBoxString_Graphic->SetSelectedIndex(static_cast<int32>(EGraphicQuality::Custom));
	}
	
	ComboBoxString_Effect->SetSelectedIndex(setting->GetVisualEffectQuality());
	ComboBoxString_Shadow->SetSelectedIndex(setting->GetShadowQuality());
	ComboBoxString_Texture->SetSelectedIndex(setting->GetTextureQuality());
	ComboBoxString_PostProcess->SetSelectedIndex(setting->GetPostProcessingQuality());
	ComboBoxString_ViewDistance->SetSelectedIndex(setting->GetViewDistanceQuality());

	ComboBoxString_DynamicGlobalIllumination->SetSelectedIndex(setting->GetGlobalIlluminationQuality());
	ComboBoxString_AntiQuality->SetSelectedIndex(setting->GetAntiAliasingQuality());
	ComboBoxString_DynamicGlobalIllumination->SetSelectedIndex(setting->GetGlobalIlluminationQuality());
	ComboBoxString_Reflection->SetSelectedIndex(setting->GetReflectionQuality());
	ComboBoxString_Foliage->SetSelectedIndex(setting->GetFoliageQuality());
	ComboBoxString_Shading->SetSelectedIndex(setting->GetShadingQuality());
	
	
	CheckBox_VSync->SetIsChecked(setting->IsVSyncEnabled());
	CheckBox_DynamicResolution->SetIsChecked(setting->IsDynamicResolutionEnabled());
	
	
	
	const auto& curResolution = UGameUserSettings::GetGameUserSettings()->GetScreenResolution();
	ComboBoxString_Resolution->SetSelectedOption(FindCurResolutionByValue(curResolution));
	ComboBoxString_FrameLimit->SetSelectedOption(UUserSettingLibrary::GetFrameRateLimitToString());
	if(auto instance = GetGameInstance<USoulLikeInstance>())
	{
		if(UWidgetHelperLibrary::IsUseSaveGameMode(this))
		{
			bool bSucc = false;
			const auto& optionSave = instance->LoadGameOption(bSucc);
			if(bSucc)
			{
				Slider_Mouse->SetValue(optionSave.MouseSensitivity);
				Slider_BGM->SetValue(optionSave.BGMVolume);
				Slider_SFX->SetValue(optionSave.SFXVolume);
				
				Slider_TextureMemory->SetValue(optionSave.TexturePoolSize);
				TextBlock_TextureMemory->SetText(FText::AsNumber(optionSave.TexturePoolSize));
				
			}
		
		}else{
			Slider_Mouse->SetValue(0.5f/*GetOwningPlayer<AUserController>()->GetMouseSensitivity()*/);
			Slider_BGM->SetValue(instance->GetBGMVolume());
			Slider_SFX->SetValue(instance->GetSFXVolume());
		}
	}
	
	CheckBox_VSync->SetIsChecked(UUserSettingLibrary::IsVSyncEnabled());


	ComboBoxString_AntiQuality->SetSelectedIndex(UUserSettingLibrary::GetAntiAliasingQuality());
	ComboBoxString_Shadow->SetSelectedIndex(UUserSettingLibrary::GetShadowQuality());
	ComboBoxString_PostProcess->SetSelectedIndex(UUserSettingLibrary::GetPostProcessingQuality());
	ComboBoxString_Effect->SetSelectedIndex(UUserSettingLibrary::GetEffectsQuality());
	ComboBoxString_Texture->SetSelectedIndex(UUserSettingLibrary::GetTextureQuality());
	
}

void UOptionWidget::LoadCurVRAMInfo()
{
	const auto& info = UUserSettingLibrary::GetVRAMInfo();
	int32 curPoolSize = info.TexturePoolSize/1024/1024;
	//int32 maxMemory = info.TotalGraphicsMemory/1024/1024;

	Slider_TextureMemory->SetMaxValue(3000);
	Slider_TextureMemory->SetMinValue(400);
	Slider_TextureMemory->SetStepSize(100);
	
	Slider_TextureMemory->SetValue(curPoolSize);
	TextBlock_TextureMemory->SetText(FText::AsNumber(curPoolSize));
	TextBlock_MaxTextureMemory->SetText(FText::AsNumber(3000));
}

void UOptionWidget::NativeConstruct()
{
	bUseAsPopUp = true;
	bRemovable = false;

	Super::NativeConstruct();
	VerticalBox_CustomOption->SetVisibility(ESlateVisibility::Hidden);
		
	ComboBoxString_Resolution->OnSelectionChanged.AddUniqueDynamic(this,&UOptionWidget::OnResolutionChanged);
	ComboBoxString_FrameLimit->OnSelectionChanged.AddUniqueDynamic(this,&UOptionWidget::OnFrameLimitChanged);
	
	CheckBox_Fullscreen->OnCheckStateChanged.AddUniqueDynamic(this,&UOptionWidget::OnFullscreenChecked);
	CheckBox_VSync->OnCheckStateChanged.AddUniqueDynamic(this,&UOptionWidget::OnVSyncChecked);
	CheckBox_DynamicResolution->OnCheckStateChanged.AddUniqueDynamic(this,&UOptionWidget::OnDynamicResolutionChecked);
	
	ComboBoxString_Graphic->OnSelectionChanged.AddUniqueDynamic(this,&UOptionWidget::OnGraphicOptionChanged);
	
	ComboBoxString_AntiQuality->OnSelectionChanged.AddUniqueDynamic(this,&UOptionWidget::OnAntiQualityChanged);
	ComboBoxString_Texture->OnSelectionChanged.AddUniqueDynamic(this,&UOptionWidget::OnTextureChanged);
	ComboBoxString_Shadow->OnSelectionChanged.AddUniqueDynamic(this,&UOptionWidget::OnShadowChanged);
	ComboBoxString_PostProcess->OnSelectionChanged.AddUniqueDynamic(this,&UOptionWidget::OnPostProcessChanged);
	ComboBoxString_ViewDistance->OnSelectionChanged.AddUniqueDynamic(this,&UOptionWidget::OnViewDistanceChanged);
	ComboBoxString_Effect->OnSelectionChanged.AddUniqueDynamic(this,&UOptionWidget::OnEffectChanged);
	ComboBoxString_DynamicGlobalIllumination->OnSelectionChanged.AddUniqueDynamic(this,&UOptionWidget::OnDynamicGlobalIlluminationChanged);
	ComboBoxString_Reflection->OnSelectionChanged.AddUniqueDynamic(this,&UOptionWidget::OnReflectionChanged);
	ComboBoxString_Foliage->OnSelectionChanged.AddUniqueDynamic(this,&UOptionWidget::OnFoliageChanged);
	ComboBoxString_Shading->OnSelectionChanged.AddUniqueDynamic(this,&UOptionWidget::OnShadingChanged);

	LoadCurVRAMInfo();
	Slider_TextureMemory->OnValueChanged.AddUniqueDynamic(this,&UOptionWidget::OnTextureStreamingMemoryChanged);

	
	Button_Close->OnClicked.AddUniqueDynamic(this,&UOptionWidget::OnClickedCloseButton);
	Button_Quit->OnClicked.AddUniqueDynamic(this,&UOptionWidget::OnClickedQuitButton);
	
	Slider_BGM->OnValueChanged.AddUniqueDynamic(this,&UOptionWidget::OnBGMVolumeChanged);
	Slider_SFX->OnValueChanged.AddUniqueDynamic(this,&UOptionWidget::OnSFXVolumeChanged);
	Slider_Mouse->OnValueChanged.AddUniqueDynamic(this,&UOptionWidget::OnMouseSensitivityChanged);
	
	if(auto instance = GetGameInstance<USoulLikeInstance>())
	{
		Slider_BGM->SetValue(instance->GetBGMVolume());
		Slider_SFX->SetValue(instance->GetSFXVolume());
	}

	LoadCurrentOptions();
}

void UOptionWidget::OnResolutionChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	if(SelectedItem.Equals("WindowedFullscreen"))
	{
		UUserSettingLibrary::SetWindowedFullScreen();
		return;
	}
	
	auto option = ResolutionOptions[SelectedItem];
	EWindowMode::Type windowMode;
	if(CheckBox_Fullscreen->IsChecked())
	{
		windowMode = EWindowMode::Fullscreen;
	}else
	{
		windowMode = EWindowMode::Windowed;
	}
	UUserSettingLibrary::SetResolution(option.X,option.Y,windowMode);
}

void UOptionWidget::OnFrameLimitChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	UUserSettingLibrary::SetFrameRateLimit(ComboBoxString_FrameLimit->GetSelectedIndex());
}

void UOptionWidget::OnDynamicResolutionChecked(bool bIsChecked)
{
	UUserSettingLibrary::SetDynamicResolution(bIsChecked);
}

void UOptionWidget::OnFullscreenChecked(bool bIsChecked)
{
	if(bIsChecked){
		UGameUserSettings::GetGameUserSettings()->SetFullscreenMode(EWindowMode::Fullscreen);
	}else
	{
		UGameUserSettings::GetGameUserSettings()->SetFullscreenMode(EWindowMode::Windowed);
	}
	
	UGameUserSettings::GetGameUserSettings()->SaveSettings();
}

void UOptionWidget::OnVSyncChecked(bool bIsChecked)
{

	UUserSettingLibrary::SetVSyncEnabled(bIsChecked);
}

void UOptionWidget::OnDynamicGlobalIlluminationChecked(bool bIsChecked)
{
	UUserSettingLibrary::SetDynamicGlobalIlluminationEnabled(bIsChecked);
}

void UOptionWidget::OnGraphicOptionChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	int32 number = ComboBoxString_Graphic->GetSelectedIndex();
	if(number >= 4)
	{
		VerticalBox_CustomOption->SetVisibility(ESlateVisibility::Visible);
		LoadCurVRAMInfo();
		return;
	}
	UUserSettingLibrary::SetQualityAtOnce(number);

	VerticalBox_CustomOption->SetVisibility(ESlateVisibility::Collapsed);
}

void UOptionWidget::OnClickedQuitButton()
{
	UKismetSystemLibrary::QuitGame(this,GetOwningPlayer(),EQuitPreference::Quit,false);
}

void UOptionWidget::OnClickedCloseButton()
{
	SetVisibility(ESlateVisibility::Collapsed);
}

void UOptionWidget::OnMouseSensitivityChanged(float Value)
{
	GetOwningPlayer<AUserController>()->SetMouseSensitivity(Value);
	TextBlock_Mouse->SetText(FText::AsNumber(static_cast<int32>(Value * 100)));

}

void UOptionWidget::OnBGMVolumeChanged(float Value)
{
	if(auto instance = GetGameInstance<USoulLikeInstance>())
	{
		instance->SetBGMVolume(Value);
		TextBlock_BGM->SetText(FText::AsNumber(static_cast<int32>(Value * 100)));
	}
}

void UOptionWidget::OnSFXVolumeChanged(float Value)
{
	if(auto instance = GetGameInstance<USoulLikeInstance>())
	{
		instance->SetSFXVolume(Value);
		TextBlock_SFX->SetText(FText::AsNumber(static_cast<int32>(Value * 100)));
	}
}

void UOptionWidget::OnTextureStreamingMemoryChanged(float Value)
{
	TextBlock_TextureMemory->SetText(FText::AsNumber(static_cast<int32>(Value)));
	if(auto instance = GetGameInstance<USoulLikeInstance>())
	{
		UUserSettingLibrary::SetTextureStreamingPool(Value);
		instance->SaveTextureStreamingPool(static_cast<int32>(Value));
	}
}


void UOptionWidget::OnAntiQualityChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	UUserSettingLibrary::SetAntiAliasingQuality(ComboBoxString_AntiQuality->GetSelectedIndex());

}

void UOptionWidget::OnTextureChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	UUserSettingLibrary::SetTextureQuality(ComboBoxString_Texture->GetSelectedIndex());

}

void UOptionWidget::OnShadowChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	UUserSettingLibrary::SetShadowQuality(ComboBoxString_Shadow->GetSelectedIndex());

}

void UOptionWidget::OnPostProcessChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	UUserSettingLibrary::SetPostProcessingQuality(ComboBoxString_PostProcess->GetSelectedIndex());

}

void UOptionWidget::OnViewDistanceChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	UUserSettingLibrary::SetViewDistanceQuality(ComboBoxString_ViewDistance->GetSelectedIndex());
	
}

void UOptionWidget::OnEffectChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	UUserSettingLibrary::SetVisualEffectsQuality(ComboBoxString_Effect->GetSelectedIndex());
	
}

void UOptionWidget::OnDynamicGlobalIlluminationChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	UUserSettingLibrary::SetDynamicGlobalIlluminationQuality(ComboBoxString_DynamicGlobalIllumination->GetSelectedIndex());
}

void UOptionWidget::OnReflectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	UUserSettingLibrary::SetReflectionQuality(ComboBoxString_Reflection->GetSelectedIndex());
}

void UOptionWidget::OnFoliageChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	UUserSettingLibrary::SetFoliageQuality(ComboBoxString_Foliage->GetSelectedIndex());
}

void UOptionWidget::OnShadingChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	UUserSettingLibrary::SetShadingQuality(ComboBoxString_Shading->GetSelectedIndex());
}

FString UOptionWidget::FindCurResolutionByValue(const FIntPoint& CurResolution)
{
	if(auto find = ResolutionOptions.FindKey(CurResolution))
	{
		return *find;
	}

	return "None of the provided option values match your resolution.";
}

FString UOptionWidget::AntiOptionNumberToString(const int32& Option)
{
	switch(Option)
	{
	case 0:		return "NotUse";
	case 1:		return "FXAA";
	case 2:		return "TAA";
	case 3:		return "MSAA";
	case 4:		return "TSR";
	}

	return "NOT VALID";
}

USoulLikeInstance* UOptionWidget::GetSoullikeInstance() const
{
	return GetGameInstance<USoulLikeInstance>();
}
