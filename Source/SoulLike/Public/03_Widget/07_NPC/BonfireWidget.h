// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "03_Widget/07_NPC/NPCWidget.h"
#include "BonfireWidget.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API UBonfireWidget : public UNPCWidget
{
	GENERATED_BODY()
	
	virtual void NativeConstruct() override;
};
