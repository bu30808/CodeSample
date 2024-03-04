// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "WidgetInteractionSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API UWidgetInteractionSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()

protected:
	UWidgetInteractionSubsystem();

	//위젯이 열리면 사용할 인풋 컨택스트
	UPROPERTY(EditAnywhere)
	class UInputMappingContext* WidgetInputContext;

	//게임이 꺼질때까지 유지되기 때문에, 레벨이 바뀌면 초기화 해야 합니다.
	UPROPERTY()
	TArray<bool> WidgetOpenStack;

public:
	UFUNCTION()
	void OpenWidgetSetting(class APlayerController* PC, UUserWidget* FocusWidget);

	UFUNCTION()
	void CloseWidgetSetting(APlayerController* PC, bool RemainMousePoint);

	UFUNCTION()
	void InitSubsystem();

private:
	class UEnhancedInputLocalPlayerSubsystem* GetLocalInputSystem(class APlayerController* PC) const;
};
