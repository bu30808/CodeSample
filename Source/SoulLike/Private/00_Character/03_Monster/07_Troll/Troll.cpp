// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/03_Monster/07_Troll/Troll.h"

#include "AIController.h"
#include "00_Character/01_Component/AbilityComponent.h"
#include "96_Library/AbilityHelperLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"

ATroll::ATroll()
{
	WeaponComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponComponent"));
	WeaponComponent->SetupAttachment(GetMesh(),"b_MF_Weapon_R_Socket");

	SET_LDMaxDrawDistance(WeaponComponent,3500.f);
}

UPrimitiveComponent* ATroll::GetPrimitiveComponentForLineTrace_Implementation()
{
	return WeaponComponent;
}

void ATrollBoss::ChangePhaseMaterial(UMaterialInstance* Body, UMaterialInstance* Weapon)
{
	GetMesh()->SetMaterial(0,Body);
	if(WeaponComponent)
	{
		WeaponComponent->SetMaterial(0,Weapon);
	}
}

void ATrollBoss::ApplyPoisonAccToTarget(ABaseCharacter* Target, TSubclassOf<UAbilityEffect> PoisonAccEffect)
{
	if(Target && PoisonAccEffect)
	{

		if(auto aiCon = Target->GetController<AAIController>())
		{
			if(aiCon->GetBlackboardComponent()->GetValueAsBool("bPhaseChanged?"))
			{
				if(!UAbilityHelperLibrary::IsInvincible(Target))
				{
					Target->GetAbilityComponent()->K2_ApplyEffect(PoisonAccEffect,this,FOnEffectExpired());
				}
			}
		}
	}
}
