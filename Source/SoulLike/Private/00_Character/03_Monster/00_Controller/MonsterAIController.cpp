// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/03_Monster/00_Controller/MonsterAIController.h"

#include "BrainComponent.h"
#include "00_Character/00_Player/PlayerCharacter.h"
#include "00_Character/01_Component/AbilityComponent.h"
#include "00_Character/03_Monster/BaseMonster.h"
#include "00_Character/03_Monster/00_Controller/MonsterAIPerceptionComponent.h"
#include "97_Interface/BossMonsterInterface.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Logging/StructuredLog.h"
#include "Navigation/PathFollowingComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Damage.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISense_Hearing.h"
#include "Perception/AISense_Sight.h"
#include "Tasks/AITask_MoveTo.h"
DEFINE_LOG_CATEGORY(LogAICon)

#define ExecuteEffectTag FGameplayTag::RequestGameplayTag("Common.Active.Execute.Effect")

AMonsterAIController::AMonsterAIController()
{
	bAttachToPawn = true;

	PerceptionComponent = CreateDefaultSubobject<UMonsterAIPerceptionComponent>(TEXT("AIPerceptionComponent"));
	PerceptionComponent->bAutoRegister = true;
	PerceptionComponent->SetDominantSense(UAISense_Sight::StaticClass());
	UAISenseConfig_Sight* sightSense = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("sightConfig"));

	//시야 범위
	sightSense->SightRadius = 1200.f;
	//시야범위에 들어와 인지하는 도중 이 거리 밖으로 빠져나가면 시야에서 사라진 것으로 봅니다.
	sightSense->LoseSightRadius = 1500.f;
	//정면에서 45도만큼을 시야라고 생각합니다.
	sightSense->PeripheralVisionAngleDegrees = 60.f;

	//어떤 대상들을 감지할 것인가 설정합니다.
	FAISenseAffiliationFilter filter;
	filter.bDetectEnemies = true;
	filter.bDetectFriendlies = false;
	filter.bDetectNeutrals = false;

	//위의 필터정보를 시야에 넘깁니다. 즉 시야로 적으로 판단한 대상만 인식한다 는 뜻.
	sightSense->DetectionByAffiliation = filter;
	//최대 5초동안 시야범위밖으로 나가면 잊습니다.
	sightSense->SetMaxAge(5.f);

	PerceptionComponent->ConfigureSense(*sightSense);

	//피해에 대한 설정을 만듭니다.
	UAISenseConfig_Damage* damageSense = CreateDefaultSubobject<UAISenseConfig_Damage>(TEXT("damageConfig"));
	damageSense->SetMaxAge(5.f);
	//만든 피해설정을 감각으로 등록합니다.
	PerceptionComponent->ConfigureSense(*damageSense);


	PerceptionComponent->OnTargetPerceptionUpdated.AddUniqueDynamic(
		this, &AMonsterAIController::OnTargetPerceptionUpdatedEvent);
	PerceptionComponent->OnTargetPerceptionForgotten.AddUniqueDynamic(
		this, &AMonsterAIController::OnTargetPerceptionForgottenEvent);


	SetGenericTeamId(FGenericTeamId(static_cast<uint8>(ETeam::Monster)));
}

bool AMonsterAIController::ShouldForceCombatState()
{
	return GetPawn<ABaseMonster>()->ShouldForceCombatState();
}

void AMonsterAIController::OnTargetPerceptionUpdatedEvent(AActor* Target, FAIStimulus Stimulus)
{
	UKismetSystemLibrary::PrintString(this,TEXT("타겟을 감지했음"));

	if (Target != nullptr && Target->IsA<APlayerCharacter>())
	{

		if(ShouldForceCombatState())
		{
			Cast<APlayerCharacter>(Target)->SetPlayerState(EPlayerCharacterState::Combat);
		}
		if (GetBrainComponent() == nullptr)
		{
			StartBehavior();
		}

		if (auto bbComp = GetBlackboardComponent())
		{
			if (bbComp->GetValueAsObject("Target") == nullptr)
			{
				UE_LOGFMT(LogAICon, Log, "{0}이 타겟을 감지했습니다 : {1}", GetPawn()->GetActorNameOrLabel(), Target->GetName());
				bbComp->SetValueAsObject("Target", Target);
				if(const auto sense = UAIPerceptionSystem::GetSenseClassForStimulus(this, Stimulus)){
					if (sense.GetDefaultObject()->IsA<UAISense_Damage>())
					{
						GetPawn<ABaseMonster>()->SetMonsterState(EMonsterState::Aggressive);
					}
					else
					{
						GetPawn<ABaseMonster>()->SetMonsterState(EMonsterState::Beware);
					}

					if (UKismetSystemLibrary::DoesImplementInterface(GetPawn(), UBossMonsterInterface::StaticClass()))
					{
						//Cast<IBossMonsterInterface>(GetPawn())->ShowBossWidget(GetPawn<ABaseMonster>(), Target);
						IBossMonsterInterface::Execute_ShowBossWidget(GetPawn(), GetPawn<ABaseMonster>(), Target);
					}
				}
			}
		}
	}
}

void AMonsterAIController::OnTargetPerceptionForgottenEvent(AActor* Target)
{
	UKismetSystemLibrary::PrintString(this,TEXT("타겟을 잊음"));
	if (Target != nullptr)
	{
		if (auto bbComp = GetBlackboardComponent())
		{
			UE_LOGFMT(LogAICon, Warning, "{0}이 타겟을 잊었습니다. : {1}", GetPawn()->GetActorNameOrLabel(), Target->GetName());
			bbComp->SetValueAsObject("Target", nullptr);
			GetPawn<ABaseMonster>()->SetMonsterState(EMonsterState::Peaceful);
		}
	}
}

