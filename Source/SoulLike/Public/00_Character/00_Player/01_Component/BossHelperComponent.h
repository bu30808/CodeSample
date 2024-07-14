// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BossHelperComponent.generated.h"

/*
 *	우두머리 스폰 트리거를 관리하기 위한 컴포넌트 입니다.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SOULLIKE_API UBossHelperComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UBossHelperComponent();
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;


	UPROPERTY(VisibleAnywhere,Transient)
	TArray<TObjectPtr<class AActor>> TriggerBoxes;

	UPROPERTY(VisibleAnywhere,Transient)
	TArray<TObjectPtr<class AActor>> MistBlocks;

public:
	UFUNCTION(BlueprintCallable)
	void AddTrigger(AActor* Trigger);
	UFUNCTION(BlueprintCallable)
	void AddMistBlock(const TArray<AActor*>& MistBlockActors);

	//우두머리가 사망하면 호출됩니다. 트리거를 비활성화 합니다.
	UFUNCTION(BlueprintCallable)
	void DisableTrigger();
	//우두머리가 사망하면 호출됩니다. 안개의 벽을 비활성화 합니다.
	UFUNCTION(BlueprintCallable)
	void DisableMistBlock();

	//플레이어가 사망했을때 호출됩니다. 트리거를 초기상태로 되돌립니다.
	UFUNCTION(BlueprintCallable)
	void ResetTrigger();
	//플레이어가 사망했을때 호출됩니다. 안개의 벽을 초기상태로 되돌립니다.
	UFUNCTION(BlueprintCallable)
	void ResetMistBlock();
};
