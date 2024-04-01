// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/03_Monster/04_Wolf/WolfBoss.h"

#include "AIController.h"
#include "00_Character/03_Monster/00_Controller/MonsterAIController.h"
#include "00_Character/03_Monster/04_Wolf/Wolf.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Engine/TargetPoint.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "00_Character/03_Monster/04_Wolf/Wolf.h"
#include "02_Ability/01_Task/GameplayTask_LaunchEvent.h"
#include "91_Sky/DynamicSkyActor.h"
#include "Engine/DirectionalLight.h"

AWolfBoss::AWolfBoss()
{
	if (MonsterDataAsset)
	{
		MonsterDataAsset->bRespawnWhenPlayerRest = false;
	}
}

void AWolfBoss::CreateDynamicMaterials()
{
	if (MaterialInterfaces.IsValidIndex(0))
	{
		//눈
		MaterialInstances.Emplace(GetMesh()->CreateDynamicMaterialInstance(2, MaterialInterfaces[0]));
	}
	if (MaterialInterfaces.IsValidIndex(1))
	{
		//털
		MaterialInstances.Emplace(GetMesh()->CreateDynamicMaterialInstance(3, MaterialInterfaces[1]));
	}
	if (MaterialInterfaces.IsValidIndex(2))
	{
		//몸
		MaterialInstances.Emplace(GetMesh()->CreateDynamicMaterialInstance(4, MaterialInterfaces[2]));
	}
}

void AWolfBoss::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	CreateDynamicMaterials();
}

void AWolfBoss::BeginPlay()
{
	Super::BeginPlay();

	if (auto actor = UGameplayStatics::GetActorOfClass(this, ADynamicSkyActor::StaticClass()))
	{
		Sky = Cast<ADynamicSkyActor>(actor);
		OriginalTime = Sky->CurrentTime;
	}
}


void AWolfBoss::ChangePhase2ColorSet(float Alpha)
{
	for (auto iter : MaterialInstances)
	{
		Phase2ColorSet.A = FMath::Lerp(0, 1, Alpha);
		iter->SetVectorParameterValue("Emision_Color", Phase2ColorSet);
		iter->SetScalarParameterValue("Emission Power", FMath::Lerp(0, Phase2EmissivePower, Alpha));
	}
}

void AWolfBoss::ChangeSkyTime(float Time, float Alpha)
{
	if (Sky)
	{
		Sky->CurrentTime = FMath::Lerp(OriginalTime, Time, Alpha);
	}
}
