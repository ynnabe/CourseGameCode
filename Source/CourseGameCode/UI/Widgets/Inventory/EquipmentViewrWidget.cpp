// Fill out your copyright notice in the Description page of Project Settings.


#include "EquipmentViewrWidget.h"

void UEquipmentViewrWidget::InitializeEquipmentWidget(UCharacterEquipmentComponent* EquipmentComponent)
{
	LinkedEquipmentComponent = EquipmentComponent;
	const TArray<AEquipableItem*>& Items = LinkedEquipmentComponent->GetItems();
	for(int32 Index = 1; Index < Items.Num(); ++Index)
	{
		AddWeaponSlotView(Items[Index], Index);
	}
}

void UEquipmentViewrWidget::AddWeaponSlotView(AEquipableItem* LinkToWeapon, int32 SlotIndex)
{
	UWeaponSlotWidget* SlotWidget = CreateWidget<UWeaponSlotWidget>(this, DefaultSlotViewClass);

	if(IsValid(SlotWidget))
	{
		SlotWidget->InitializeEquipmentSlot(LinkToWeapon, SlotIndex);
		VBWeaponSlots->AddChildToVerticalBox(SlotWidget);
		SlotWidget->UpdateView();
		SlotWidget->OnEquipmentDropInSlotDelegate.BindUObject(this, &UEquipmentViewrWidget::EquipWeaponToSlot);
		SlotWidget->OnEquipmentRemoveFromSlotDelegate.BindUObject(this, &UEquipmentViewrWidget::RemoveWeaponFromSlot);
	}
}

void UEquipmentViewrWidget::UpdateSlot(int32 SlotIndex)
{
	UWeaponSlotWidget* WidgetToUpdate = Cast<UWeaponSlotWidget>(VBWeaponSlots->GetChildAt(SlotIndex - 1));
	if(IsValid(WidgetToUpdate))
	{
		WidgetToUpdate->InitializeEquipmentSlot(LinkedEquipmentComponent->GetItems()[SlotIndex], SlotIndex);
		WidgetToUpdate->UpdateView();
	}
}

bool UEquipmentViewrWidget::EquipWeaponToSlot(const TSubclassOf<AEquipableItem>& WeaponClass, int32 SenderIndex)
{
	const bool Result = LinkedEquipmentComponent->AddEquipmentItemInSlot(WeaponClass, SenderIndex);
	if(Result)
	{
		UpdateSlot(SenderIndex);
	}
	return Result;
}

void UEquipmentViewrWidget::RemoveWeaponFromSlot(int32 SlotIndex)
{
	LinkedEquipmentComponent->RemoveEquipmentItemFromSlot(SlotIndex);
}
