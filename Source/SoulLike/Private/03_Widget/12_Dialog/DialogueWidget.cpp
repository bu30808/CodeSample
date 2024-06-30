// Fill out your copyright notice in the Description page of Project Settings.


#include "03_Widget/12_Dialog/DialogueWidget.h"

#include "00_Character/00_Player/00_Controller/UserController.h"
#include "00_Character/00_Player/00_Controller/00_Component/WidgetManagerComponent.h"
#include "Components/AudioComponent.h"
#include "Components/Button.h"
#include "Components/Overlay.h"
#include "Components/RichTextBlock.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetInputLibrary.h"
#include "Logging/StructuredLog.h"

void UDialogueWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	if(NextText){
		NextText->ColorAndOpacityDelegate.BindDynamic(this,&UDialogueWidget::GetContinueColorAndOpacity);
	}
}

void UDialogueWidget::NativeConstruct()
{
	
	Super::NativeConstruct();
	if(InDialogue)
	{
		InDialogue->AssignPersistentOuter(UGameplayStatics::GetGameInstance(GetOwningPlayer()));
		ToNPCReply(InDialogue->Data[0],true);
		SpeakerName->SetText(InDialogue->Name);
		
		NextButton->OnClicked.AddUniqueDynamic(this,&UDialogueWidget::OnClickedNextButton);
		
	}else
	{
		UE_LOGFMT(LogTemp,Error,"다이얼로그 오브젝트가 유효하지 않습니다.");
	}
	
}

FSlateColor UDialogueWidget::GetContinueColorAndOpacity()
{
	if(CurrentlySelectedReply == 0 || NextButton->IsHovered())
	{
		return FLinearColor::Gray;
	}

	return FLinearColor::White;
}

void UDialogueWidget::OnClickedNextButton()
{
	if(auto pc = GetOwningPlayer<AUserController>())
	{
		if(pc->GetWorldTimerManager().IsTimerActive(ToNPCReplyTimerHandle))
		{
			ToNPCReply(CurrentPlayerNode,true);
			pc->GetWorldTimerManager().ClearTimer(ToNPCReplyTimerHandle);
		}else
		{
			ToNPCReply(CurNPCNode,false);
		}
	}
}

void UDialogueWidget::MouseSelectReply(const UDialogueReplyWidget* DialogueReplyWidget)
{
	for(auto i = 0;i<PlayerVBox->GetChildrenCount()-1;i++)
	{
		if(auto reply = Cast<UDialogueReplyWidget>(PlayerVBox->GetChildAt(i)))
		{
			if(reply == DialogueReplyWidget)
			{
				CurrentlySelectedReply = i;
				break;
			}
		}
	}

	HightlightSelectedReply();
}

void UDialogueWidget::PlayPlayerReply(const FDialogueNode& PlayerNode)
{
	CurrentPlayerNode = PlayerNode;
	NextButton->SetVisibility(ESlateVisibility::Visible);
	
	if(PlayerNode.Sound)
	{
		PlayDialogueSound(PlayerNode.Sound);
		if(PlayerNode.Sound->Duration >= 0)
		{
			FTimerDelegate toNpcReplyTimerDel = 	FTimerDelegate::CreateUFunction(this,"ToNPCReply",PlayerNode,true);
			GetOwningPlayer()->GetWorldTimerManager().SetTimer(ToNPCReplyTimerHandle,toNpcReplyTimerDel,PlayerNode.Sound->Duration,false);
			PlayerVBox->ClearChildren();
			ShowContinueButtonWithText(FText::FromString(TEXT("다음")));
			NPCText->SetText(ReplaceVarStrings(PlayerNode.Text));
		}
		
	}else
	{
		ToNPCReply(PlayerNode,true);
	}
}

void UDialogueWidget::ToNPCReply(const FDialogueNode& PlayerReply, bool bFireEvents)
{
	bool bFoundValid = false;

	if(bFireEvents)
	{
		LaunchEvents(PlayerReply);
	}

	const auto& nextNodes = InDialogue->GetNextNodes(PlayerReply);

	for(const auto& iter : nextNodes)
	{
		if(CheckConditions(iter))
		{
			DisplayNPCNode(iter);
			bFoundValid = true;
			break;
		}
	}

	if(!bFoundValid)
	{
		OnDialogueClose.Broadcast(InDialogue,NPCActor);
		GetOwningPlayer<AUserController>()->RemoveFromPopUp(this,true);
		
	}
}

void UDialogueWidget::LaunchEvents(const FDialogueNode& Node)
{
	RunEventsForNode(Node);
}

bool UDialogueWidget::CheckConditions(const FDialogueNode& Node)
{
	return IsConditionsMetForNode(Node);
}

