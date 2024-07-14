// Fill out your copyright notice in the Description page of Project Settings.


#include "96_Library/UserSettingLibrary.h"

#include "GameFramework/GameUserSettings.h"
#include "Kismet/KismetMathLibrary.h"
#include "Logging/StructuredLog.h"
#include "GameFramework/Actor.h"


void UUserSettingLibrary::SetDynamicGlobalIlluminationEnabled(bool bEnabled)
{
	if (GEngine)
	{
		GEngine->Exec(nullptr, *FString::Printf(TEXT("r.DynamicGlobalIlluminationMethod %d"), bEnabled ? 1 : 0));
	}
}

void UUserSettingLibrary::SetResolution(int32 Width, int32 Height, EWindowMode::Type WindowMode)
{
	if (GEngine)
	{
		FString ResolutionString = FString::Printf(TEXT("%dx%d%s"), Width, Height, WindowMode ? TEXT("f") : TEXT(""));
		GEngine->GameUserSettings->SetScreenResolution(FIntPoint(Width, Height));
		GEngine->GameUserSettings->SetFullscreenMode(WindowMode);
		GEngine->GameUserSettings->ApplySettings(false);
	}
}

void UUserSettingLibrary::SetWindowedFullScreen()
{
	if (GEngine)
	{
		GEngine->GameUserSettings->SetFullscreenMode(EWindowMode::WindowedFullscreen);
		GEngine->GameUserSettings->ApplySettings(false);
	}
}

TMap<FString, FIntPoint> UUserSettingLibrary::GetResolutionOptions()
{
	TMap<FString, FIntPoint> resolutions;
	// 16:9 해상도
	resolutions.Add("3840*2160",FIntPoint(3840, 2160)); // 4K UHD
	resolutions.Add("2560*1440",FIntPoint(2560, 1440)); // QHD
	resolutions.Add("1920*1080",FIntPoint(1920, 1080)); // Full HD
	resolutions.Add("1600*900",FIntPoint(1600, 900));
	resolutions.Add("1366*768",FIntPoint(1366, 768));
	resolutions.Add("1280*720",FIntPoint(1280, 720));  // HD

	// 16:10 해상도
	resolutions.Add("2560*1600",FIntPoint(2560, 1600));
	resolutions.Add("1920*1200",FIntPoint(1920, 1200));
	resolutions.Add("1680*1050",FIntPoint(1680, 1050));
	resolutions.Add("1440*900",FIntPoint(1440, 900));
	resolutions.Add("1280*800",FIntPoint(1280, 800));

	// 21:9 해상도
	resolutions.Add("3440*1440",FIntPoint(3440, 1440));
	resolutions.Add("2560*1080",FIntPoint(2560, 1080));

	// 4:3 해상도
	resolutions.Add("1600*1200",FIntPoint(1600, 1200));
	resolutions.Add("1280*960",FIntPoint(1280, 960));
	resolutions.Add("1024*768",FIntPoint(1024, 768));

	// 5:4 해상도
	resolutions.Add("1280*1024",FIntPoint(1280, 1024));
	
	return resolutions;
}

void UUserSettingLibrary::SetAntiAliasingMode(int32 Mode)
{
	switch (Mode)
	{
	case 0:
		GEngine->Exec(nullptr, TEXT("r.AntiAliasingMethod 0")); // None
		break;
	case 1:
		GEngine->Exec(nullptr, TEXT("r.AntiAliasingMethod 1")); // FXAA
		break;
	case 2:
		GEngine->Exec(nullptr, TEXT("r.AntiAliasingMethod 2")); // TAA
		break;
	case 3:
		GEngine->Exec(nullptr, TEXT("r.AntiAliasingMethod 3")); //MSAA
		break;
	case 4:
		GEngine->Exec(nullptr, TEXT("r.TemporalAA.Upsampling 1"));
		GEngine->Exec(nullptr, TEXT("r.AntiAliasingMethod 4"));
		break;
	}

	GEngine->GameUserSettings->ApplySettings(false);
}

void UUserSettingLibrary::SetAntiAliasingQuality(int32 Quality)
{
	if (GEngine)
	{
		GEngine->GameUserSettings->SetAntiAliasingQuality(Quality);
		GEngine->GameUserSettings->ApplySettings(false);
	}
}

