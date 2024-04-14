// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/00_Player/01_Component/TeleportBonfireComponent.h"

#include "00_Character/00_Player/PlayerCharacter.h"
#include "03_Widget/MainWidget.h"
#include "96_Library/SaveGameHelperLibrary.h"
#include "98_GameInstance/SoulLikeInstance.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"


// Sets default values for this component's properties
UTeleportBonfireComponent::UTeleportBonfireComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UTeleportBonfireComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	ComponentOwnerCharacter = GetOwner<APlayerCharacter>();
	
}

void UTeleportBonfireComponent::PlayTeleportEndMontage()
{
	if(ComponentOwnerCharacter){
		ComponentOwnerCharacter->PlayAnimMontage(TeleportEndMontage);
	}
}

void UTeleportBonfireComponent::StartTeleportToOtherBonfire()
{
	if(ComponentOwnerCharacter)
	{
		if (auto instance = GetWorld()->GetGameInstance<USoulLikeInstance>())
		{
			ComponentOwnerCharacter->SetActorTickEnabled(false);
			ComponentOwnerCharacter->GetCapsuleComponent()->SetEnableGravity(false);
		
			ComponentOwnerCharacter->GetMainWidget()->SetVisibility(ESlateVisibility::Collapsed);
			/*auto cameraManager = UGameplayStatics::GetPlayerCameraManager(this, 0);
			cameraManager->StartCameraFade(0,1,2.f,FColor::Black,true,true);*/
			instance->OnFinishLoadGame.AddUniqueDynamic(ComponentOwnerCharacter, &APlayerCharacter::OnFinishLoadGame);
			instance->OnFinishLoadGame.AddUniqueDynamic(this,&UTeleportBonfireComponent::PlayTeleportEndMontage);
			instance->TeleportToOtherBonfireLoad();
		}
	}
}

void UTeleportBonfireComponent::TeleportToOtherBonfire(const FBonfireInformation& TeleportBonfireInforation)
{
	if(ComponentOwnerCharacter)
	{
		if(auto interactionComp = ComponentOwnerCharacter->GetInteractionWidgetComponent()){
			if (interactionComp->IsValidLowLevel() && interactionComp->IsVisible())
			{
				interactionComp->SetVisibility(false);
			}

			ComponentOwnerCharacter->DisableInput(Cast<APlayerController>(ComponentOwnerCharacter->GetController()));
			TeleportOtherBonfireInformation = TeleportBonfireInforation;
			auto cameraManager = UGameplayStatics::GetPlayerCameraManager(this, 0);
			cameraManager->StartCameraFade(0,1,2.f,FColor::Black,true,true);
	
			//아니라면 몽타주가 끝나고 레벨을 오픈합니다.
			ComponentOwnerCharacter->GetMesh()->GetAnimInstance()->OnMontageBlendingOut.AddUniqueDynamic(
				this, &UTeleportBonfireComponent::OnEndPlayTeleportMontage);
			ComponentOwnerCharacter->GetMesh()->GetAnimInstance()->Montage_Play(TeleportMontage);
		}
	}
}

void UTeleportBonfireComponent::OnEndPlayTeleportMontage(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == TeleportMontage)
	{
		if(USaveGameHelperLibrary::IsSameLevel(this,TeleportOtherBonfireInformation.LevelName) == false)
		{
			UGameplayStatics::OpenLevel(GetWorld(), FName(TeleportOtherBonfireInformation.LevelName));
		}else
		{
			StartTeleportToOtherBonfire();
		}
		
	}
}


