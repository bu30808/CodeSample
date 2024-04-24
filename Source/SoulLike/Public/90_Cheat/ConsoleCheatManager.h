// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ConsoleCheatManager.generated.h"

/*
TAutoConsoleVariable<int32> CVar_TestIntVar(
	TEXT("c.testintvar"),
	0,
	TEXT("테스트용 int 변수"),
	ECVF_Default);

FAutoConsoleCommand CCmd_HelloWorld(
	TEXT("c.helloworld"),
	TEXT("HelloWorld를 띄워보자"),
	FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Params)
		{
			UE_LOG(LogTemp, Log, TEXT("Hello World!! TestIntVar : [%d]"), CVar_TestIntVar.GetValueOnAnyThread());
		}),
	ECVF_Default);
	*/


/**
 * 
 */
UCLASS()
class SOULLIKE_API UConsoleCheatManager : public UObject
{
	GENERATED_BODY()

public:
};
