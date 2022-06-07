// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryWidget.h"
#include "InventorySlotWidget.h"
#include "Components/CharacterComponents/CharacterInventoryComponent.h"
#include "Components/GridPanel.h"



void UInventoryWidget::InitializeWidget(TArray<FInventorySlot>& InventorySlots)
{
	for(FInventorySlot& Item : InventorySlots)
	{
		AddItemSlotView(Item);
	}
}

void UInventoryWidget::AddItemSlotView(FInventorySlot& SlotToAdd)
{
	if(!IsValid(InventorySlotWidgetClass))
	{
		return;
	}

	UInventorySlotWidget* SlotWidget = CreateWidget<UInventorySlotWidget>(this, InventorySlotWidgetClass);
	

	if(SlotWidget != nullptr)
	{
		SlotWidget->InitializeItemSlot(SlotToAdd);

		const int32 CurrentSlotCount = GridPanelItemSlots->GetChildrenCount();
		const int32 CurrentSlotRow = CurrentSlotCount / ColumnCount;
		const int32 CurrentSlotColumn = CurrentSlotCount % ColumnCount;
		GridPanelItemSlots->AddChildToGrid(SlotWidget, CurrentSlotRow, CurrentSlotColumn);

		SlotWidget->UpdateView();
	}
}
