// Fill out your copyright notice in the Description page of Project Settings.


#include "03_Widget/01_Menu/05_Orb/OrbMatrixElementWidget.h"

#include "03_Widget/01_Menu/05_Orb/OrbListButtonWidget.h"
#include "03_Widget/99_Drag/DragAndDropOperation.h"
#include "03_Widget/99_Drag/DraggableWidget.h"
#include "Blueprint/DragDropOperation.h"
#include "NiagaraSystemWidget.h"
#include "Logging/StructuredLog.h"
#include "NiagaraUIComponent.h"
#include "00_Character/BaseCharacter.h"
#include "03_Widget/SimpleToolTipWidget.h"
#include "96_Library/ItemHelperLibrary.h"
#include "96_Library/WidgetHelperLibrary.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"
#include "SoulLike/SoulLike.h"

void UOrbMatrixElementWidget::UnEquipPreCore(FGuid PreCoreUniqueID)
{
	if (auto invenComp = GetOwningPlayerPawn<ABaseCharacter>()->GetInventoryComponent())
	{
		UE_LOGFMT(LogTemp, Log, "이전 코어를 제거합니다 : {0}", PreCoreUniqueID.ToString());
		invenComp->UnEquip(PreCoreUniqueID);
		invenComp->SetEquippedCoreID(FGuid());

		//생성된 목록중, 이 코어를 표시하는 위젯을 찾습니다.
		if (OrbWidget.IsValid())
		{
			OrbWidget->SetEquippedInOrbListWidget(PreCoreUniqueID, EOrbType::CORE, false);
		}

		OrbParticle->ActivateSystem(true);
		OrbParticle_Empty->DeactivateSystem();
	}
}

bool UOrbMatrixElementWidget::UnEquipPreFragment(FGuid PreFragmentUniqueID)
{
	if (auto invenComp = GetOwningPlayerPawn<ABaseCharacter>()->GetInventoryComponent())
	{
		//생성된 목록중, 이 파편을 표시하는 위젯을 찾습니다.
		if (OrbWidget.IsValid())
		{
			OrbWidget->SetEquippedInOrbListWidget(PreFragmentUniqueID, EOrbType::FRAGMENT, false);
		}

		ClearSlot();

		OrbParticle->DeactivateSystem();
		OrbParticle_Empty->ActivateSystem(true);

		OverrideMatrixSlot();

		UE_LOGFMT(LogTemp, Log, "이전 파편을 제거합니다 : {0}", PreFragmentUniqueID.ToString());
		invenComp->UnEquip(PreFragmentUniqueID);


		return true;
	}

	return false;
}

void UOrbMatrixElementWidget::OverrideMatrixSlot()
{
	if (auto invenComp = GetOwningPlayerPawn<ABaseCharacter>()->GetInventoryComponent())
	{
		if (auto coreInfo = static_cast<const FOrbCoreInformation*>(invenComp->GetEquippedCore().GetItemInformation()))
		{
			if (UOrbMatrix* matrix = coreInfo->OrbMatrix)
			{
				matrix->OrbMatrix[MatrixElementInfo.Row][MatrixElementInfo.Col] = MatrixElementInfo;
#if WITH_EDITOR
				UE_LOGFMT(LogTemp, Log, "----------------------------------------");
				UE_LOGFMT(LogTemp, Log, "--------------메트릭스 슬롯 오버라이드-------------------");

				FString msg = "\n";
				for (auto i = 0; i < matrix->OrbMatrix.Num(); i++)
				{
					for (auto j = 0; j < matrix->OrbMatrix[i].Num(); j++)
					{
						if (matrix->OrbMatrix[i][j].OrbMatrixSlotType == EOrbMatrixSlotType::LINE)
						{
							msg += FString::Printf(
								TEXT("%s	"),
								*StaticEnum<EOrbMatrixSlotType>()->GetNameByValue(
									static_cast<int64>(matrix->OrbMatrix[i][j].OrbMatrixSlotType)).ToString());
							continue;
						}

						if (matrix->OrbMatrix[i][j].OrbMatrixSlotType == EOrbMatrixSlotType::NONE)
						{
							msg += FString::Printf(
								TEXT("%s	"),
								*StaticEnum<EOrbMatrixSlotType>()->GetNameByValue(
									static_cast<int64>(matrix->OrbMatrix[i][j].OrbMatrixSlotType)).ToString());
							continue;
						}
						if (matrix->OrbMatrix[i][j].OrbMatrixSlotType == EOrbMatrixSlotType::CORE)
						{
							msg += FString::Printf(
								TEXT("%s"), *StaticEnum<EOrbMatrixSlotType>()->GetNameByValue(
									static_cast<int64>(matrix->OrbMatrix[i][j].OrbMatrixSlotType)).ToString());
							continue;
						}

						msg += FString::Printf(
							TEXT("%s(%s)	"),
							*StaticEnum<EOrbMatrixSlotType>()->GetNameByValue(
								static_cast<int64>(matrix->OrbMatrix[i][j].OrbMatrixSlotType)).ToString(),
							matrix->OrbMatrix[i][j].bLock ? TEXT("잠김") : TEXT("열림"));
					}
					msg += "\n";
				}
				UE_LOGFMT(LogTemp, Log, "{0}", msg);
				UE_LOGFMT(LogTemp, Log, "----------------------------------------");
#endif
			}
		}
	}
}

