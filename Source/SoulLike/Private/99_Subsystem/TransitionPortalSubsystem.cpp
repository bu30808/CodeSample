// Fill out your copyright notice in the Description page of Project Settings.


#include "99_Subsystem/TransitionPortalSubsystem.h"

#include "Components/WorldPartitionStreamingSourceComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Logging/StructuredLog.h"


void UTransitionPortalSubsystem::Init()
{
	TArray<AActor*> portals;
	UGameplayStatics::GetAllActorsOfClass(this,ATransition_PortalActor::StaticClass(),portals);
	
	for(auto iter : portals)
	{
		if(auto p = Cast<ATransition_PortalActor>(iter))
		{
			switch (p->TransitionPortalType) {
			case ETransitionPortalType::ENTRANCE_TO_TRANSITION_ZONE:
				break;
			case ETransitionPortalType::EXIT_TO_ORIGINAL_ZONE:
				InPortal = p;
				break;
			case ETransitionPortalType::EXIT_TO_ANOTHER_ZONE:
				OutPortal = p;
				break;
			case ETransitionPortalType::ENTRANCE_FROM_ANOTHER_ZONE:
				break;
			}
		}
	}
	
}

UTransitionPortalSubsystem* UTransitionPortalSubsystem::GetSubsystem(UObject* WorldContext)
{
	if(WorldContext)
	{
		if(auto world = WorldContext->GetWorld())
		{
			return UGameplayStatics::GetGameInstance(world)->GetSubsystem<UTransitionPortalSubsystem>();
		}
	}

	return nullptr;
}

void UTransitionPortalSubsystem::UpdateLink(ATransition_PortalActor* PortalActor)
{

	if(PortalActor == nullptr)
	{
		UE_LOGFMT(LogTemp,Error,"링크를 업데이트할 포탈이 널입니다.");
		return;
	}
	
	switch (PortalActor->TransitionPortalType) {
	case ETransitionPortalType::ENTRANCE_TO_TRANSITION_ZONE:

		if(PortalActor->DestinationPortalActor==nullptr)
		{
			UE_LOGFMT(LogTemp,Error,"목적지가 설정되지 않았습니다.");
			return;
		}
		
		
		UE_LOGFMT(LogTemp,Log,"포탈 목적지 설정");
		InPortal->TransitionPortalType = ETransitionPortalType::EXIT_TO_ORIGINAL_ZONE;
		OutPortal->TransitionPortalType = ETransitionPortalType::EXIT_TO_ANOTHER_ZONE;
	
		
		InPortal->OverrideSkyAndWeatherCurValue();
		Destination = PortalActor->DestinationPortalActor;
		PortalActor->SetLinkedPortal(InPortal);
		Origin = PortalActor;
		Destination->StreamingSourceComponent->SetActive(false);
		break;
	case ETransitionPortalType::EXIT_TO_ORIGINAL_ZONE:
		if(Origin)
		{
			UE_LOGFMT(LogTemp,Log,"포탈 되돌아갈 위치 설정");
			Destination->StreamingSourceComponent->SetActive(false);
			Origin->StreamingSourceComponent->SetActive(true);
			PortalActor->SetLinkedPortal(Origin);
		}
		break;
	case ETransitionPortalType::EXIT_TO_ANOTHER_ZONE:
		if(Destination)
		{
			UE_LOGFMT(LogTemp,Log,"포탈 이동할 목적 위치 설정");
			Destination->StreamingSourceComponent->SetActive(true);
			Origin->StreamingSourceComponent->SetActive(false);
			PortalActor->SetLinkedPortal(Destination);
		}
		break;
	case ETransitionPortalType::ENTRANCE_FROM_ANOTHER_ZONE:
		UE_LOGFMT(LogTemp,Log,"포탈 목적지 설정");
		InPortal->TransitionPortalType = ETransitionPortalType::EXIT_TO_ANOTHER_ZONE;
		OutPortal->TransitionPortalType = ETransitionPortalType::EXIT_TO_ORIGINAL_ZONE;
		InPortal->OverrideSkyAndWeatherCurValue();
	
		
		Destination = PortalActor->DestinationPortalActor;
	
		PortalActor->SetLinkedPortal(OutPortal);
		Origin = PortalActor;
		Destination->StreamingSourceComponent->SetActive(false);
		break;
	}
}


void UTransitionPortalSubsystem::LinkToInPortal(ATransition_PortalActor* PortalActor)
{
	if(PortalActor)
	{
		PortalActor->LinkedPortal = InPortal;
	}
}

void UTransitionPortalSubsystem::LinkToExitPortalWithDestinationPortal(APortalActor* DestinationPortal)
{
	if(DestinationPortal)
	{
		DestinationPortal->LinkedPortal = OutPortal;
	}
}

