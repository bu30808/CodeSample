// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AttributeHelperLibrary.generated.h"

enum class EStatusEffect : uint8;
enum class EAttributePointType : uint8;
enum class EAttributeType : uint8;
/**
 * 
 */
UCLASS()
class SOULLIKE_API UAttributeHelperLibrary : public UObject
{
	GENERATED_BODY()

public:
	static EAttributeType ConvertAttributePointTypeToAttributeType(EAttributePointType PointType);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static class UAttributeProcessSubsystem* GetAttributeProcessSubsystem(UObject* WorldContext);

	UFUNCTION(BlueprintCallable,BlueprintPure)
	static EStatusEffect AttributeTypeToStatusEffect(EAttributeType Type);

	//어빌리티 이팩트 내부 어트리뷰트 이팩트를 상태이상 UI 업데이트용 정보 데이터로 변경합니다.
	UFUNCTION(BlueprintCallable)
	static void AttributeEffectElementToStatusEffectValues(class UAttributeComponent* AttributeComponent, EAttributeType AttributeType, EStatusEffect& StatusEffect, float& AccValue, float& ResistValue);
};