void UOrbMatrixElementWidget::LoadMatrixSlot()
{
	if (MatrixElementInfo.UniqueID != FGuid())
	{
		if (auto invenComp = GetOwningPlayerPawn<ABaseCharacter>()->GetInventoryComponent())
		{
			if (invenComp->IsFragmentContains(MatrixElementInfo.UniqueID))
			{
				OnEquipFragmentEvent(invenComp->GetFragment(MatrixElementInfo.UniqueID));
			}
		}
	}
}

void UOrbMatrixElementWidget::EquipOrbFragment(const FInventoryItem& OrbFragmentItem)
{
	if (auto invenComp = GetOwningPlayerPawn<ABaseCharacter>()->GetInventoryComponent())
	{
		if (!invenComp->IsEquippedEquipment(OrbFragmentItem.UniqueID))
		{
			invenComp->UseItem(OrbFragmentItem.UniqueID);
		}
	}
}

UUserWidget* UOrbMatrixElementWidget::CreateCustomDragShadowWidget()
{
	if (auto widget = CreateWidget<UOrbMatrixElementDragShadowWidget>(GetOwningPlayer(), CustomDragShadowWidgetObject))
	{
		UKismetSystemLibrary::PrintString(this,TEXT("커스텀 드래그 위젯 생성"));
		widget->DraggedWidget = this;
		widget->SetParticleColor(OrbColor[MatrixElementInfo.OrbMatrixSlotType]);
		return widget;
	}

	return nullptr;
}


void UOrbMatrixElementWidget::OnEquipFragmentEvent(const FInventoryItem& OrbFragmentItem)
{
	if (OrbFragmentItem.UniqueID != FGuid())
	{
		MatrixElementInfo.UniqueID = OrbFragmentItem.UniqueID;
		MatrixElementInfo.OrbMatrixSlotContent = EOrbMatrixSlotContent::Set;

		//저장을 위해서 정보를 덮어씁니다.
		OverrideMatrixSlot();

		//장착된 파편을 사용하게 합니다.
		EquipOrbFragment(OrbFragmentItem);

		OrbParticle->ActivateSystem(true);
		OrbParticle_Empty->DeactivateSystem();

		//생성된 목록중, 이 파편을 표시하는 위젯을 찾습니다.
		if (OrbWidget.IsValid())
		{
			OrbWidget->SetEquippedInOrbListWidget(OrbFragmentItem.UniqueID, EOrbType::FRAGMENT, true);
		}
	}
}

void UOrbMatrixElementWidget::OnClicked()
{
	UKismetSystemLibrary::PrintString(this,TEXT("슬롯 개방용 클릭"));
	OnClickedMatrixElement.Broadcast(MatrixElementInfo, this);
}

#define LOCTEXT_NAMESPACE "OrbMatrixElementWidget"

