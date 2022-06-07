// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponSlotWidget.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Image.h"
#include "Utility/CGCDataTablesUtils.h"

void UWeaponSlotWidget::InitializeEquipmentSlot(TWeakObjectPtr<AEquipableItem> Equipment, int32 Index)
{
	if(!Equipment.IsValid())
	{
		return;
	}

	LinkedEquipableItem = Equipment;
	SlotIndexInComponent = Index;

	FWeaponTableRow* EquipmentData = CGCDataTablesUtils::FindWeaponData(Equipment->GetDataTableId());
	if(EquipmentData != nullptr)
	{
		AdapterLinkedInventoryItem = NewObject<UWeaponInventoryItem>(Equipment->GetOwner());
		AdapterLinkedInventoryItem->Initialize(Equipment->GetDataTableId(), EquipmentData->WeaponItemDescription);
		AdapterLinkedInventoryItem->SetEquipWeaponClass(EquipmentData->EquipableActor);
	}
}

void UWeaponSlotWidget::UpdateView()
{
	if(LinkedEquipableItem.IsValid())
	{
		Icon->SetBrushFromTexture(AdapterLinkedInventoryItem->GetItemDescription().Icon);
		TBWeaponName->SetText(FText::FromName(AdapterLinkedInventoryItem->GetItemDescription().Name));
	}
	else
	{
		Icon->SetBrushFromTexture(nullptr);
		TBWeaponName->SetText(FText::FromName(NAME_None));
	}
}

FReply UWeaponSlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if(!LinkedEquipableItem.IsValid())
	{
		return FReply::Handled();
	}

	FEventReply Reply = UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton);
	return Reply.NativeReply;
}

void UWeaponSlotWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent,
	UDragDropOperation*& OutOperation)
{
	if(!AdapterLinkedInventoryItem.IsValid())
	{
		return;
	}

	UDragDropOperation* DragDropOperation = Cast<UDragDropOperation>(UWidgetBlueprintLibrary::CreateDragDropOperation(UDragDropOperation::StaticClass()));
	UInventorySlotWidget* DragWidget = CreateWidget<UInventorySlotWidget>(GetOwningPlayer(), DragAndDropWidgetClass);
	DragWidget->SetItemIcon(AdapterLinkedInventoryItem->GetItemDescription().Icon);

	DragDropOperation->DefaultDragVisual = DragWidget;
	DragDropOperation->Pivot = EDragPivot::MouseDown;
	DragDropOperation->Payload = AdapterLinkedInventoryItem.Get();
	OutOperation = DragDropOperation;

	LinkedEquipableItem.Reset();
	if(OnEquipmentRemoveFromSlotDelegate.IsBound())
	{
		OnEquipmentRemoveFromSlotDelegate.Execute(SlotIndexInComponent);
	}
	UpdateView();
}

bool UWeaponSlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	const UWeaponInventoryItem* OperationObject = Cast<UWeaponInventoryItem>(InOperation->Payload);
	if(IsValid(OperationObject))
	{
		return OnEquipmentDropInSlotDelegate.Execute(OperationObject->GetEquipWeaponClass(), SlotIndexInComponent);
	}
	return false;
}

void UWeaponSlotWidget::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	AdapterLinkedInventoryItem = Cast<UWeaponInventoryItem>(InOperation->Payload);
	OnEquipmentDropInSlotDelegate.Execute(AdapterLinkedInventoryItem->GetEquipWeaponClass(), SlotIndexInComponent);
}