void UUserSettingLibrary::SetTextureQuality(int32 Quality)
{
	if (GEngine)
	{
		GEngine->GameUserSettings->SetTextureQuality(Quality);
		GEngine->GameUserSettings->ApplySettings(false);
	}
}

void UUserSettingLibrary::SetShadowQuality(int32 Quality)
{
	if (GEngine)
	{
		GEngine->GameUserSettings->SetShadowQuality(Quality);
		GEngine->GameUserSettings->ApplySettings(false);
	}
}

void UUserSettingLibrary::SetPostProcessingQuality(int32 Quality)
{
	if (GEngine)
	{
		GEngine->GameUserSettings->SetPostProcessingQuality(Quality);
		GEngine->GameUserSettings->ApplySettings(false);
	}
}

void UUserSettingLibrary::SetViewDistanceQuality(int32 Quality)
{
	if (GEngine)
	{
		GEngine->GameUserSettings->SetViewDistanceQuality(Quality);
		GEngine->GameUserSettings->ApplySettings(false);
	}
}

void UUserSettingLibrary::SetVisualEffectsQuality(int32 Quality)
{
	if (GEngine)
	{
		GEngine->GameUserSettings->SetVisualEffectQuality(Quality);
		GEngine->GameUserSettings->ApplySettings(false);
	}
}

void UUserSettingLibrary::SetVSyncEnabled(bool bEnabled)
{
	if (GEngine)
	{
		GEngine->GameUserSettings->SetVSyncEnabled(bEnabled);
		GEngine->GameUserSettings->ApplySettings(false);
	}
}

void UUserSettingLibrary::SetQualityAtOnce(int32 Quality)
{
	if(GEngine)
	{
		GEngine->GameUserSettings->SetOverallScalabilityLevel(Quality);
		GEngine->GameUserSettings->ApplySettings(false);
	}
}

