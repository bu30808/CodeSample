// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/03_Monster/05_Lich/LichBoss.h"

#include "AIController.h"
#include "00_Character/01_Component/AbilityComponent.h"
#include "00_Character/02_Animation/00_NotifyState/AnimNotifyState_SpawnProjectile.h"
#include "00_Character/03_Monster/00_Controller/MonsterAIController.h"
#include "02_Ability/02_Projectile/ProjectileActor.h"
#include "94_Task/GameplayTask_LaunchTimerEvent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Engine/TargetPoint.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetArrayLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Logging/StructuredLog.h"

ALichBoss::ALichBoss()
{
	if (MonsterDataAsset)
	{
		MonsterDataAsset->bRespawnWhenPlayerRest = false;
	}
}

void ALichBoss::BeginPlay()
{
	Super::BeginPlay();

	UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), ATargetPoint::StaticClass(), "Lich", SpawnPoints);
}

void ALichBoss::SpawnBeam()
{
	if (BeamObject)
	{
		FActorSpawnParameters spawnParams;
		spawnParams.Owner = this;
		spawnParams.Instigator = this;
		spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		Beam = GetWorld()->SpawnActor<AActor>(BeamObject, GetMesh()->GetSocketLocation("BeamSocket"),
		                                      GetActorRotation(), spawnParams);
		Beam->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::KeepWorld, true), "BeamSocket");
	}
}

void ALichBoss::DestroyBeam()
{
	if (Beam.IsValid())
	{
		Beam->Destroy();
	}
}

void ALichBoss::K2_SpawnAlly_Implementation(int32 SpawnCount)
{
	AllySpawnCount = SpawnCount;
	GetMesh()->GetAnimInstance()->Montage_Play(SummonAllyMontage);
	GetWorldTimerManager().SetTimer(SpawnAllyTimerHandle, this, &ALichBoss::SpawnAllyEvent, 1.f, true);
}

bool ALichBoss::CanSpawnMoreAlly() const
{
	return SpawnMonsters.Num() < AllySpawnCount;
}

void ALichBoss::OnAllyDeadEvent(AActor* Who, AActor* DeadBy)
{
	SpawnMonsters.Remove(Who);
}


void ALichBoss::SpawnAllyEvent()
{
	if (SpawnMonsters.Num() >= AllySpawnCount)
	{
		GetWorldTimerManager().ClearTimer(SpawnAllyTimerHandle);
		OnFinishSpawnAllyEvent.Broadcast(
			Cast<UBehaviorTreeComponent>(GetController<AAIController>()->GetBrainComponent()),
			EBTNodeResult::Succeeded);
	}
	else
	{
		if (auto spawnMonster = SpawnRandomMonster())
		{
			SpawnMonsters.AddUnique(spawnMonster);
			spawnMonster->OnDead.AddUniqueDynamic(this, &ALichBoss::OnAllyDeadEvent);
			ShareTarget(spawnMonster);
			spawnMonster->GetAbilityComponent()->ApplyCue(SpawnCueInformation);
		}
	}
}

ABaseMonster* ALichBoss::SpawnRandomMonster()
{
	int32 randPointIndex = FMath::RandRange(0, SpawnPoints.Num() - 1);
	if (SpawnPoints.IsValidIndex(randPointIndex))
	{
		auto point = SpawnPoints[randPointIndex];

		int32 randMonsterIndex = FMath::RandRange(0, SpawnableMonsters.Num() - 1);
		if (SpawnableMonsters.IsValidIndex(randMonsterIndex))
		{
			FActorSpawnParameters spawnParams;
			spawnParams.Instigator = this;
			spawnParams.Owner = this;
			spawnParams.SpawnCollisionHandlingOverride =
				ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
			return GetWorld()->SpawnActor<ABaseMonster>(SpawnableMonsters[randMonsterIndex], point->GetActorLocation(),
			                                            FRotator::ZeroRotator, spawnParams);
		}
	}

	return nullptr;
}

void ALichBoss::ShareTarget(ABaseMonster* Monster)
{
	if (Monster)
	{
		if (auto aiCon = Cast<AMonsterAIController>(Monster->GetController()))
		{
			aiCon->StartBehavior();
			aiCon->GetBlackboardComponent()->SetValueAsObject(
				"Target", Cast<AAIController>(GetController())->GetBlackboardComponent()->GetValueAsObject("Target"));
		}
	}
}

