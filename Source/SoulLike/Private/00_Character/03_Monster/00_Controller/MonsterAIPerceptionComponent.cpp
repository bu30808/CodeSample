// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/03_Monster/00_Controller/MonsterAIPerceptionComponent.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AISense_Damage.h"
#include "Perception/AISense_Sight.h"

//특정 대상에 대한 감각 만료를 알 수가 없네요.
void UMonsterAIPerceptionComponent::HandleExpiredStimulus(FAIStimulus& StimulusStore)
{
	//이 시점에서 이미 까먹고 리스트로 가져올 수 없음..
	if(StimulusStore.IsExpired())
	{
		
		if(auto sence = UAIPerceptionSystem::GetSenseClassForStimulus(this,StimulusStore))
		{
			//시야에 대해서만 처리합니다.
			if(sence.GetDefaultObject()->IsA<UAISense_Sight>())
			{
				TArray<AActor*> KnownPerceivedActors;
				GetKnownPerceivedActors(UAIPerceptionSystem::GetSenseClassForStimulus(this,StimulusStore),KnownPerceivedActors);
		
				//블랙보드에 기억하는 타겟이 이 리스트에 없으면, 잊어먹을걸로 합시다.
				if(auto bbComp = GetOwner<AAIController>()->GetBlackboardComponent())
				{
					if(auto target = bbComp->GetValueAsObject("Target"))
					{
						if(!KnownPerceivedActors.Contains(target))
						{
							ForgetActor(Cast<AActor>(target));
							OnTargetPerceptionForgotten.Broadcast(Cast<AActor>(target));
						}
					}
				}
				
			}
		}
		
	}
}
