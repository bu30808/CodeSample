// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/01_Component/UROComponent.h"


// Sets default values for this component's properties
UUROComponent::UUROComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UUROComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

	if (m_pLOD_FrameRate.Num() <= 0) return;

	UActorComponent* pCpt = GetOwner()->GetComponentByClass(USkinnedMeshComponent::StaticClass());
	if (pCpt != nullptr)
	{
		USkinnedMeshComponent* pSKMesh = Cast<USkinnedMeshComponent>(pCpt);
		pSKMesh->bEnableUpdateRateOptimizations = true;
		pSKMesh->AnimUpdateRateParams->bShouldUseLodMap = true;

		for (int i = 0; i < m_pLOD_FrameRate.Num(); i++)
		{
			pSKMesh->AnimUpdateRateParams->LODToFrameSkipMap.Add(i, m_pLOD_FrameRate[i]);
		}


	}
	
}


