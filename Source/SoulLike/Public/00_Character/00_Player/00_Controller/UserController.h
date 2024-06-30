// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "GameFramework/PlayerController.h"
#include "UserController.generated.h"

/**
 * 플레이어 컨트롤러 클래스입니다.
 */
UCLASS()
class SOULLIKE_API AUserController : public APlayerController, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	AUserController();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	class UWidgetManagerComponent* WidgetManagerComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Input, meta = (AllowPrivateAccess = "true"))
	class UInputHandlerComponent* InputHandlerComponent;

public:
	FORCEINLINE class UInputHandlerComponent* GetInputHandlerComponent() const { return InputHandlerComponent; }
	FORCEINLINE class UWidgetManagerComponent* GetWidgetManagerComponent() const { return WidgetManagerComponent; }

	virtual void OnPossess(APawn* InPawn) override;

	virtual FGenericTeamId GetGenericTeamId() const override { return TeamId; }

private:
	FGenericTeamId TeamId;

	UPROPERTY(VisibleAnywhere)
	TArray<class UUserWidget*> PopUpStack;

public:
	TArray<class UUserWidget*> GetPopUpStack() { return PopUpStack; }

	//팝업위젯으로 추가합니다.
	UFUNCTION(BlueprintCallable)
	void AddToPopUp(UUserWidget* Widget);

	/**
	* ESC로 순차적으로 닫기거나 혹은
	* 모종의 이유로 닫겼다면, 팝업위젯에서 제거합니다.
	* 위젯 전용입니다.
	 * @param Widget 해당 위젯
	 * @param bIsRemovable 이 값이 참이면 RemoveFromParents를 호출합니다.
	 */
	UFUNCTION(BlueprintCallable)
	void RemoveFromPopUp(UUserWidget* Widget, bool bIsRemovable);

	UFUNCTION(BlueprintCallable)
	void ClosePopUp();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsThereAnyPopUp();

	UFUNCTION(BlueprintCallable)
	void SetMouseSensitivity(float Sensitivity);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetMouseSensitivity();


	UFUNCTION(BlueprintCallable)
	void AddDialogWidget(class AActor* NPC,class UDialogue* InDialogue);
};
