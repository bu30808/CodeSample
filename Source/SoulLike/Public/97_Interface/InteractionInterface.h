// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Logging/StructuredLog.h"
#include "UObject/Interface.h"
#include "InteractionInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInteractionInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SOULLIKE_API IInteractionInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	//플레이어가 직접적으로 상호작용 위젯을 표시할 수 있는가 받아옵니다. 이 값이 참이어야 플레이어가 트레이스로 감지한 대상의 상호작용 위젯이 표시됩니다.
	//거짓인 경우, 다른 경로로 위젯을 표시해야 합니다.
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool ShowInteractionWidgetDirectly();
	virtual bool ShowInteractionWidgetDirectly_Implementation()
	{
		return true;
	}
	
	/**
	 * 상호작용 합니다.
	 * @param Start 상호작용을 건 대상입니다. 주로 플레이어 캐릭터가 됩니다.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Interaction(class ABaseCharacter* Start);
	virtual void Interaction_Implementation(class ABaseCharacter* Start)
	{
	}

	/**
	 * 상호작용을 끝냅니다.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void FinishInteraction();
	virtual void FinishInteraction_Implementation()
	{
		UE_LOGFMT(LogTemp,Error,"인터페이스 함수 FinishInteraction_Implementation 를 구현해야 합니다!!");
	}

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	FName GetActionName();
	virtual FName GetActionName_Implementation(){ return "인터페이스를 구현해 주세요."; }
};
