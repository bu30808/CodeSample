// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/04_NPC/NPCAIController.h"

#include "00_Character/BaseCharacter.h"
#include "00_Character/03_Monster/00_Controller/MonsterAIController.h"
#include "00_Character/03_Monster/00_Controller/MonsterAIPerceptionComponent.h"
#include "96_Library/AIConHelperLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AISenseConfig_Damage.h"
#include "Perception/AISenseConfig_Sight.h"


// Sets default values
ANPCAIController::ANPCAIController()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;


	PerceptionComponent->GetSenseConfig<UAISenseConfig_Sight>()->SetMaxAge(0);
	
	SetGenericTeamId(FGenericTeamId(ETeamAttitude::Neutral));
}

// Called when the game starts or when spawned
void ANPCAIController::BeginPlay()
{
	Super::BeginPlay();
}

ETeamAttitude::Type ANPCAIController::GetTeamAttitudeTowards(const AActor& Other) const
{
	return UAIConHelperLibrary::CheckTeam(GetPawn(), Other);
}



void ANPCAIController::OnTargetPerceptionUpdatedEvent(AActor* Target, FAIStimulus Stimulus)
{
	if (Stimulus.WasSuccessfullySensed())
	{
		if (auto pawn = GetPawn<ANPCBase>())
		{
			if(!UKismetSystemLibrary::DoesImplementInterface(pawn,UHostileInterface::StaticClass()))
			{
				return;
			}
			
			//적대적이 아닌 상태라면,
			if (IHostileInterface::Execute_IsHostile(pawn) == false)
			{
				if (auto sence = UAIPerceptionSystem::GetSenseClassForStimulus(this, Stimulus))
				{
					UE_LOGFMT(LogTemp,Log,"감지된 감각 : {0}",sence->GetName());
					//피해를 입었을 때,
					if (sence.GetDefaultObject()->IsA<UAISense_Damage>())
					{
						pawn->AddHitCount(pawn->GotHitCount);
						UE_LOGFMT(LogTemp,Log,"히트 카운트 : {0}",pawn->GotHitCount);
						if(pawn->IsBeHostile(pawn->GotHitCount,pawn->MaxHitCount))
						{
							IHostileInterface::Execute_SetHostile(pawn,Cast<ABaseCharacter>(Target));
							if(auto bbComp = GetBlackboardComponent())
							{
								bbComp->SetValueAsObject("Target",Target);
							}
						}else
						{
							pawn->ShowGotHitDialog(pawn,Target);
						}
					}
				}
			}
			//적대적인 상태라면
			else
			{
				Super::OnTargetPerceptionUpdatedEvent(Target, Stimulus);
			}
		}
	}
}

void ANPCAIController::OnTargetPerceptionForgottenEvent(AActor* Target)
{
	if (auto pawn = GetPawn<ANPCBase>())
	{
		if(!UKismetSystemLibrary::DoesImplementInterface(pawn,UHostileInterface::StaticClass()))
		{
			return;
		}
		
		if(IHostileInterface::Execute_IsHostile(pawn))
		{
			if (Target != nullptr && Target->IsA<ABaseCharacter>())
			{
				if (auto bbComp = GetBlackboardComponent())
				{
					UE_LOGFMT(LogAICon, Warning, "{0}이 타겟을 잊었습니다. : {1}", GetNameSafe(GetPawn()), Target->GetName());
					bbComp->SetValueAsObject("Target", nullptr);
					UAIConHelperLibrary::ChangePlayerState(this, Target, ECombatState::Peaceful);
				}
			}
		}
	}
}