void UOrbMatrixElementWidget::OnHoveredEvent()
{
	if (MatrixElementInfo.OrbMatrixSlotType != EOrbMatrixSlotType::LINE)
	{
		if (MatrixElementInfo.UniqueID != FGuid())
		{
			UKismetSystemLibrary::PrintString(this,TEXT("이 슬롯의 유니크 아이디 : ") + MatrixElementInfo.UniqueID.ToString());
		}
		UGameplayStatics::PlaySound2D(this, HoveredSound);
	}
	//UKismetSystemLibrary::PrintString(this,TEXT("이 슬롯의 상태 : ")+StaticEnum<EOrbMatrixSlotContent>()->GetValueAsString(MatrixElementInfo.OrbMatrixSlotContent));
	if (MatrixElementInfo.OrbMatrixSlotContent == EOrbMatrixSlotContent::Empty)
	{
		FString toolTipMsg;
		toolTipMsg += "<orb.slot>" + OrbMatrixSlotTypeToText(MatrixElementInfo.OrbMatrixSlotType).ToString() + "</>";
		toolTipMsg += "\n<orb.empty>" + OrbMatrixSlotContentToString(MatrixElementInfo.OrbMatrixSlotContent).ToString()
			+ "</>";

		if (MatrixElementInfo.bLock)
		{
			const FText lockedText = NSLOCTEXT("OrbMatrixElementWidget", "LockedText", "잠김");
			toolTipMsg += "\n<orb.lock>" + lockedText.ToString() + "</>";
		}

		UWidgetHelperLibrary::SetToolTipWidget(this, FText::FromString(toolTipMsg));
	}
	else
	{
		UpdateToolTip(MatrixElementInfo.UniqueID);
	}
}
#undef LOCTEXT_NAMESPACE
void UOrbMatrixElementWidget::NativeConstruct()
{
	Super::NativeConstruct();

	bUseCustomDragShadowWidget = true;

	OnEquipFragment.AddUniqueDynamic(this, &UOrbMatrixElementWidget::OnEquipFragmentEvent);
	/*OnClickedMatrixElement.AddUniqueDynamic(this,&UOrbMatrixElementWidget::OnClickedMatrixElementEvent);*/
	/*Button->OnClicked.AddUniqueDynamic(this, &UOrbMatrixElementWidget::OnClicked);
	Button->OnHovered.AddUniqueDynamic(this,&UOrbMatrixElementWidget::OnHoverdEvent);*/
}


bool UOrbMatrixElementWidget::IsSameType(EOrbMatrixSlotType SlotType)
{
	//자유 슬롯은 무조건 같은 타입이라고 해줍니다.
	if (MatrixElementInfo.OrbMatrixSlotType == EOrbMatrixSlotType::FREE)
	{
		return true;
	}

	//자유타입의 프래그먼트인 경우, 그냥 장착하게 할 수 있도록 합니다.
	if (SlotType == EOrbMatrixSlotType::FREE)
	{
		return true;
	}

	return MatrixElementInfo.OrbMatrixSlotType == SlotType;
}


void UOrbMatrixElementWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent,
                                                   UDragDropOperation*& OutOperation)
{
	UKismetSystemLibrary::PrintString(this,TEXT("NativeOnDragDetected"));
	if (MatrixElementInfo.OrbMatrixSlotType == EOrbMatrixSlotType::LINE || MatrixElementInfo.OrbMatrixSlotType ==
		EOrbMatrixSlotType::NONE || MatrixElementInfo.OrbMatrixSlotType == EOrbMatrixSlotType::CORE)
	{
		return;
	}

	if (MatrixElementInfo.OrbMatrixSlotContent == EOrbMatrixSlotContent::Empty)
	{
		return;
	}

	//만약 슬롯이 잠긴 상태라면 아무것도 안 합니다.
	if (bIsLocked)
	{
		return;
	}

	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);
}

