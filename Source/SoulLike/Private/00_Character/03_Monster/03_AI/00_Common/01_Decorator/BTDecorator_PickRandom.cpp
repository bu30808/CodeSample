// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/03_Monster/03_AI/00_Common/01_Decorator/BTDecorator_PickRandom.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Logging/StructuredLog.h"
#include "Elements/Framework/TypedElementHandle.h"

UBTDecorator_PickRandom::UBTDecorator_PickRandom()
{
}

bool UBTDecorator_PickRandom::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	/*TArray<FTypedElementHandle> out;
	OwnerComp.GetComponentChildElements(out);

	UE_LOGFMT(LogTemp,Warning,"이 데코레이터 노드의 자식의 갯수 : {0}",out.Num());*/

	// 부모 클래스의 CalculateRawConditionValue를 호출하여 블랙보드 검사를 수행합니다.
	bool ParentResult = Super::CalculateRawConditionValue(OwnerComp, NodeMemory);

	if (ParentResult)
	{
		// 랜덤 숫자 생성 (예: 1에서 10까지의 랜덤 정수)
		int32 RandomNumber = FMath::RandRange(0, 10);

		// 블랙보드 컴포넌트에 액세스하여 랜덤 숫자를 저장합니다.
		UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
		if (BlackboardComp)
		{
			// 랜덤 숫자를 블랙보드에 저장합니다. "RandomNumberKey"는 블랙보드에서 사용할 키입니다.
			BlackboardComp->SetValueAsInt(GetSelectedBlackboardKey(), RandomNumber);

			// 데코레이터를 통과합니다.
			return true;
		}
	}

	// 랜덤 숫자를 생성하지 못하거나 블랙보드에 저장하지 못한 경우 데코레이터를 통과하지 않습니다.
	return false;
}
