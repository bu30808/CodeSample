// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "04_Item/00_Consume/ConsumeItemActor.h"
#include "PotionItemActor.generated.h"

UCLASS()
class SOULLIKE_API APotionItemActor : public AConsumeItemActor
{
	GENERATED_BODY()

	friend class UGameLoadHandler;

public:
	// Sets default values for this actor's properties
	APotionItemActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//포션 아이템 정보를 직접 가지고 있습니다.
	//이것으로 강화당 최대 갯수를 조정할 수 있습니다.
	//또한 강화수치도 같이 가지고 있습니다.
	UPROPERTY(VisibleAnywhere)
	FPotionInformation PotionItemInformation;

	virtual const FItemInformation* GetItemInformation() override;
	void GetPotionInformation();
	virtual void PostInitializeComponents() override;

public:
	FText GetEnhancedDescription();

	//현 강화 수치를 가져옴
	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetEnhancementValue();

	const FEnhancement& GetPotionEnhancement() { return PotionItemInformation.Enhancement; }
	void IncreaseEnhance();
};