bool UOrbMatrixElementWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
                                           UDragDropOperation* InOperation)
{
	//슬롯 타입 중, 빈 타입과 라인타입이면 아무것도 안 합니다.
	if (MatrixElementInfo.OrbMatrixSlotType == EOrbMatrixSlotType::LINE || MatrixElementInfo.OrbMatrixSlotType ==
		EOrbMatrixSlotType::NONE)
	{
		return false;
	}

	//만약 슬롯이 잠긴 상태라면 아무것도 안 합니다.
	if (bIsLocked)
	{
		return false;
	}

	if (InOperation->IsA<UDragAndDropOperation>())
	{
		auto oper = Cast<UDragAndDropOperation>(InOperation);

		oper->DraggedWidget->SetRenderOpacity(oper->OriginalOpacity);
		oper->DraggedWidget->SetVisibility(ESlateVisibility::Visible);

		//리스트에서 장착을 위해서 끌어왔을 때,
		if (oper->DraggedWidget->IsA<UOrbListButtonWidget>())
		{
			if (auto button = Cast<UOrbListButtonWidget>(oper->DraggedWidget))
			{
				/*button->SetRenderOpacity(oper->OriginalOpacity);
				button->SetVisibility(ESlateVisibility::Visible);*/
				const auto& orbInfo = button->GetOrbItem();

				if (orbInfo.GetItemInformation() == nullptr)
				{
					UE_LOGFMT(LogTemp, Error, "오브 아이템 정보가 비었습니다!!!");
					return false;
				}

				if (UItemHelperLibrary::IsOrbCore(orbInfo))
				{
					UE_LOGFMT(LogTemp, Log, "코어 장착 시도");
					if (MatrixElementInfo.OrbMatrixSlotType == EOrbMatrixSlotType::CORE)
					{
						//이전에 장착중인 코어가 있었다면, 장착해제합니다.
						if (MatrixElementInfo.UniqueID != FGuid())
						{
							UnEquipPreCore(MatrixElementInfo.UniqueID);
						}
						MatrixElementInfo.UniqueID = orbInfo.UniqueID;
						if (auto invenComp = GetOwningPlayerPawn<ABaseCharacter>()->GetInventoryComponent())
						{
							invenComp->SetEquippedCoreID(MatrixElementInfo.UniqueID);
						}

						//코어 설정, 저장된 메트릭스를 가져와서 그리드에 설정해줍니다.
						OnEquipCore.Broadcast(orbInfo);

						return true;
					}
				}

				if (UItemHelperLibrary::IsOrbFragment(orbInfo) && OrbWidget.IsValid())
				{
					//코어 슬롯에 파편을 끌어온 경우는 아무것도 안 합니다.
					if(MatrixElementInfo.OrbMatrixSlotType == EOrbMatrixSlotType::CORE)
					{
						return false;
					}
					UE_LOGFMT(LogTemp, Log, "파편 장착 시도");
					//다른 슬롯에 장착되어 있는 경우,
					if (auto matrixElementWidget = OrbWidget->IsOrbEquipped(orbInfo.UniqueID))
					{
						//다른 슬롯을 찾아서 장착 해제함.
						matrixElementWidget->UnEquipPreFragment(orbInfo.UniqueID);
					}

					//슬롯 타입과 오브 타입에 따라서 장착합니다.
					if (IsSameType(static_cast<const FOrbFragmentInformation*>(orbInfo.GetItemInformation())->SlotType))
					{
						if (MatrixElementInfo.UniqueID != FGuid())
						{
							UnEquipPreFragment(MatrixElementInfo.UniqueID);
						}

						OnEquipFragment.Broadcast(orbInfo);
						UGameplayStatics::PlaySound2D(this, FragmentEquippedSound);

						return true;
					}
				}
			}
		}
		//이미 장착된 파편을 교환하기 위해 끌어왔을 때,
		if (oper->DraggedWidget->IsA<UOrbMatrixElementWidget>())
		{
			UE_LOGFMT(LogTemp, Log, "교환을 위한 드래그 감지");
			//드래그 한 위젯
			if (auto draggedWidget = Cast<UOrbMatrixElementWidget>(oper->DraggedWidget))
			{
				//서로 교환 가능한 슬롯 타입인지 확인합니다.
				if (CanExchange(draggedWidget->MatrixElementInfo))
				{
					//내부 데이터를 교환합니다.
					//적용 효과는 따로 건들필요는 없습니다.
					//이미 적용된 대상끼리의 교환이니까요
					ExchangeData(draggedWidget);

					return true;
				}
			}
		}
	}

	return false;
}


void UOrbMatrixElementWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

	OnHoveredEvent();
}

void UOrbMatrixElementWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
}

FReply UOrbMatrixElementWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	UKismetSystemLibrary::PrintString(this,TEXT("NativeOnMouseButtonUp"));
	return Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
}

FReply UOrbMatrixElementWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	UKismetSystemLibrary::PrintString(this,TEXT("NativeOnMouseButtonDown"));
	if (ElementType == EOrbMatrixElementType::Normal)
	{
		if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
		{
			if (MatrixElementInfo.UniqueID != FGuid())
			{
				if (!UItemHelperLibrary::IsOrbCore(MatrixElementInfo.UniqueID,
				                                   GetOwningPlayerPawn<ABaseCharacter>()->GetInventoryComponent()))
				{
					UnEquipPreFragment(MatrixElementInfo.UniqueID);
					return FReply::Handled();
				}
			}
		}

		if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
		{
			if (ElementType == EOrbMatrixElementType::Normal)
			{
				if (OrbWidget.IsValid())
				{
					OrbWidget->UMG_OrbElementListFragment->InitFilterFromElement();
					if (MatrixElementInfo.OrbMatrixSlotType == EOrbMatrixSlotType::FREE)
					{
						OrbWidget->UMG_OrbElementListFragment->AddFilterFromElement(EOrbMatrixSlotType::FREE);
						OrbWidget->UMG_OrbElementListFragment->AddFilterFromElement(EOrbMatrixSlotType::PHYSICAL);
						OrbWidget->UMG_OrbElementListFragment->AddFilterFromElement(EOrbMatrixSlotType::MAGICAL);
						OrbWidget->UMG_OrbElementListFragment->AddFilterFromElement(EOrbMatrixSlotType::DEFENCE);
					}
					else
					{
						OrbWidget->UMG_OrbElementListFragment->AddFilterFromElement(EOrbMatrixSlotType::FREE);
						OrbWidget->UMG_OrbElementListFragment->
						           AddFilterFromElement(MatrixElementInfo.OrbMatrixSlotType);
					}

					OrbWidget->UMG_OrbElementListFragment->OnUpdateFilterEvent();
				}
			}
		}
	}
	else
	{
		//좌클릭시 클릭 처리
		if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
		{
			OnClicked();
			return FReply::Handled();
		}
	}

	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void UOrbMatrixElementWidget::ChangeElementImageStyle(UObject* NewTexture, float Alpha) const
{
	if (NewTexture == nullptr)
	{
		Image_Line->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}
	Image_Line->SetVisibility(ESlateVisibility::HitTestInvisible);
	Image_Line->SetBrushResourceObject(NewTexture);

	/*auto style = Button->GetStyle();

	style.Normal.TintColor = FLinearColor(1, 1, 1, Alpha);
	style.Normal.SetResourceObject(NewTexture);

	style.Hovered.TintColor = FLinearColor(0.095307, 0.095307, 0.095307, Alpha);
	style.Hovered.SetResourceObject(NewTexture);

	style.Pressed.TintColor = FLinearColor(0.028426, 0.028426, 0.028426, Alpha);
	style.Pressed.SetResourceObject(NewTexture);

	Button->SetStyle(style);*/
}

