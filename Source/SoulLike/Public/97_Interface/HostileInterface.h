// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HostileInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UHostileInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 아군이었다가 적대적으로 변할수 있는 대상에게 사용하세요.
 */
class SOULLIKE_API IHostileInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	
	//피격 카운트를 늘립니다.
	void AddHitCount(int32& Count);
	//적대적이 되어야 하나요?
	bool IsBeHostile(const int32& CurHitCount, const int32& MaxHitCount) const;
	//지금 적대적인 상태인가요?
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool IsHostile() const;
	virtual bool IsHostile_Implementation() const{ return false; }
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetHostile(class ABaseCharacter* HostileBy);
	virtual void SetHostile_Implementation(class ABaseCharacter* HostileBy){ }
	
	void ShowGotHitDialog(class ANPCBase* NPC, const AActor* HitBy);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	class UBehaviorTree* GetHostileBehaviorTree();
	virtual UBehaviorTree* GetHostileBehaviorTree_Implementation(){ return nullptr; }

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	class UDialogue* GetGotHitDialog();
	virtual UDialogue* GetGotHitDialog_Implementation(){ return nullptr; }
};
