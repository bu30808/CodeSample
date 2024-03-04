// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/02_Animation/01_Notify/AnimNotify_ChangeMaterialParam.h"

UAnimNotify_ChangeMaterialParam::UAnimNotify_ChangeMaterialParam()
{
#if WITH_EDITOR
	bShouldFireInEditor = false;
#endif
}

void UAnimNotify_ChangeMaterialParam::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                             const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);


	if (MeshComp)
	{
		auto mat = MeshComp->GetMaterials();
		for (auto i = 0; i < mat.Num(); i++)
		{
			auto instance = MeshComp->CreateAndSetMaterialInstanceDynamicFromMaterial(i, mat[i]);
			instance->SetScalarParameterValue(ParamName, Value);
		}
	}
}
