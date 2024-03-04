// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerCharacter.h"
#include "GameFramework/Actor.h"
#include "SoulTomb.generated.h"

UCLASS()
class SOULLIKE_API ASoulTomb : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASoulTomb();

protected:
	//사망해서 드롭한 경험치가 저장될 변수입니다.
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite)
	float EXP;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	class UBoxComponent* BoxComponent;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	class UNiagaraComponent* NiagaraComponent;
	

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:
	void SetEXP(float exp){EXP = exp;}

	//플레이어 캐릭터에게 EXP를 되돌려줍니다.
	void Absorption(class APlayerCharacter* PlayerCharacter);
};
