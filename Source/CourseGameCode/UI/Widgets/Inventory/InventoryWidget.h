// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryWidget.generated.h"

struct FInventorySlot;
class UInventorySlotWidget;
class UGridPanel;
/**
 * 
 */
UCLASS()
class COURSEGAMECODE_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void InitializeWidget(TArray<FInventorySlot>& InventorySlots);

protected:

	UPROPERTY(meta=(BindWidget))
	UGridPanel* GridPanelItemSlots;

	UPROPERTY(EditDefaultsOnly, Category="Item container view settings")
	TSubclassOf<UInventorySlotWidget> InventorySlotWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category="Item container view settings")
	int32 ColumnCount = 4;
	
	void AddItemSlotView(FInventorySlot& SlotToAdd);
};