void ALichBoss::SpawnSpell()
{
	FOnTimerEvent TimerEvent;
	TimerEvent.BindDynamic(this, &ALichBoss::OnSpawnSpellTimerEvent);
	if (SpawnSpellTask.IsValid() == false)
	{
		SpawnSpellTask = UGameplayTask_LaunchTimerEvent::LaunchTimerEvent(this, TimerEvent, 0.75f, true);
	}
	SpawnSpellTask->ReadyForActivation();
}

void ALichBoss::OnSpawnSpellTimerEvent()
{
	if (SpawnSpellPoint.IsValidIndex(SpawnSpellCount))
	{
		auto spawnLocation = GetMesh()->GetSocketLocation(SpawnSpellPoint[SpawnSpellCount]);

		FActorSpawnParameters spawnParams;
		spawnParams.Instigator = this;
		spawnParams.Owner = this;
		spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		{
			auto p = GetWorld()->SpawnActor<AProjectileActor>(HomingSpellObject, spawnLocation, GetActorRotation(),
			                                                  spawnParams);
			p->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::KeepWorld, true),
			                     SpawnSpellPoint[SpawnSpellCount]);
			SpawnSpells.AddUnique(p);
			SpawnSpellCount++;
		}
		//스폰 끝
		if (SpawnSpellCount >= SpawnSpellPoint.Num())
		{
			if (SpawnSpellTask.IsValid())
			{
				SpawnSpellTask->EndTask();
			}
			{
				for (auto p : SpawnSpells)
				{
					p->DetachFromActor(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));
				}
			}
			//n초마다 발사
			GetWorldTimerManager().SetTimer(LaunchProjectileTimerHandle, this, &ALichBoss::LaunchProjectileEvent, 0.75f,
			                                true);
		}
	}
}

void ALichBoss::LaunchProjectileEvent()
{
	if (SpawnSpells.Num() > 0)
	{
		if (auto projectile = SpawnSpells.Pop())
		{
			projectile->SetEffect(SpawnSpellEffects);
			projectile->LaunchProjectileWithOption(EProjectileDirection::BlackboardTarget, EProjectileShootType::DelayUntilRotation);
		}
	}
	else
	{
		SpawnSpellCount = 0;
		GetWorldTimerManager().ClearTimer(LaunchProjectileTimerHandle);
	}
}

void ALichBoss::SpawnProjectileFast()
{
	if (ProjectileFastObject)
	{
		FActorSpawnParameters spawnParams;
		spawnParams.Instigator = this;
		spawnParams.Owner = this;
		spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		for (auto socket : SpawnSpellPoint)
		{
			auto loc = GetMesh()->GetSocketLocation(socket);
			if (auto projectile = GetWorld()->SpawnActor<AProjectileActor>(
				ProjectileFastObject, loc, GetActorRotation(), spawnParams))
			{
				projectile->SetEffect(SpawnSpellEffects);
				ProjectileFasts.Add(projectile);
			}
		}

		/*FTimerHandle LaunchTimerHandle;
		const FTimerDelegate LaunchTimerDel = FTimerDelegate::CreateLambda([this]()
		{
			if(const auto bbComp = Cast<AAIController>(GetController())->GetBlackboardComponent())
			{
				if(const auto target = Cast<AActor>(bbComp->GetValueAsObject("Target")))
				{
					const auto direction = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(),target->GetActorLocation()).Vector().ForwardVector;
			
					for(auto p : ProjectileFasts)
					{
						p->LaunchProjectile(direction);
					}

					ProjectileFasts.Empty();
				}
			}
		});
		
		GetWorldTimerManager().SetTimer(LaunchTimerHandle,LaunchTimerDel,3.f,false);*/
	}
}

void ALichBoss::LaunchProjectileFast()
{
	if (GetController())
	{
		if (const auto bbComp = Cast<AAIController>(GetController())->GetBlackboardComponent())
		{
			if (const auto target = Cast<AActor>(bbComp->GetValueAsObject("Target")))
			{
				for (auto p : ProjectileFasts)
				{
					p->SetActorRotation(
						UKismetMathLibrary::FindLookAtRotation(p->GetActorLocation(), target->GetActorLocation()));
					p->LaunchProjectile(p->GetActorForwardVector());
				}

				ProjectileFasts.Empty();
			}
		}
	}
}

void ALichBoss::SpawnCrack()
{
	if (CrackObject)
	{
		auto loc = (GetActorForwardVector() * CrackForward) + GetActorLocation() + CrackOffset;
		FActorSpawnParameters spawnParams;
		spawnParams.Instigator = this;
		spawnParams.Owner = this;
		spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		GetWorld()->SpawnActor<AActor>(CrackObject, loc, FRotator::ZeroRotator, spawnParams);
	}
}