const FOrbFragmentInformation* UOrbMatrixElementWidget::GetOrbFragmentInformation(
	const FOrbMatrixElementInfo& Info)
{
	if (auto invenComp = GetOwningPlayerPawn<ABaseCharacter>()->GetInventoryComponent())
	{
		if (invenComp->IsFragmentContains(Info.UniqueID))
		{
			if (auto frag = invenComp->GetFragment(Info.UniqueID).GetItemInformation())
			{
				return static_cast<const FOrbFragmentInformation*>(frag);
			}
		}
	}

	return nullptr;
}

bool UOrbMatrixElementWidget::ExchangeCheck(const FOrbMatrixElementInfo& A,
                                            const FOrbMatrixElementInfo& B)
{
	//0.
	{
		if (auto frag = GetOrbFragmentInformation(A))
		{
			if (frag->SlotType == EOrbMatrixSlotType::FREE)
			{
				//0-1.
				/*if (B.OrbMatrixSlotContent == EOrbMatrixSlotContent::Empty)
				{
					return true;
				}*/

				return true;
			}
		}

		//0-2.
		//B슬롯에 장착된 파편을 가져옵니다.
		if (auto frag = GetOrbFragmentInformation(B))
		{
			//B슬롯에 장착된 파편이 A슬롯에 장착 가능한가?
			if (frag->SlotType == A.OrbMatrixSlotType)
			{
				return true;
			}
		}
	}
	return false;
}

bool UOrbMatrixElementWidget::CanExchange(const FOrbMatrixElementInfo& OtherMatrixElementInfo)
{
	//코어 슬롯에 파편을 끌어온 경우는 아무것도 안 합니다.
	if(MatrixElementInfo.OrbMatrixSlotType == EOrbMatrixSlotType::CORE)
	{
		if(OtherMatrixElementInfo.OrbMatrixSlotType != EOrbMatrixSlotType::CORE)
		{
			return false;
		}
	}
	
	//다음과 같은 경우에만 교환 가능합니다.
	//드롭 당한 슬롯 : A / 드래그한 슬롯 : B


	//두개의 슬롯이 모두 비어있지 않은 경우,
	//0. A 파편이 Free 타입인 경우
	//0-1. B가 비어있는 슬롯인 경우
	//0-2. B슬롯에 장착된 파편이 A슬롯에 장착 가능한 경우
	//1.  B 파편이 Free 타입인 경우
	//1-1. 교환 슬롯이 비어있는 경우
	//1-2. 교환 슬롯에 장착된 파편이 원본 파편이 장착된 슬롯에 장착 가능한 경우


	//한쪽 슬롯이 비어있는 경우

	//2. 어느 한쪽의 슬롯이 자유 타입인 경우

	//3. 양쪽 슬롯 타입이 같은 경우

	const auto& A = MatrixElementInfo;
	const auto& B = OtherMatrixElementInfo;
	//두개의 슬롯이 모두 비어있지 않은 경우의 처리
	if (A.UniqueID != FGuid() && B.UniqueID != FGuid())
	{
		//0.
		if (ExchangeCheck(A, B))
		{
			return true;
		}
		//1.
		if (ExchangeCheck(B, A))
		{
			return true;
		}

		return false;
	}


	//한쪽 슬롯이 비어있는 경우의 처리
	if (A.UniqueID == FGuid() || B.UniqueID == FGuid())
	{
		//비어있지 않은 슬롯의 파편 타입이 자유인 경우 참
		if (A.UniqueID != FGuid())
		{
			if (GetOrbFragmentInformation(A)->SlotType == EOrbMatrixSlotType::FREE)
			{
				return true;
			}

			if (GetOrbFragmentInformation(A)->SlotType == B.OrbMatrixSlotType)
			{
				return true;
			}

			if (B.OrbMatrixSlotType == EOrbMatrixSlotType::FREE)
			{
				return true;
			}
		}

		if (B.UniqueID != FGuid())
		{
			if (GetOrbFragmentInformation(B)->SlotType == EOrbMatrixSlotType::FREE)
			{
				return true;
			}

			if (GetOrbFragmentInformation(B)->SlotType == A.OrbMatrixSlotType)
			{
				return true;
			}

			if (A.OrbMatrixSlotType == EOrbMatrixSlotType::FREE)
			{
				return true;
			}
		}

		return false;
	}

	//두개중에 하나만 자유여도 참
	if (A.OrbMatrixSlotType == EOrbMatrixSlotType::FREE || B.OrbMatrixSlotType ==
		EOrbMatrixSlotType::FREE)
	{
		return true;
	}

	//3번 체크
	return MatrixElementInfo.OrbMatrixSlotType == OtherMatrixElementInfo.OrbMatrixSlotType;
}

