// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "JsonDataUtility.generated.h"
/**
 * 
 */
USTRUCT(BlueprintType)
struct FSaveData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = jsondata)
	FString Name;

	// UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = jsondata)
	// FVector Size;

	// UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = jsondata)
	// int32 Price;
};

class SOULLIKE_API JsonDataUtility
{
public:
	JsonDataUtility();
	~JsonDataUtility();

	static bool SaveToJson(const FString& filepath, const FSaveData& data);
	static bool LoadFromJson(const FString& filepath, FSaveData& Data);
};