void AMonsterAIController::OnDeadEvent(AActor* Who, AActor* DeadBy)
{
	PerceptionComponent->SetActive(false);
}

void AMonsterAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	Cast<ABaseMonster>(InPawn)->OnDead.AddUniqueDynamic(this, &AMonsterAIController::OnDeadEvent);
}

void AMonsterAIController::StartBehavior()
{
	if (auto monster = Cast<ABaseMonster>(GetPawn()))
	{
		if (monster->CharacterState == ECharacterState::DEAD)
		{
			return;
		}


		if (monster->IsDead() == false && !monster->GetAbilityComponent()->HasEffectTag(ExecuteEffectTag))
		{
			UE_LOGFMT(LogTemp, Log, "{0} : 비헤이비어 트리 실행", monster->GetMonsterTag().GetTagName());
			RunBehaviorTree(monster->MonsterDataAsset->BehaviorTree);

			if (auto bbComp = GetBlackboardComponent())
			{
				bbComp->SetValueAsVector("HomeLocation", GetPawn()->GetActorLocation());
				if (UKismetSystemLibrary::DoesImplementInterface(monster, UAIInterface::StaticClass()))
				{
					float range;
					IAIInterface::Execute_GetAttackRange(monster, range);
					range = monster->GetScaledAttackRange(range);
					bbComp->SetValueAsFloat("AttackRange", range);
				}
			}
		}
	}
}

ETeamAttitude::Type AMonsterAIController::GetTeamAttitudeTowards(const AActor& Other) const
{
	if (const APawn* OtherPawn = Cast<APawn>(&Other))
	{
		// DEFAULT BEHAVIOR---------------------------------------------------
		if (const IGenericTeamAgentInterface* TeamAgent = Cast<IGenericTeamAgentInterface>(OtherPawn->GetController()))
		{
			return Super::GetTeamAttitudeTowards(*OtherPawn->GetController());
		}

		//OR CUSTOM BEHAVIOUR--------------------------------------------------
		if (const IGenericTeamAgentInterface* TeamAgent = Cast<IGenericTeamAgentInterface>(OtherPawn->GetController()))
		{
			//Create an alliance with Team with ID 10 and set all the other teams as Hostiles:
			FGenericTeamId OtherTeamID = TeamAgent->GetGenericTeamId();
			if (OtherTeamID == 10)
			{
				return ETeamAttitude::Neutral;
			}
			return ETeamAttitude::Hostile;
		}
	}

	return ETeamAttitude::Neutral;
}

void AMonsterAIController::ResetBlackboard()
{
	if (auto bbComp = GetBlackboardComponent())
	{
		bbComp->ClearValue("Target");
	}
}

void AMonsterAIController::BeginPlay()
{
	Super::BeginPlay();

	if (auto pawn = GetPawn<ABaseMonster>())
	{
		if (pawn->IsStartBehaviorTreeImmediately())
		{
			StartBehavior();
		}
	}
}

void AMonsterAIController::UpdateControlRotation(float DeltaTime, bool bUpdatePawn)
{
	Super::UpdateControlRotation(DeltaTime, false);


	//Smooth and change the pawn rotation
	if (bUpdatePawn)
	{
		//Get pawn
		APawn* const MyPawn = GetPawn();
		//Get Pawn current rotation
		const FRotator CurrentPawnRotation = MyPawn->GetActorRotation();

		//Calculate smoothed rotation
		SmoothTargetRotation = UKismetMathLibrary::RInterpTo_Constant(MyPawn->GetActorRotation(), ControlRotation,
		                                                              DeltaTime, SmoothFocusInterpSpeed);
		//Check if we need to change
		if (CurrentPawnRotation.Equals(SmoothTargetRotation, 1e-3f) == false)
		{
			//Change rotation using the Smooth Target Rotation
			MyPawn->FaceRotation(SmoothTargetRotation, DeltaTime);
		}
	}
}

bool AMonsterAIController::IsBehaviorTreeRunning()
{
	if (auto btComp = Cast<UBehaviorTreeComponent>(GetBrainComponent()))
	{
		return btComp->IsRunning();
	}

	return false;
}

void AMonsterAIController::OverrideSightConfig(UMonsterDataAsset* MonsterDataAsset)
{
	if (auto sight = PerceptionComponent->GetSenseConfig(UAISense::GetSenseID<UAISense_Sight>()))
	{
		auto sightConfig = Cast<UAISenseConfig_Sight>(sight);

		//시야 범위
		sightConfig->SightRadius = MonsterDataAsset->SightRadius;
		//시야범위에 들어와 인지하는 도중 이 거리 밖으로 빠져나가면 시야에서 사라진 것으로 봅니다.
		sightConfig->LoseSightRadius = MonsterDataAsset->LoseSightRadius;
		//정면에서 이 각도의*2 만큼을 시야라고 생각합니다.
		sightConfig->PeripheralVisionAngleDegrees = MonsterDataAsset->PeripheralVisionAngleDegrees;
		//범위 안이고, 시야 밖인 경우 이 시간동안 다시 시야에 들어오지 않으면 잊습니다.
		sightConfig->SetMaxAge(MonsterDataAsset->MaxAge);

		PerceptionComponent->ConfigureSense(*sightConfig);
	}

	/*if (auto damage = PerceptionComponent->GetSenseConfig(UAISense::GetSenseID<UAISense_Damage>()))
	{
		auto damageConfig = Cast<UAISenseConfig_Sight>(damage);
		damageConfig->SetMaxAge(MonsterDataAsset->MaxAge);
		PerceptionComponent->ConfigureSense(*damageConfig);
	}*/
}
