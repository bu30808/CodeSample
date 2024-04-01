// Fill out your copyright notice in the Description page of Project Settings.


#include "02_Ability/02_Projectile/ProjectileActor.h"

#include "00_Character/BaseCharacter.h"
#include "00_Character/01_Component/AbilityComponent.h"
#include "00_Character/02_Animation/00_NotifyState/AnimNotifyState_SpawnProjectile.h"
#include "00_Character/03_Monster/00_Controller/MonsterAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Logging/StructuredLog.h"

// Sets default values
AProjectileActor::AProjectileActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent->SetCollisionProfileName("Projectile");
	RootComponent = SphereComponent;

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComponent"));
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->ProjectileGravityScale = 0.f;
	ProjectileMovementComponent->SetAutoActivate(false);
}

// Called when the game starts or when spawned
void AProjectileActor::BeginPlay()
{
	Super::BeginPlay();
}

void AProjectileActor::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

// Called every frame
void AProjectileActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	if (CurProjectileShootType == EProjectileShootType::DelayUntilRotation)
	{
		if (bRotationComplete == false)
		{
			SetActorRotation(
				FMath::RInterpTo(GetActorRotation(),
				                 TargetRotation,
				                 DeltaTime,
				                 InterpSpeed
				));

			if (GetActorRotation().Equals(TargetRotation, 1.f))
			{
				ShootSetting(CurProjectileDirectionType);
				bRotationComplete = true;
				ProjectileMovementComponent->SetActive(true);
			}
		}
	}
}

void AProjectileActor::LaunchProjectile(const FVector& ForwardVector)
{
	ProjectileMovementComponent->Velocity = ForwardVector * ProjectileSpeed;
	ProjectileMovementComponent->SetActive(true);
	SetActorTickEnabled(true);
}

void AProjectileActor::ShootSetting(EProjectileDirection ProjectileDirection)
{
	switch (ProjectileDirection)
	{
	case EProjectileDirection::Forward:
		SetActorRotation(FRotationMatrix::MakeFromX(GetOwner()->GetActorForwardVector()).Rotator());
		ProjectileMovementComponent->Velocity = GetOwner()->GetActorForwardVector() * ProjectileSpeed;
		break;
	case EProjectileDirection::BlackboardTarget:
		{
			if (auto aiCon = Cast<ACharacter>(GetOwner())->GetController<AMonsterAIController>())
			{
				if (auto blackboard = aiCon->GetBlackboardComponent())
				{
					if (auto target = Cast<AActor>(blackboard->GetValueAsObject("Target")))
					{
						const FRotator& lookRot = UKismetMathLibrary::FindLookAtRotation(
							GetActorLocation(), target->GetActorLocation());
						SetActorRotation(lookRot);
						ProjectileMovementComponent->Velocity = lookRot.Vector() * ProjectileSpeed;
					}
				}
			}
		}
		break;
	default: ;
	}
}

void AProjectileActor::LaunchProjectileWithOption_Implementation(
	EProjectileDirection P_Direction, EProjectileShootType P_ShootType)
{
	CurProjectileDirectionType = P_Direction;
	CurProjectileShootType = P_ShootType;

	if (auto owner = GetOwner<ABaseCharacter>())
	{
		switch (P_ShootType)
		{
		case EProjectileShootType::Immediately:
		case EProjectileShootType::ImmediatelyButNotRotationFollowsVelocity:
			ShootSetting(P_Direction);
			break;
		case EProjectileShootType::DelayUntilRotation:
			break;
		}


		switch (P_ShootType)
		{
		case EProjectileShootType::Immediately:
			ProjectileMovementComponent->bRotationFollowsVelocity = false;
			SetActorTickEnabled(true);
			ProjectileMovementComponent->SetActive(true);
			break;
		case EProjectileShootType::ImmediatelyButNotRotationFollowsVelocity:
			ProjectileMovementComponent->bRotationFollowsVelocity = false;
			SetActorTickEnabled(true);
			ProjectileMovementComponent->SetActive(true);
			break;
		case EProjectileShootType::DelayUntilRotation:
			ProjectileMovementComponent->bRotationFollowsVelocity = true;
			if (CurProjectileDirectionType == EProjectileDirection::Forward)
			{
				TargetRotation = UKismetMathLibrary::FindLookAtRotation(
					GetActorLocation(), GetOwner()->GetActorForwardVector() + GetOwner()->GetActorLocation());
			}
			else
			{
				if (auto aiCon = owner->GetController<AMonsterAIController>())
				{
					if (auto blackboard = aiCon->GetBlackboardComponent())
					{
						if (auto target = Cast<AActor>(blackboard->GetValueAsObject("Target")))
						{
							TargetRotation = UKismetMathLibrary::FindLookAtRotation(
								GetActorLocation(), target->GetActorLocation());
						}
					}
				}
			}
			SetActorTickEnabled(true);
			break;
		default: ;
		}
	}
}

void AProjectileActor::LaunchProjectileDelayWithOption(float Time, EProjectileDirection P_Direction,
                                                       EProjectileShootType P_ShootType)
{
	if (!GetWorldTimerManager().TimerExists(DelayLaunchTimerHandle))
	{
		FTimerDelegate timerDel = FTimerDelegate::CreateUObject(this, &AProjectileActor::LaunchProjectileWithOption,
		                                                        P_Direction, P_ShootType);
		GetWorldTimerManager().SetTimer(DelayLaunchTimerHandle, timerDel, Time, false);
	}
}

void AProjectileActor::SetEffect(const TArray<TSubclassOf<UAbilityEffect>>& Effects)
{
	ProjectileEffects = Effects;
}


void AProjectileActor::ApplyEffects(ABaseCharacter* HitTarget, FVector HitLocation)
{
	if (HitTarget && GetOwner() != nullptr)
	{
		if (const auto abComp = HitTarget->GetAbilityComponent())
		{
			auto addInfo = NewObject<UAbilityCueAdditionalInformation>(this);
			addInfo->HitLocation = HitLocation;

			abComp->K2_ApplyEffectsWithReturn(ProjectileEffects, GetOwner(), addInfo);
		}
	}
}

void AProjectileActor::ProcessHit(const TArray<FHitResult>& Hits, bool bManualDestroy)
{
	for (auto iter : Hits)
	{
		if (iter.GetActor() != GetOwner())
		{
			if (iter.GetActor()->IsA<ABaseCharacter>())
			{
				ApplyEffects(Cast<ABaseCharacter>(iter.GetActor()), iter.Location);
			}
			else
			{
				if (GetOwner() != nullptr && GetOwner()->IsA<ABaseCharacter>())
				{
					if (VisibilityHitCue.IsValid())
					{
						VisibilityHitCue.SpawnLocation = iter.Location;
						GetOwner<ABaseCharacter>()->GetAbilityComponent()->ApplyCue(VisibilityHitCue);
					}
				}
			}

			if (!bManualDestroy)
			{
				SetActorTickEnabled(false);
				Destroy();
			}
		}
	}
}
