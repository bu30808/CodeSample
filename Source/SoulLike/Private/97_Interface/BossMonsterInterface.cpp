// Fill out your copyright notice in the Description page of Project Settings.


#include "97_Interface/BossMonsterInterface.h"

#include "00_Character/00_Player/PlayerCharacter.h"
#include "00_Character/00_Player/00_Controller/UserController.h"
#include "00_Character/00_Player/00_Controller/00_Component/WidgetManagerComponent.h"
#include "03_Widget/MainWidget.h"
#include "94_Task/GameplayTask_LaunchTimerEvent.h"
#include "Blueprint/UserWidget.h"
#include "Components/AudioComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Logging/StructuredLog.h"

// Add default functionality here for any IBossMonsterInterface functions that are not pure virtual.
void IBossMonsterInterface::ShowBossWidget_Implementation(class ABaseMonster* Boss, AActor* PlayerCharacter)
{
	UE_LOGFMT(LogTemp, Warning, "보스 위젯을 추가합니다.");
	if (Boss)
	{
		if (auto widgetComp = Boss->GetHealthBarWidgetComponent())
		{
			widgetComp->DestroyComponent();
		}
	}

	if (PlayerCharacter)
	{
		if (auto player = Cast<APlayerCharacter>(PlayerCharacter))
		{
			if (auto pc = player->GetController<AUserController>())
			{
				if (auto mainWidget = pc->GetWidgetManagerComponent()->GetWidget(
					FGameplayTag::RequestGameplayTag("Widget.Main")))
				{
					Cast<UMainWidget>(mainWidget)->ShowBossWidget(Boss);
				}
			}
		}
	}
}

void IBossMonsterInterface::RemoveBossWidget_Implementation(ABaseMonster* Boss, AActor* PlayerCharacter)
{
	if (PlayerCharacter == nullptr)
	{
		PlayerCharacter = UGameplayStatics::GetPlayerCharacter(Boss, 0);
	}

	if (auto player = Cast<APlayerCharacter>(PlayerCharacter))
	{
		if (auto pc = player->GetController<AUserController>())
		{
			if (auto mainWidget = pc->GetWidgetManagerComponent()->GetWidget(
				FGameplayTag::RequestGameplayTag("Widget.Main")))
			{
				Cast<UMainWidget>(mainWidget)->RemoveBossWidget(Boss);
			}
		}
	}
}
