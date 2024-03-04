// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/03_Monster/05_Lich/LichPullProjectile.h"

#include "NiagaraComponent.h"
#include "02_Ability/01_Task/GameplayTask_LaunchEvent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

ALichPullProjectile::ALichPullProjectile()
{
	ProjectileParticle = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ProjectileParticle"));
	ProjectileParticle->SetupAttachment(RootComponent);
	TrailParticle = CreateDefaultSubobject<UNiagaraComponent>(TEXT("TrailParticle"));
	TrailParticle->SetupAttachment(ProjectileParticle);
	TrailParticle->Deactivate();
}

void ALichPullProjectile::OnTaskTickEvent()
{
	TrailParticle->SetVectorParameter("StartTrail", GetOwner()->GetActorLocation());
	TrailParticle->SetVectorParameter("EndTrail", Target->GetActorLocation());

	FVector ownerLocation = GetOwner()->GetActorLocation();
	FVector targetLocation = Target->GetActorLocation();
	auto power = UKismetMathLibrary::FindLookAtRotation(targetLocation, ownerLocation).Vector() * PullForce;
	Target->GetCharacterMovement()->AddImpulse(power);

	if (UKismetMathLibrary::EqualEqual_VectorVector(ownerLocation, targetLocation, 225.f))
	{
		Target->GetCharacterMovement()->StopMovementImmediately();
		Task->EndTask();
		Target->EnableInput(Cast<APlayerController>(Target->GetController()));
		Destroy();
	}
}

void ALichPullProjectile::OnHiEvent(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor != nullptr)
	{
		ProjectileMovementComponent->Deactivate();

		if (OtherActor->IsA<ABaseCharacter>())
		{
			Target = Cast<ABaseCharacter>(OtherActor);

			SphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			ProjectileParticle->Deactivate();
			TrailParticle->Activate();

			if (!Task.IsValid())
			{
				Task = UGameplayTask_LaunchEvent::LaunchEvent(GetOwner());
			}

			Task->OnTaskTick.AddUniqueDynamic(this, &ALichPullProjectile::OnTaskTickEvent);
			Task->ReadyForActivation();
			Target->DisableInput(Cast<APlayerController>(Target->GetController()));
			Target->GetCharacterMovement()->StopMovementImmediately();
			ApplyEffects(Target.Get(), Hit.Location);
		}
		else
		{
			auto cueInfo = CueInformation;
			cueInfo.AbilityCueType = EAbilityCueType::AtLocation;
			cueInfo.SpawnLocation = Hit.Location;

			GetOwner<ABaseCharacter>()->GetAbilityComponent()->ApplyCue(cueInfo);
			Destroy();
		}
	}
}

void ALichPullProjectile::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	OnActorHit.AddUniqueDynamic(this, &ALichPullProjectile::OnHiEvent);
}
