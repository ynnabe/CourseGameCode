// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Equipment/EquipableItem.h"
#include "Components/TextBlock.h"
#include "Inventory/Items/Weapons/WeaponInventoryItem.h"
#include "UI/Widgets/Inventory/InventorySlotWidget.h"
#include "WeaponSlotWidget.generated.h"

/**
 * 
 */
UCLASS()
class COURSEGAMECODE_API UWeaponSlotWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	
	DECLARE_DELEGATE_RetVal_TwoParams(bool, FOnEquipmentDropInSlot, const TSubclassOf<AEquipableItem>&, int32);
	DECLARE_DELEGATE_OneParam(FOnEquipmentRemoveFromSlot, int32);

	FOnEquipmentDropInSlot OnEquipmentDropInSlotDelegate;
	FOnEquipmentRemoveFromSlot OnEquipmentRemoveFromSlotDelegate;

	void InitializeEquipmentSlot(TWeakObjectPtr<AEquipableItem> Equipment, int32 Index);
	void UpdateView();

protected:
	UPROPERTY(meta=(BindWidget))
	UImage* Icon;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* TBWeaponName;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UInventorySlotWidget> DragAndDropWidgetClass;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

private:
	TWeakObjectPtr<AEquipableItem> LinkedEquipableItem;

	TWeakObjectPtr<UWeaponInventoryItem> AdapterLinkedInventoryItem;

	int32 SlotIndexInComponent = 0;
	
};
