// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "00_Character/03_Monster/BaseMonster.h"
#include "Perception/AIPerceptionTypes.h"
#include "SoulLike/SoulLike.h"
#include "MonsterAIController.generated.h"
DECLARE_LOG_CATEGORY_EXTERN(LogAICon, Log, All);

//AI 테스크를 끝내기 위한 이벤트입니다.
DECLARE_DELEGATE(FOnFinishTask);

/**
 * 몬스터가 사용할 AI컨트롤러입니다.
 * 팀과 감각기능을 담당합니다.
 * AI 컨트롤러 내부에는 팀 기능이 내장되어 있습니다.
 */
UCLASS()
class SOULLIKE_API AMonsterAIController : public AAIController
{
	GENERATED_BODY()

public:
	AMonsterAIController();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool ShouldForceCombatState();
	//대상을 감지했을 떄 호출됨.
	UFUNCTION()
	void OnTargetPerceptionUpdatedEvent(AActor* Target, FAIStimulus Stimulus);

	//대상을 잊었을 때 호출됨
	UFUNCTION()
	void OnTargetPerceptionForgottenEvent(AActor* Target);

	UFUNCTION()
	void OnDeadEvent(AActor* Who, AActor* DeadBy);

	virtual void OnPossess(APawn* InPawn) override;

	UFUNCTION(BlueprintCallable)
	void StartBehavior();

	//팀 비교하는 함수
	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;

	//블랙보드에 기억했던 내용을 초기화 합니다.
	void ResetBlackboard();
	void StartImmediatelyBehavior();
	void DrawSightDebugLine();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void PostInitializeComponents() override;

	virtual void UpdateControlRotation(float DeltaTime, bool bUpdatePawn) override;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsBehaviorTreeRunning();

	void OverrideSightConfig(class UMonsterDataAsset* MonsterDataAsset);

protected:
	FRotator SmoothTargetRotation;

	//이 값이 0이면 즉시 회전합니다 주의하세요.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SmoothFocusInterpSpeed = 30.0f;

public:
	UFUNCTION(BlueprintCallable)
	void SetSmoothFocusInterpSpeed(float NewSpeed) { SmoothFocusInterpSpeed = NewSpeed; }
};