void UDialogueWidget::DisplayNPCNode(const FDialogueNode& NPCNode)
{
	CurNPCNode = NPCNode;
	LaunchEvents(NPCNode);
	PlayDialogueSound(NPCNode.Sound);
	NextButtonContainer->SetVisibility(ESlateVisibility::Collapsed);
	
	PlayerVBox->ClearChildren();
	NPCText->SetText(ReplaceVarStrings(NPCNode.Text));
	const auto& nextNodes = InDialogue->GetNextNodes(NPCNode);

	if(nextNodes.Num() > 0)
	{

		const auto& node = nextNodes[0];
		if(node.isPlayer)
		{
			DisplayReplies(nextNodes);
		}else
		{
			if(node.Text.IsEmpty())
			{
				LaunchEvents(node);
				DisplayReplies(InDialogue->GetNextNodes(node));
				
			}else
			{
				ShowContinueButtonWithText(FText::FromString(TEXT("계속")));
			}
		}
		
	}else
	{
		ShowContinueButtonWithText(FText::FromString(TEXT("대화 종료")));
	}
}

void UDialogueWidget::DisplayReplies(const TArray<FDialogueNode>& Replies)
{
	for(const auto& iter : Replies)
	{
		if(CheckConditions(iter))
		{
			if(auto reply = CreateReplyWidget(iter))
			{
				reply->SetOwnerWidget(this);
				reply->SetReplyNode(iter);
			
				PlayerVBox->AddChildToVerticalBox(reply);

				NextButton->SetVisibility(ESlateVisibility::HitTestInvisible);
				
				CurrentlySelectedReply = 0;
				HightlightSelectedReply();
			}
		}
	}
}

void UDialogueWidget::HightlightSelectedReply()
{
	for(auto i = 0;i<PlayerVBox->GetChildrenCount()-1;i++)
	{
		if(auto reply = Cast<UDialogueReplyWidget>(PlayerVBox->GetChildAt(i)))
		{
			reply->SetSelected(false);
		}
	}

	if(auto reply = Cast<UDialogueReplyWidget>(PlayerVBox->GetChildAt(CurrentlySelectedReply)))
	{
		reply->SetSelected(true);
	}
	
}

void UDialogueWidget::PlayDialogueSound(USoundBase* Sound)
{
	StopDialogueSound();
	if(Sound)
	{
		Sound2DComponent = UGameplayStatics::SpawnSound2D(GetOwningPlayer(),Sound);
		Sound2DComponent->Play();
	}
}

void UDialogueWidget::StopDialogueSound()
{
	if(Sound2DComponent->IsValidLowLevel())
	{
		Sound2DComponent->Stop();
	}
}

void UDialogueWidget::ShowContinueButtonWithText(const FText& Text)
{
	NextText->SetText(Text);
	NextButtonContainer->SetVisibility(ESlateVisibility::Visible);
}

UDialogueReplyWidget* UDialogueWidget::CreateReplyWidget(const FDialogueNode& Node)
{
	if(auto pc = GetOwningPlayer<AUserController>())
	{
		return Cast<UDialogueReplyWidget>(pc->GetWidgetManagerComponent()->AddWidget(FGameplayTag::RequestGameplayTag("Widget.Dialogue.Reply"),1,false,false));
	}

	return nullptr;
}


void UDialogueWidget::JumpToNPCNode(const int32& NodeID)
{
	bool bFound =false;
	const auto& node = InDialogue->GetNodeById(NodeID,bFound);
	if(bFound)
	{
		if(node.isPlayer==false)
		{
			DisplayNPCNode(node);
		}
	}
	
}

void UDialogueWidget::SimulateNextClicked()
{
	NextText->SetColorAndOpacity(FSlateColor(FLinearColor::Gray));
	OnClickedNextButton();
}

void UDialogueWidget::SimulateNextHoverd()
{
	NextText->SetColorAndOpacity(FSlateColor(FLinearColor(FColor::White)));
}

void UDialogueWidget::GamepadReply(const int32& Index)
{
	if(NextButtonContainer->IsVisible())
	{
		SimulateNextClicked();
	}else
	{
		if(auto reply = Cast<UDialogueReplyWidget>(PlayerVBox->GetChildAt(Index)))
		{
			SimulateNextClicked();
		}
		
	}
}

FReply UDialogueWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{

	if(UKismetInputLibrary::GetKey(InKeyEvent) == EKeys::Gamepad_DPad_Up)
	{
		CurrentlySelectedReply = FMath::Clamp(CurrentlySelectedReply-1,0, PlayerVBox->GetChildrenCount()-1);
		HightlightSelectedReply();
		return FReply::Handled();
	}
	
	if(UKismetInputLibrary::GetKey(InKeyEvent) == EKeys::Gamepad_DPad_Down)
	{
		CurrentlySelectedReply = FMath::Clamp(CurrentlySelectedReply+1,0, PlayerVBox->GetChildrenCount()-1);
		HightlightSelectedReply();
		return FReply::Handled();
	}

	if(UKismetInputLibrary::GetKey(InKeyEvent) == EKeys::Gamepad_FaceButton_Bottom)
	{
		if(bIsSelectedReplyKeyDown)
		{
			return FReply::Handled();
		}

		bIsSelectedReplyKeyDown = true;
		GamepadReply(CurrentlySelectedReply);
		return FReply::Handled();
	}
	
	return FReply::Unhandled();
}

FReply UDialogueWidget::NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if(UKismetInputLibrary::GetKey(InKeyEvent) == EKeys::Gamepad_FaceButton_Bottom)
	{
		bIsSelectedReplyKeyDown = false;
		return FReply::Handled();
	}
	return FReply::Unhandled();
}
