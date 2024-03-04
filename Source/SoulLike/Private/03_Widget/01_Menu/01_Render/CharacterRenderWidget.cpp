// Fill out your copyright notice in the Description page of Project Settings.


#include "03_Widget/01_Menu/01_Render/CharacterRenderWidget.h"

#include "00_Character/00_Player/PlayerCharacter.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Kismet/KismetInputLibrary.h"
#include "Logging/StructuredLog.h"

void UCharacterRenderWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Player = GetOwningPlayerPawn<APlayerCharacter>();

	Button_Down->OnPressed.AddUniqueDynamic(this, &UCharacterRenderWidget::OnPressedDownButton);
	Button_Up->OnPressed.AddUniqueDynamic(this, &UCharacterRenderWidget::OnPressedUpButton);
	Button_Left->OnPressed.AddUniqueDynamic(this, &UCharacterRenderWidget::OnPressedLeftButton);
	Button_Right->OnPressed.AddUniqueDynamic(this, &UCharacterRenderWidget::OnPressedRightButton);

	Button_Down->OnReleased.AddUniqueDynamic(this, &UCharacterRenderWidget::OnReleasedDownButton);
	Button_Up->OnReleased.AddUniqueDynamic(this, &UCharacterRenderWidget::OnReleasedUpButton);
	Button_Left->OnReleased.AddUniqueDynamic(this, &UCharacterRenderWidget::OnReleasedLeftButton);
	Button_Right->OnReleased.AddUniqueDynamic(this, &UCharacterRenderWidget::OnReleasedRightButton);


	Button_Reset->OnClicked.AddUniqueDynamic(this, &UCharacterRenderWidget::OnClickedResetButton);
}

FReply UCharacterRenderWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	bPressed = true;
	StartLoc = UKismetInputLibrary::PointerEvent_GetScreenSpacePosition(InMouseEvent);

	return FReply::Handled();
}

FReply UCharacterRenderWidget::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (bPressed)
	{
		if (Player.IsValid())
		{
			auto delta = StartLoc - UKismetInputLibrary::PointerEvent_GetScreenSpacePosition(InMouseEvent);

			//왼쪽
			if (delta.X > 0)
			{
				Player->GetSceneCaptureCameraBoom()->AddRelativeRotation(FRotator(0, -1.5, 0));
			}
			//오른쪽
			else if (delta.X < 0)
			{
				Player->GetSceneCaptureCameraBoom()->AddRelativeRotation(FRotator(0, 1.5, 0));
			}

			if (delta.Y > 0)
			{
				if (Player->GetSceneCaptureCameraBoom()->GetRelativeRotation().Pitch > PitchMax * (-1))
				{
					Player->GetSceneCaptureCameraBoom()->AddRelativeRotation(FRotator(-1.5, 0, 0));
				}
			}
			else if (delta.Y < 0)
			{
				if (Player->GetSceneCaptureCameraBoom()->GetRelativeRotation().Pitch < PitchMax)
				{
					Player->GetSceneCaptureCameraBoom()->AddRelativeRotation(FRotator(1.5, 0, 0));
				}
			}
		}
	}
	return FReply::Handled();
}

FReply UCharacterRenderWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	bPressed = false;
	return FReply::Handled();
}

void UCharacterRenderWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (bPressedDown)
	{
		if (Player->GetSceneCaptureCameraBoom()->GetRelativeRotation().Pitch < PitchMax)
		{
			Player->GetSceneCaptureCameraBoom()->AddRelativeRotation(FRotator(1.5, 0, 0));
		}
	}

	if (bPressedLeft)
	{
		Player->GetSceneCaptureCameraBoom()->AddRelativeRotation(FRotator(0, -1.5, 0));
	}

	if (bPressedRight)
	{
		Player->GetSceneCaptureCameraBoom()->AddRelativeRotation(FRotator(0, 1.5, 0));
	}

	if (bPressedUp)
	{
		if (Player->GetSceneCaptureCameraBoom()->GetRelativeRotation().Pitch > PitchMax * (-1))
		{
			Player->GetSceneCaptureCameraBoom()->AddRelativeRotation(FRotator(-1.5, 0, 0));
		}
	}
}

void UCharacterRenderWidget::OnClickedResetButton()
{
	Init();
}

void UCharacterRenderWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	if(Image_PlayerRenderTarget)
	{
		Image_PlayerRenderTarget->SetIsEnabled(false);
	}
}


void UCharacterRenderWidget::OnReleasedDownButton()
{
	bPressedDown = false;
}

void UCharacterRenderWidget::OnReleasedUpButton()
{
	bPressedUp = false;
}

void UCharacterRenderWidget::OnReleasedLeftButton()
{
	bPressedLeft = false;
}

void UCharacterRenderWidget::OnReleasedRightButton()
{
	bPressedRight = false;
}

void UCharacterRenderWidget::OnPressedRightButton()
{
	bPressedRight = true;
}

void UCharacterRenderWidget::OnPressedLeftButton()
{
	bPressedLeft = true;
}

void UCharacterRenderWidget::OnPressedUpButton()
{
	bPressedUp = true;
}

void UCharacterRenderWidget::OnPressedDownButton()
{
	bPressedDown = true;
}

void UCharacterRenderWidget::Init()
{
	Player->ResetSceneCaptureCamera();
	/*Player->GetSceneCaptureCameraBoom()->SetRelativeRotation(FRotator(5.f,140.f,0.f));
	Player->GetSceneCaptureCameraBoom()->SetRelativeLocation(FVector(-4.516228f,-5.727651f,52.221545f));*/
}
