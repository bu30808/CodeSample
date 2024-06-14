// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "SoulLike/SoulLike.h"
#include "BTTask_PlayAnimMontage.generated.h"


DECLARE_DELEGATE(FMontageBlendOut);

UENUM(BlueprintType)
enum class EMontageEndType : uint8
{
	BLEND_OUT,
	ENDED
};

/**
 * 
 */
UCLASS()
class SOULLIKE_API UBTTask_PlayAnimMontage : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

	/*
	FAITaskFinishDelegate OnMontageFinishedEvent;
	FAITaskFinishDelegate OnTaskFinishedEvent;
	//가끔 탈출하지 못하는 경우를 위한 강제종료용 타이머
	FTimerHandle FinishTimerHandle;
	*/
	UPROPERTY()
	TObjectPtr<AAIController> AIController;
	UPROPERTY(Transient)
	bool bOriginalMirror;

	UBTTask_PlayAnimMontage();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	                            EBTNodeResult::Type TaskResult) override;

	UFUNCTION()
	void OnMontageEndEvent(UAnimMontage* Montage, bool bInterrupted);

public:
	UPROPERTY(EditAnywhere)
	class UAnimMontage* MontageToPlay;
	//이 값에 따라서 테스크를 종료하는 시점이 조금씩 달라집니다.
	UPROPERTY(EditAnywhere)
	EMontageEndType MontageFinishType;
	//이 값이 거짓일 경우, 몬스터 속성값중 ActionSpeed값을 재생속도로 사용합니다.
	UPROPERTY(EditAnywhere)
	bool bUseCustomPlayRate;
	UPROPERTY(EditAnywhere, meta=(EditCondition = "bUseCustomPlayRate"))
	float PlaySpeed = 1.f;
	UPROPERTY(EditAnywhere)
	bool bEnableRootMotion = true;
	//이 값이 참이면, 실행과 동시에 즉시 성공처리 됩니다.
	UPROPERTY(EditAnywhere)
	bool bNonBlocking;
	//이 값이 참이면, 히트상태가 되면 몽타주가 중지됩니다.
	UPROPERTY(EditAnywhere)
	bool bCanHitCancel;
	//이 값이 참이면 애니메이션을 좌우반전합니다.
	UPROPERTY(EditAnywhere)
	bool bMirrorAnimation = false;
	//강제로 Hit 애니메이션 빠져나감 이벤트를 트리거 해야 할 때 참으로 주세요.
	//이 값이 참이면, 이 노드가 실행되는 순간 Hit애니메이션이 재생중이라면 취소되고 NORMAL상태로 되돌립니다.
	UPROPERTY(EditAnywhere)
	bool bForceTriggerHitExitEvent = true;
};
