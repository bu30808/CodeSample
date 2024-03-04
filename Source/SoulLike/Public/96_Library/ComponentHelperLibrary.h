// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ComponentHelperLibrary.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API UComponentHelperLibrary : public UObject
{
	GENERATED_BODY()

public:
	template <typename T>
	static T* CreateActorComponent(UObject* Outer, FName CompName);
};

template <typename T>
T* UComponentHelperLibrary::CreateActorComponent(UObject* Outer, FName CompName)
{
	// 액터에 커스텀 액터 컴포넌트 생성
	if (auto newComp = NewObject<T>(Outer, CompName))
	{
		newComp->RegisterComponent();
		return newComp;
	}

	return nullptr;
}