void UOrbMatrixElementWidget::ExchangeData(UOrbMatrixElementWidget* OtherWidget)
{
	UE_LOGFMT(LogTemp, Warning, "----------교환 전 상태 표시-----------");
	UE_LOGFMT(LogTemp, Warning, "{0}의 상태", GetName());
	UE_LOGFMT(LogTemp, Warning, "{0} / {1}",
	          StaticEnum<EOrbMatrixSlotContent>()->GetValueAsString(MatrixElementInfo.OrbMatrixSlotContent),
	          StaticEnum<EOrbMatrixSlotType>()->GetValueAsString(MatrixElementInfo.OrbMatrixSlotType));
	UE_LOGFMT(LogTemp, Warning, "------------------------------------");
	UE_LOGFMT(LogTemp, Warning, "{0}의 상태", OtherWidget->GetName());
	UE_LOGFMT(LogTemp, Warning, "{0} / {1}",
	          StaticEnum<EOrbMatrixSlotContent>()->GetValueAsString(OtherWidget->MatrixElementInfo.OrbMatrixSlotContent
	          ), StaticEnum<EOrbMatrixSlotType>()->GetValueAsString(OtherWidget->MatrixElementInfo.OrbMatrixSlotType));
	UE_LOGFMT(LogTemp, Warning, "------------------------------------");

	const auto copy = MatrixElementInfo;
	MatrixElementInfo.Exchange(OtherWidget->MatrixElementInfo);
	OtherWidget->MatrixElementInfo.Exchange(copy);

	UE_LOGFMT(LogTemp, Warning, "----------교환 후 상태 표시-----------");
	UE_LOGFMT(LogTemp, Warning, "{0}의 상태", GetName());
	UE_LOGFMT(LogTemp, Warning, "{0} / {1}",
	          StaticEnum<EOrbMatrixSlotContent>()->GetValueAsString(MatrixElementInfo.OrbMatrixSlotContent),
	          StaticEnum<EOrbMatrixSlotType>()->GetValueAsString(MatrixElementInfo.OrbMatrixSlotType));
	UE_LOGFMT(LogTemp, Warning, "------------------------------------");
	UE_LOGFMT(LogTemp, Warning, "{0}의 상태", OtherWidget->GetName());
	UE_LOGFMT(LogTemp, Warning, "{0} / {1}",
	          StaticEnum<EOrbMatrixSlotContent>()->GetValueAsString(OtherWidget->MatrixElementInfo.OrbMatrixSlotContent
	          ), StaticEnum<EOrbMatrixSlotType>()->GetValueAsString(OtherWidget->MatrixElementInfo.OrbMatrixSlotType));
	UE_LOGFMT(LogTemp, Warning, "------------------------------------");

	ChangeOrbParticleState();
	UpdateToolTip(MatrixElementInfo.UniqueID);

	OtherWidget->ChangeOrbParticleState();
	OtherWidget->UpdateToolTip(OtherWidget->MatrixElementInfo.UniqueID);

	UGameplayStatics::PlaySound2D(this, ExchangeSound);
}

