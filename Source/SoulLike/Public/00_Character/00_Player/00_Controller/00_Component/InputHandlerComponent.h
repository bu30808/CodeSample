// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "InputAction.h"
#include "00_Character/00_Player/00_Controller/UserController.h"
#include "Components/ActorComponent.h"
#include "InputHandlerComponent.generated.h"


/**
 * 특정 어빌리티에서 기다리는 키가 눌렸는지 확인하기 위한 정보를 저장하는 구조체입니다.
 */
USTRUCT(BlueprintType)
struct FKeyPressedInfo
{
	GENERATED_BODY()

	//기다리는 키 액션
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UInputAction* InputAction = nullptr;

	//어떤 어빌리티가 이 키 입력을 구하고 있는지?
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag AbilityTag;

	FKeyPressedInfo()
	{
	}

	FKeyPressedInfo(class UInputAction* Action, const FGameplayTag& Tag): InputAction(Action), AbilityTag(Tag)
	{
	}

	FORCEINLINE bool operator==(const FKeyPressedInfo& Other) const
	{
		return InputAction == Other.InputAction && AbilityTag.MatchesTagExact(Other.AbilityTag);
	}

	FORCEINLINE friend uint32 GetTypeHash(const FKeyPressedInfo& KeyInfo)
	{
		return HashCombine(GetTypeHash(KeyInfo.InputAction), GetTypeHash(KeyInfo.AbilityTag));
	}
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnKeyNotPressed);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnKeyPressedWithAction, const FKeyPressedInfo&, ActionInfo);

/*
 *	키 입력 대기를 처리하는 컴포넌트입니다.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SOULLIKE_API UInputHandlerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UInputHandlerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;


	UPROPERTY()
	TWeakObjectPtr<class AUserController> PC;
	UPROPERTY()
	TWeakObjectPtr<class UEnhancedInputLocalPlayerSubsystem> InputLocalPlayerSubsystem;

	/**
	 * StartWaitAction에서 대기중인 키 목록입니다.
	 */
	UPROPERTY()
	TArray<FKey> WaitActionKeys;

	UPROPERTY()
	TMap<FKeyPressedInfo, class UGameplayTask_WaitKeyInput*> WaitActionTasks;

	/**
	 * 특정 액션 키가 눌렸는지 확인할 맵
	 */
	UPROPERTY()
	TMap<FKeyPressedInfo, bool> PressedActionMap;

	/**
	 * 특정 행동에 대한 키가 입력되었을 때, 할 행동을 저장하는 맵
	 */
	UPROPERTY()
	TMap<FKeyPressedInfo, FOnKeyPressedWithAction> InputPressedActionMap;

	/**
	 * 특정 행동에 대한 키가 입력되지 않았을 때 할 행동을 저장하는 맵
	 */
	UPROPERTY()
	TMap<FKeyPressedInfo, FOnKeyNotPressed> InputNotPressedMap;
	bool bObserveTrigger;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;


	/**
	 * 해당 액션에 대한 키를 눌렀는가 대기를 시작합니다.
	 * @param WaitAction 기다리는 액션
	 * @param bTriggerImmediately 참이면 감지되면 즉시 트리거합니다.
	 */
	void StartWaitAction(const TArray<FKeyPressedInfo>& WaitAction, bool bTriggerImmediately = false);

	/**
	 * 해당 액션에 대한 대기를 종료하고, 만약 키 입력이 있던적이 있었다면, 해당 키 이벤트를 찾아 실행합니다.
	 * @param WaitAction 기다리는 액션
	 */
	void EndWaitAction(const TArray<FKeyPressedInfo>& WaitAction);


	void BindEvent(const FKeyPressedInfo& ActionInfo, const FOnKeyPressedWithAction& OnKeyPressed,
	               const FOnKeyNotPressed& OnKeyNotPressed);

	

	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnKeyPressedWithActionBP, class UInputAction*, Action);
	DECLARE_DYNAMIC_DELEGATE(FOnKeyNotPressedBP);

	void UnBindEvent(const FKeyPressedInfo& ActionInfo);


	/**
	 * 맵에서 해당 액션에 대한 행동을 찾아 실행합니다.
	 * @param Action 입력된액션
	 */
	void BroadcastPressedEvent(const FKeyPressedInfo& Action);

	/**
	 * 해당 액션에 대한 키가 눌리지 않을 때 무엇인가 합니다.
	 * @param WaitAction 기다렸던 액션
	 */
	void BroadcastNotPressedEvent(const FKeyPressedInfo& WaitAction);

private:
	UFUNCTION()
	void OnPressedKeyDown(const FKeyPressedInfo& Action);
};
