// Fill out your copyright notice in the Description page of Project Settings.


#include "InventorySlotWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Characters/CGCBaseCharacter.h"
#include "Components/Image.h"
#include "Components/CharacterComponents/CharacterInventoryComponent.h"
#include "Inventory/Items/InventoryItem.h"

void UInventorySlotWidget::InitializeItemSlot(FInventorySlot& InventorySlot)
{
	LinkedSlot = &InventorySlot;

	FInventorySlot::FInventorySlotUpdateDelegate OnInventorySlotUpdateDelegate;
	OnInventorySlotUpdateDelegate.BindUObject(this, &UInventorySlotWidget::UpdateView);
	LinkedSlot->BindOnInventorySlotUpdateDelegate(OnInventorySlotUpdateDelegate);
}

void UInventorySlotWidget::UpdateView()
{
	if(LinkedSlot == nullptr)
	{
		ImageItemIcon->SetBrushFromTexture(nullptr);
		return;
	}

	if(LinkedSlot->Item.IsValid())
	{
		const FInventoryItemDescription& Description = LinkedSlot->Item->GetItemDescription();
		ImageItemIcon->SetBrushFromTexture(Description.Icon);
	}
	else
	{
		ImageItemIcon->SetBrushFromTexture(nullptr);
	}
	DebugLinkedSlots();
}

void UInventorySlotWidget::DebugLinkedSlots()
{
	if(LinkedSlot->Item != nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Linked slot: %s"), *LinkedSlot->Item->GetItemDescription().Name.ToString()));
	}
}

void UInventorySlotWidget::SetItemIcon(UTexture2D* Texture)
{
	ImageItemIcon->SetBrushFromTexture(Texture);
}

FReply UInventorySlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if(LinkedSlot == nullptr)
	{
		return FReply::Handled();
	}

	if(!LinkedSlot->Item.IsValid())
	{
		return FReply::Handled();
	}

	FKey MouseBtn = InMouseEvent.GetEffectingButton();
	if(MouseBtn == EKeys::RightMouseButton)
	{
		TWeakObjectPtr<UInventoryItem> LinkedSlotItem = LinkedSlot->Item;
		ACGCBaseCharacter* ItemOwner = Cast<ACGCBaseCharacter>(LinkedSlotItem->GetOuter());

		if(LinkedSlotItem->Consume(ItemOwner))
		{
			LinkedSlot->ClearSlot();
		}
		return FReply::Handled();
	}

	FEventReply Reply = UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton);
	return Reply.NativeReply;
}

void UInventorySlotWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent,
	UDragDropOperation*& OutOperation)
{
	UDragDropOperation* DragDropOperation = Cast<UDragDropOperation>(UWidgetBlueprintLibrary::CreateDragDropOperation(UDragDropOperation::StaticClass()));

	UInventorySlotWidget* DragWidget = CreateWidget<UInventorySlotWidget>(GetOwningPlayer(), GetClass());
	DragWidget->ImageItemIcon->SetBrushFromTexture(LinkedSlot->Item->GetItemDescription().Icon);

	DragDropOperation->DefaultDragVisual = DragWidget;
	DragDropOperation->Pivot = EDragPivot::MouseDown;
	DragDropOperation->Payload = LinkedSlot->Item.Get();
	OutOperation = DragDropOperation;

	LinkedSlot->ClearSlot();
}

bool UInventorySlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	if(!LinkedSlot->Item.IsValid() && IsValid(InOperation->Payload))
	{
		LinkedSlot->Item = TWeakObjectPtr<UInventoryItem>(Cast<UInventoryItem>(InOperation->Payload));
		LinkedSlot->UpdateSlot();
		return true;
	}
	return false;
}

void UInventorySlotWidget::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	LinkedSlot->Item = TWeakObjectPtr<UInventoryItem>(Cast<UInventoryItem>(InOperation->Payload));
	LinkedSlot->UpdateSlot();
}
