// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputSubsystems.h"
#include "GameplayTask.h"
#include "00_Character/00_Player/00_Controller/00_Component/InputHandlerComponent.h"
#include "GameplayTask_WaitKeyInput.generated.h"


/**
 * 
 */
UCLASS()
class SOULLIKE_API UGameplayTask_WaitKeyInput : public UGameplayTask
{
	GENERATED_BODY()

private:
	UPROPERTY()
	FKeyPressedInfo WaitAction;
	UPROPERTY()
	TWeakObjectPtr<class APlayerController> PC;
	UPROPERTY()
	TWeakObjectPtr<class APlayerCharacter> PlayerCharacter;

	bool bWaitCont = false;

	UPROPERTY()
	TArray<FKey> Actionkeys;

	/*UPROPERTY()
	TWeakObjectPtr<class UEnhancedInputLocalPlayerSubsystem> InputLocalPlayerSubsystem;*/

	UGameplayTask_WaitKeyInput(const FObjectInitializer& ObjectInitializer);

public:
	/**
	 * @brief 플레이어의 특정 키 입력을 기다리는 태스크
	 * @param playerCharacter 키를 기다리는 대상 
	 * @param waitKey 어떤 키를 기다리는가?
	 * @param bAutoTaskEnd 키를 받았다면 자동으로 종료할 것인가? 거짓이면 키 입력을 다시 기다리게 됩니다.
	 * @return 
	 */
	UFUNCTION(BlueprintCallable)
	static UGameplayTask_WaitKeyInput* WaitKeyInput(class APlayerCharacter* Player, const FKeyPressedInfo& ActionInfo,
	                                                bool bAutoTaskEnd = true);


	virtual void Activate() override;
	virtual void TickTask(float DeltaTime) override;


	//기다리던 키가 눌렸을 때, 호출되는 이벤트입니다.
	UPROPERTY(BlueprintAssignable)
	FOnKeyPressedWithAction OnKeyPressedWithAction;
};