int32 UUserSettingLibrary::GetAntiAliasingMode()
{
	if (IConsoleVariable* CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.DefaultFeature.AntiAliasing")))
	{
		return CVar->GetInt();
	}
	return -1;
}

int32 UUserSettingLibrary::GetAntiAliasingQuality()
{
	if (GEngine)
	{
		return GEngine->GameUserSettings->GetAntiAliasingQuality();
	}
	return 0; // Default to None
}

bool UUserSettingLibrary::IsDynamicGlobalIlluminationEnabled()
{
	IConsoleVariable* CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.DynamicGlobalIlluminationMethod"));
	if (CVar)
	{
		return CVar->GetInt() == EDynamicGlobalIlluminationMethod::Lumen;
	}
	
	
	return false;
}

bool UUserSettingLibrary::IsVSyncEnabled()
{
	if (GEngine)
	{
		return GEngine->GameUserSettings->IsVSyncEnabled();
	}
	return false;
}

int32 UUserSettingLibrary::GetShadowQuality()
{
	if (GEngine)
	{
		return GEngine->GameUserSettings->GetShadowQuality();
	}
	return 0; // Default to Low
}

int32 UUserSettingLibrary::GetPostProcessingQuality()
{
	if (GEngine)
	{
		return GEngine->GameUserSettings->GetPostProcessingQuality();
	}
	return 0; // Default to Low
}

int32 UUserSettingLibrary::GetEffectsQuality()
{
	if (GEngine)
	{
		return GEngine->GameUserSettings->GetVisualEffectQuality();
	}
	return 0; // Default to Low
}

int32 UUserSettingLibrary::GetTextureQuality()
{
	if (GEngine)
	{
		return GEngine->GameUserSettings->GetTextureQuality();
	}
	return 0; // Default to Low
}

void UUserSettingLibrary::SetFrameRateLimit(int32 Mode)
{
	if(GEngine)
	{

		float newLimit = 120.f;
		switch(Mode)
		{
		case 0:
			newLimit = 0;
			break;
		case 1:
			newLimit = 120.f;
			break;
		case 2:
			newLimit = 90.f;
			break;
		case 3:
			newLimit = 60.f;
			break;
		case 4:
			newLimit = 30.f;
			break;
		}
		
		GEngine->GameUserSettings->SetFrameRateLimit(newLimit);
		GEngine->GameUserSettings->ApplySettings(false);
	}
}

int32 UUserSettingLibrary::GetFrameRateLimit()
{
	if(GEngine)
	{
		return UKismetMathLibrary::FTrunc(GEngine->GameUserSettings->GetFrameRateLimit());
	}
	return -1;
}

FString UUserSettingLibrary::GetFrameRateLimitToString()
{
	switch(GetFrameRateLimit())
	{
	case 0:
		return "Unlmited";
	case 1:
		return "120";
	case 2:
		return "90";
	case 3:
		return "60";
	case 4:
		return "30";
	}
	
	return "ERROR";
}

void UUserSettingLibrary::SetDynamicGlobalIlluminationQuality(int32 Quality)
{
	if(GEngine)
	{
		GEngine->GameUserSettings->SetGlobalIlluminationQuality(Quality);
		GEngine->GameUserSettings->ApplySettings(false);
	}
}

void UUserSettingLibrary::SetReflectionQuality(int32 Quality)
{
	if(GEngine)
	{
		GEngine->GameUserSettings->SetReflectionQuality(Quality);
		GEngine->GameUserSettings->ApplySettings(false);
	}
}

void UUserSettingLibrary::SetFoliageQuality(int32 Quality)
{
	if(GEngine)
	{
		GEngine->GameUserSettings->SetFoliageQuality(Quality);
		GEngine->GameUserSettings->ApplySettings(false);
	}
}

void UUserSettingLibrary::SetShadingQuality(int32 Quality)
{
	if(GEngine)
	{
		GEngine->GameUserSettings->SetShadingQuality(Quality);
		GEngine->GameUserSettings->ApplySettings(false);
	}
}

void UUserSettingLibrary::SetDynamicResolution(bool bEnable)
{
	if(GEngine)
	{
		GEngine->GameUserSettings->SetDynamicResolutionEnabled(bEnable);
		GEngine->GameUserSettings->ApplySettings(false);
	}
}

void UUserSettingLibrary::ShowVRAMInfo()
{
	FTextureMemoryStats textureMemoryStats;
	GDynamicRHI->RHIGetTextureMemoryStats(textureMemoryStats);
	UE_LOGFMT(LogTemp,Log,"---------------텍스쳐 메모리 상태----------------");
	UE_LOGFMT(LogTemp,Log,"TotalGraphicsMemory : {0}MB",textureMemoryStats.TotalGraphicsMemory/ 1024 / 1024);
	UE_LOGFMT(LogTemp,Log,"StreamingMemorySize : {0}MB",textureMemoryStats.StreamingMemorySize/ 1024 / 1024);
	UE_LOGFMT(LogTemp,Log,"TexturePoolSize : {0}MB",textureMemoryStats.TexturePoolSize/ 1024 / 1024);	
	UE_LOGFMT(LogTemp,Log,"ComputeAvailableMemorySize : {0}MB",textureMemoryStats.ComputeAvailableMemorySize()/ 1024 / 1024);
}


FTextureMemoryStats UUserSettingLibrary::GetVRAMInfo()
{

	FTextureMemoryStats textureMemoryStats;
	GDynamicRHI->RHIGetTextureMemoryStats(textureMemoryStats);
	return textureMemoryStats;
	/*IDXGIFactory4* pFactory;
	CreateDXGIFactory1(__uuidof(IDXGIFactory4), (void**)&pFactory);

	IDXGIAdapter3* adapter;
	pFactory->EnumAdapters(0, reinterpret_cast<IDXGIAdapter**>(&adapter));

	DXGI_QUERY_VIDEO_MEMORY_INFO videoMemoryInfo;
	adapter->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &videoMemoryInfo);

	UE_LOGFMT(LogTemp,Log,"---------------VRAM 상태----------------");
	UE_LOGFMT(LogTemp,Log,"전체 사용 가능한 VRAM : {0}MB",videoMemoryInfo.Budget/ 1024 / 1024);
	UE_LOGFMT(LogTemp,Log,"사용중인 VRAM : {0}MB",videoMemoryInfo.CurrentUsage / 1024 / 1024);	*/

}

void UUserSettingLibrary::SetTextureStreamingPool(float Value)
{
	if (GEngine)
	{
		GEngine->Exec(nullptr, *FString::Printf(TEXT("r.Streaming.PoolSize %d"), static_cast<int>(Value)));
	}
}
