// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTask.h"
#include "02_Ability/AbilityEffect.h"
#include "GameplayTask_LaunchEvent.generated.h"

class ABaseCharacter;
/**
 * 
 */
UCLASS()
class SOULLIKE_API UGameplayTask_LaunchEvent : public UGameplayTask
{
	GENERATED_BODY()

	UGameplayTask_LaunchEvent(const FObjectInitializer& ObjectInitializer);

	//틱이 실행되는 주기입니다.
	float TickRate = -1;
	UPROPERTY(Transient)
	float CurrentTime;

	//추가 오브젝트를 전달해서 유지해야 할 경우 사용하세요.
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	UObject* AdditionalObject = nullptr;

public:
	UFUNCTION(BlueprintCallable)
	static UGameplayTask_LaunchEvent* LaunchEvent(AActor* TaskOwner, UObject* AdditionalObj = nullptr,
	                                              float TickRateValue = -1);

	virtual void Activate() override;
	virtual void TickTask(float DeltaTime) override;
	void SetTickRate(float Rate) { TickRate = Rate; }


	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTaskActivate);

	UPROPERTY(BlueprintAssignable)
	FOnTaskActivate OnTaskActivate;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTaskTick);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTaskTickWithDelta, float, DeltaTime);

	UPROPERTY(BlueprintAssignable)
	FOnTaskTick OnTaskTick;

	//델타타임이 필요한 경우 바인드 하세요.
	UPROPERTY(BlueprintAssignable)
	FOnTaskTickWithDelta OnTaskTickWithDelta;

	//델타타임
	UPROPERTY(BlueprintReadOnly)
	float DT;

	template <typename T>
	T* GetAdditionalObject();
};

template <typename T>
T* UGameplayTask_LaunchEvent::GetAdditionalObject()
{
	return Cast<T>(AdditionalObject);
}

UCLASS()
class SOULLIKE_API UGameplayTask_AddImpulse : public UGameplayTask
{
	GENERATED_BODY()

	UGameplayTask_AddImpulse(const FObjectInitializer& ObjectInitializer);

	UPROPERTY()
	class UCharacterMovementComponent* CharacterMovementComponent;
	UPROPERTY()
	float FinishTime;
	UPROPERTY()
	float ForcePower;
	UPROPERTY()
	FVector DirVector;
	UPROPERTY()
	float TotalTime;
	UPROPERTY()
	bool bVelocityChange;

public:
	UFUNCTION(BlueprintCallable)
	static UGameplayTask_AddImpulse* Task_AddImpulse(ABaseCharacter* TaskOwner, float Time, float Power,
	                                                 FVector DirectionVector, bool bChangeVelocity);

	virtual void Activate() override;
	virtual void TickTask(float DeltaTime) override;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTaskFinished);

	UPROPERTY(BlueprintAssignable)
	FOnTaskFinished OnTaskFinished;
};