void UOrbMatrixElementWidget::SetLine()
{
	if (OrbParticle)
	{
		if (auto comp = OrbParticle->GetNiagaraComponent())
		{
			OrbParticle->DeactivateSystem();
		}
	}

	if (OrbParticle_Empty)
	{
		if (auto comp = OrbParticle_Empty->GetNiagaraComponent())
		{
			OrbParticle_Empty->DeactivateSystem();
		}
	}


	SetVisibility(ESlateVisibility::HitTestInvisible);


	if (LineSlotTexture)
	{
		ChangeElementImageStyle(LineSlotTexture);
		switch (MatrixElementInfo.SlotLineDirection)
		{
		case EOrbMatrixSlotLineDirection::Vertical:
			SetRenderTransform(
				FWidgetTransform(FVector2D::ZeroVector, FVector2D(1.f, 1.f), FVector2D::ZeroVector, 90.f));
			break;
		case EOrbMatrixSlotLineDirection::Horizontal:
			SetRenderTransform(FWidgetTransform(FVector2D::ZeroVector, FVector2D(1.f, 1.f), FVector2D::ZeroVector,
			                                    0.f));
			break;
		case EOrbMatrixSlotLineDirection::LeftUpToRightDown:
			SetRenderTransform(FWidgetTransform(FVector2D::ZeroVector, FVector2D(1.5f, 1.f), FVector2D::ZeroVector,
			                                    45.f));
			break;
		case EOrbMatrixSlotLineDirection::RightUpToLeftDown:
			SetRenderTransform(FWidgetTransform(FVector2D::ZeroVector, FVector2D(1.5f, 1.f), FVector2D::ZeroVector,
			                                    -45.f));
			break;
		case EOrbMatrixSlotLineDirection::None:
			SetRenderTransform(FWidgetTransform(FVector2D::ZeroVector, FVector2D(1.f, 1.f), FVector2D::ZeroVector,
			                                    0.f));
			ChangeElementImageStyle(nullptr, 0);
			break;
		default: ;
		}
	}
}

void UOrbMatrixElementWidget::SetCore()
{
	//코어 아이디를 가져옵니다.
	if (auto invenComp = GetOwningPlayerPawn<ABaseCharacter>()->GetInventoryComponent())
	{
		MatrixElementInfo.UniqueID = invenComp->GetEquippedCoreID();
	}

	OrbParticle->ActivateSystem(true);
	OrbParticle_Empty->DeactivateSystem();


	SetRenderScale(FVector2D(1.25f, 1.25f));
	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	ChangeElementImageStyle(nullptr, 0);

	if (OrbParticle)
	{
		if (auto comp = OrbParticle->GetNiagaraComponent())
		{
			comp->SetColorParameter("Color", CoreColor);
		}
		else
		{
			UE_LOGFMT(LogTemp, Error, "나이아가라 컴포넌트가 널입니다.");
		}
	}
}

void UOrbMatrixElementWidget::ChangeOrbParticleState()
{
	if (MatrixElementInfo.OrbMatrixSlotContent == EOrbMatrixSlotContent::Set)
	{
		OrbParticle->ActivateSystem(true);
		OrbParticle_Empty->DeactivateSystem();
	}
	else
	{
		OrbParticle->DeactivateSystem();
		OrbParticle_Empty->ActivateSystem(true);
	}
}

