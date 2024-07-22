// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "01_GameMode/SoulLikeGameMode.h"
#include "03_Widget/05_Alert/AlertWidget.h"
#include "03_Widget/11_Tutorial/TutorialWidget.h"
#include "UObject/NoExportTypes.h"
#include "WidgetHelperLibrary.generated.h"



/**
 * 
 */

UCLASS()
class SOULLIKE_API UWidgetHelperLibrary : public UObject
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	static void ShowOkCancelMsg(class AUserController* PC, EAlertMsgType AlertMsgType, FText Msg,
							 const FOnButtonClicked& OnClickedOKButtonEvent,const FOnButtonClicked& OnClickedCancelButtonEvent);
	//전달되는 Msg내부 String 가장 마지막줄 끝에 줄바꿈 넣지 마세요. 텍스트 데코레이터가 적용되지 않습니다.
	UFUNCTION(BlueprintCallable)
	static void ShowAlertMsg(class AUserController* PC, EAlertMsgType AlertMsgType, FText Msg,
	                         const FOnButtonClicked& OnClickedOKButtonEvent);

	UFUNCTION(BlueprintCallable)
	static class USimpleToolTipWidget* GetSimpleToolTipWidget(class APlayerController* PC, FText Msg);

	//인터렉션으로 위젯이 열렸을 때, 사전설정하는 함수입니다.
	UFUNCTION(BlueprintCallable)
	static void OpenWidgetSetting(class APlayerController* PC, class UUserWidget* OpenWidget);

	//위젯이 닫혔을 떄, 설정을 되돌립니다.
	UFUNCTION(BlueprintCallable)
	static void CloseWidgetSetting(APlayerController* PC, UUserWidget* ClosedWidget, bool RemainMousePoint = false);

	UFUNCTION(BlueprintCallable)
	static void SetToolTipWidget(UUserWidget* TargetWidget, const FText& Msg);
	UFUNCTION(BlueprintCallable)
	static void SetAbilityToolTipWidget(UUserWidget* TargetWidget, const FAbilityInformation& AbilityInformation);

	static FString EnhancementTagToDecoString(const FGameplayTag& Key);

	UFUNCTION(BlueprintCallable)
	static void ShowBossItemGetWidget(AUserController* PC, AItemActor* Item);

	UFUNCTION(BlueprintCallable)
	static class UTutorialWidget* ShowTutorialWidget(APlayerController* PC, FGameplayTag TutorialTag);

	/**
* ESC로 순차적으로 닫기거나 혹은
* 모종의 이유로 닫겼다면, 팝업위젯에서 제거합니다.
* 위젯 전용입니다.
 * @param Widget 해당 위젯
 * @param bIsRemovable 이 값이 참이면 RemoveFromParents를 호출합니다.
 */
	UFUNCTION(BlueprintCallable)
	static void PopUpWidgetProcess(UUserWidget* Widget, bool bIsRemovable);

	UFUNCTION(BlueprintCallable)
	static bool IsUseSaveGameMode(UObject* WorldContext);

	UFUNCTION(BlueprintCallable)
	static void StopLoadingScreen();
};
