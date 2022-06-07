// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/CharacterComponents/CharacterInventoryComponent.h"
#include "InventorySlotWidget.generated.h"

class UImage;
/**
 * 
 */
UCLASS()
class COURSEGAMECODE_API UInventorySlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	void InitializeItemSlot(FInventorySlot& InventorySlot);
	virtual void UpdateView();
	void DebugLinkedSlots();
	void SetItemIcon(UTexture2D* Texture);

protected:

	UPROPERTY(meta=(BindWidget))
	UImage* ImageItemIcon;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

private:
	
	FInventorySlot* LinkedSlot;
};
