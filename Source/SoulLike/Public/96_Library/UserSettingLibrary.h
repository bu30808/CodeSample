// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "RHI.h"
#include "StaticBoundShaderState.h"
#include "UserSettingLibrary.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API UUserSettingLibrary : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Settings")
	static void SetDynamicGlobalIlluminationEnabled(bool bEnabled);

	UFUNCTION(BlueprintCallable, Category = "Settings")
	static void SetResolution(int32 Width, int32 Height, EWindowMode::Type WindowMode);
	UFUNCTION(BlueprintCallable, Category = "Settings")
	static void SetWindowedFullScreen();
	UFUNCTION(BlueprintCallable, Category = "Settings")
	static TMap<FString, FIntPoint> GetResolutionOptions();
	
	UFUNCTION(BlueprintCallable, Category = "Settings")
	static void SetAntiAliasingMode(int32 Mode);
	
	UFUNCTION(BlueprintCallable, Category = "Settings")
	static void SetAntiAliasingQuality(int32 Quality);

	UFUNCTION(BlueprintCallable, Category = "Settings")
	static void SetTextureQuality(int32 Quality);

	UFUNCTION(BlueprintCallable, Category = "Settings")
	static void SetShadowQuality(int32 Quality);

	UFUNCTION(BlueprintCallable, Category = "Settings")
	static void SetPostProcessingQuality(int32 Quality);

	UFUNCTION(BlueprintCallable, Category = "Settings")
	static void SetViewDistanceQuality(int32 Quality);

	UFUNCTION(BlueprintCallable, Category = "Settings")
	static void SetVisualEffectsQuality(int32 Quality);

	UFUNCTION(BlueprintCallable, Category = "Settings")
	static void SetVSyncEnabled(bool bEnabled);

	UFUNCTION(BlueprintCallable, Category = "Settings")
	static void SetQualityAtOnce(int32 Quality);

	// 안티앨리어싱 사용 여부
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Settings")
	static int32 GetAntiAliasingMode();

	// 안티앨리어싱 종류
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Settings")
	static int32 GetAntiAliasingQuality();

	// 레이트레이싱 사용 여부
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Settings")
	static bool IsDynamicGlobalIlluminationEnabled();
	// VSync 사용 여부
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Settings")
	static bool IsVSyncEnabled();

	// 품질 설정 레벨
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Settings")
	static int32 GetShadowQuality();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Settings")
	static int32 GetPostProcessingQuality();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Settings")
	static int32 GetEffectsQuality();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Settings")
	static int32 GetTextureQuality();

	UFUNCTION(BlueprintCallable, Category = "Settings")
	static void SetFrameRateLimit(int32 Mode);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Settings")
	static int32 GetFrameRateLimit();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Settings")
	static FString GetFrameRateLimitToString();
	
	UFUNCTION(BlueprintCallable, Category = "Settings")
	static void SetDynamicGlobalIlluminationQuality(int32 Quality);
	UFUNCTION(BlueprintCallable, Category = "Settings")
	static void SetReflectionQuality(int32 Quality);
	UFUNCTION(BlueprintCallable, Category = "Settings")
	static void SetFoliageQuality(int32 Quality);
	UFUNCTION(BlueprintCallable, Category = "Settings")
	static void SetShadingQuality(int32 Quality);
	UFUNCTION(BlueprintCallable, Category = "Settings")
	static void SetDynamicResolution(bool bEnable);

	UFUNCTION(BlueprintCallable, Category = "Settings")
	static void ShowVRAMInfo();
	static FTextureMemoryStats GetVRAMInfo();
	UFUNCTION(BlueprintCallable, Category = "Settings")
	static void SetTextureStreamingPool(float Value);
};
