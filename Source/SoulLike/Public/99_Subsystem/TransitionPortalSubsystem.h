// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "92_Tools/PortalActor.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "TransitionPortalSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSetInPortal,class APortalActor*,InPortal);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSetOutPortal,class APortalActor*,InPortal);
/**
 * 
 */
UCLASS()
class SOULLIKE_API UTransitionPortalSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

protected:
	UPROPERTY(Transient)
	TObjectPtr<class ATransition_PortalActor> InPortal;
	UPROPERTY(Transient)
	TObjectPtr<class ATransition_PortalActor> OutPortal;
	UPROPERTY(Transient)
	TObjectPtr<ATransition_PortalActor> Destination;
	UPROPERTY(Transient)
	TObjectPtr<ATransition_PortalActor> Origin;
public:
	UPROPERTY()
	FOnSetInPortal OnSetInPortal;
	UPROPERTY()
	FOnSetOutPortal OnSetOutPortal;
	
	void Init();
	static UTransitionPortalSubsystem* GetSubsystem(UObject* WorldContext);
	
	
	void UpdateLink(ATransition_PortalActor* PortalActor);
	void SetInPortal(class ATransition_PortalActor* Transition_PortalActor)
	{

		UE_LOG(LogTemp,Log,TEXT("들어오는 포탈 설정됨."));
		
		InPortal = Transition_PortalActor; OnSetInPortal.Broadcast(InPortal);
	}
	void SetOutPortal(class ATransition_PortalActor* Transition_PortalActor){OutPortal = Transition_PortalActor; OnSetOutPortal.Broadcast(OutPortal);}
	void LinkToInPortal(class ATransition_PortalActor* PortalActor);
	void LinkToExitPortalWithDestinationPortal(APortalActor* DestinationPortal);
	
	ATransition_PortalActor* GetInPortal(){return InPortal;}
	ATransition_PortalActor* GetOutPortal(){return OutPortal;}
};
