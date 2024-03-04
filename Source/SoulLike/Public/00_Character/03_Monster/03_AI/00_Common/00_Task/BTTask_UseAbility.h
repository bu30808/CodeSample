// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "00_Character/01_Component/AbilityComponent.h"
#include "00_Character/03_Monster/00_Controller/MonsterAIController.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_UseAbility.generated.h"


/**
 * 
 */
UCLASS()
class SOULLIKE_API UBTTask_UseAbility : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	FGameplayTag AbilityTag;

	//어빌리티가 끝날때 까지 이 태스크를 유지할지 여부입니다.
	UPROPERTY(EditAnywhere)
	bool bWaitAbilityEnd = true;

	//델리게이트 이벤트. 테스크 끝났을 떄 호출
	FOnFinishTask OnFinishTask;

	UPROPERTY()
	TWeakObjectPtr<class UAbilityAdditionalInformation> AdditionalInfo;

	UBTTask_UseAbility();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual FString GetStaticDescription() const override;
	UFUNCTION()
	void OnEndAbilityEvent();
};