void UOrbMatrixElementWidget::SetPoint()
{
	ChangeOrbParticleState();

	//이 슬롯이 잠겨있는 경우
	if (bIsLocked)
	{
		OrbParticle_Empty->DeactivateSystem();
		OrbParticle->DeactivateSystem();
		Image_Locked->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else
	{
		Image_Locked->SetVisibility(ESlateVisibility::Collapsed);
	}

	SetRenderScale(FVector2D(1, 1));
	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	ChangeElementImageStyle(nullptr, 0);

	if (OrbColor.Contains(MatrixElementInfo.OrbMatrixSlotType))
	{
		if (OrbParticle)
		{
			if (auto comp = OrbParticle->GetNiagaraComponent())
			{
				comp->SetColorParameter("Color", OrbColor[MatrixElementInfo.OrbMatrixSlotType]);
			}
		}

		if (OrbParticle_Empty)
		{
			if (auto comp = OrbParticle_Empty->GetNiagaraComponent())
			{
				comp->SetColorParameter("Color", OrbColor[MatrixElementInfo.OrbMatrixSlotType]);
			}
		}
	}
}

void UOrbMatrixElementWidget::SetInfo(
	const FOrbMatrixElementInfo& OrbMatrixElementInfo)
{
	bIsLocked = OrbMatrixElementInfo.bLock;
	MatrixElementInfo = OrbMatrixElementInfo;

	OrbParticle->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	switch (MatrixElementInfo.OrbMatrixSlotType)
	{
	case EOrbMatrixSlotType::NONE:
		SetVisibility(ESlateVisibility::Hidden);
		break;
	case EOrbMatrixSlotType::LINE:
		SetLine();
		break;
	case EOrbMatrixSlotType::CORE:
		SetCore();
		break;
	case EOrbMatrixSlotType::PHYSICAL:
	case EOrbMatrixSlotType::DEFENCE:
	case EOrbMatrixSlotType::MAGICAL:
	case EOrbMatrixSlotType::FREE:
		SetPoint();
		LoadMatrixSlot();
	default: ;
	}
}

void UOrbMatrixElementWidget::ClearSlot()
{
	CoreColor = FLinearColor::Gray;
	OnEquipCore.Clear();
	MatrixElementInfo.UniqueID = FGuid();
	MatrixElementInfo.OrbMatrixSlotContent = EOrbMatrixSlotContent::Empty;
}

void UOrbMatrixElementWidget::UpdateToolTip(const FGuid& UniqueID)
{
	if (UniqueID != FGuid())
	{
		if (auto invenComp = GetOwningPlayerPawn<APlayerCharacter>()->GetInventoryComponent())
		{
			if (MatrixElementInfo.OrbMatrixSlotType == EOrbMatrixSlotType::CORE)
			{
				const auto& item = invenComp->GetInventoryItem(UniqueID);
				UpdateToolTip(item);
			}
			else
			{
				if (invenComp->IsFragmentContains(UniqueID))
				{
					const auto& item = invenComp->GetFragment(UniqueID);
					UpdateToolTip(item);
				}
			}
		}
	}
	else
	{
		UWidgetHelperLibrary::SetToolTipWidget(this, FText::FromString(TEXT("비어있음")));
	}
}

void UOrbMatrixElementWidget::UpdateToolTip(const FInventoryItem& OrbItem)
{
	if (UItemHelperLibrary::IsOrbCore(OrbItem))
	{
		//UKismetSystemLibrary::PrintString(this,TEXT("코어"));
		FString desc = "<orb.name>" + OrbItem.GetItemInformation()->Item_Name.ToString() + "</>";

		desc += "\n" + UItemHelperLibrary::GetItemDetailText(
			OrbItem, GetOwningPlayerPawn<ABaseCharacter>()->GetInventoryComponent()).ToString();

		UWidgetHelperLibrary::SetToolTipWidget(this, FText::FromString(desc));
		return;
	}

	if (UItemHelperLibrary::IsOrbFragment(OrbItem))
	{
		UWidgetHelperLibrary::SetToolTipWidget(this, UItemHelperLibrary::GetFragmentToolTipText(OrbItem));
		/*//UKismetSystemLibrary::PrintString(this,TEXT("파편"));
		auto msg = "<orb.name>" + OrbItem.GetItemInformation()->Item_Name + "</>";

		const auto frag = static_cast<const FOrbFragmentInformation*>(OrbItem.GetItemInformation());
		msg += "\n" + SlotTypeToDecoText(frag->SlotType) + SlotTypeToString(frag->SlotType) + "</>";
		msg += "\n" + RarityToDecoText(frag->Rarity);
		msg += RarityToString(frag->Rarity) + "</>";


		msg += "\n" + OrbItem.GetFormattedDescription();

		if (frag->Abilities.Num() > 0)
		{
			msg += TEXT("\n<orb.information>추가 특성</>\n");
			for (auto ab : frag->Abilities)
			{
				if (ab.GetDefaultObject() != nullptr)
				{
					msg += "<orb.talent>" + ab.GetDefaultObject()->GetAbilityInformation().AbilityName + "</>\n";
					msg += "<orb.talent.desc>" + ab.GetDefaultObject()->GetAbilityInformation().AbilityDescription +
						"</>\n";
				}
			}
		}

		UWidgetHelperLibrary::SetToolTipWidget(this, msg);
		//Cast<USimpleToolTipWidget>(GetToolTip())->SetDescriptionText(msg);*/
	}
}

void UOrbMatrixElementWidget::Refresh(const FOrbMatrixElementInfo& RefreshedInfo)
{
	//이 슬롯이 들고 있는 MatrixElementInfo를 다시 받아옵니다.
	SetInfo(RefreshedInfo);
}

void UOrbMatrixElementDragShadowWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (auto comp = OrbParticle->GetNiagaraComponent())
	{
		comp->SetColorParameter("Color", ColorToChange);
	}
}

void UOrbMatrixElementDragShadowWidget::SetParticleColor(FLinearColor Color)
{
	ColorToChange = Color;

	/*
	UKismetSystemLibrary::PrintString(this,TEXT("오브 컬러 변경함 1"));
	if (auto comp = OrbParticle->GetNiagaraComponent())
	{
		UKismetSystemLibrary::PrintString(this,TEXT("오브 컬러 변경함 2"));
		comp->SetColorParameter("Color",Color);
	}*/
}
