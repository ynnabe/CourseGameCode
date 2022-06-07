// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterInventoryComponent.h"
#include "Inventory/Items/InventoryItem.h"


void FInventorySlot::BindOnInventorySlotUpdateDelegate(const FInventorySlotUpdateDelegate& Callback) const
{
	OnInventorySlotUpdateDelegate = Callback;
}

void FInventorySlot::UnBindOnInventorySlotUpdateDelegate()
{
	OnInventorySlotUpdateDelegate.Unbind();
}

void FInventorySlot::UpdateSlot()
{
	if(OnInventorySlotUpdateDelegate.IsBound())
	{
		OnInventorySlotUpdateDelegate.Execute();
	}
}

void FInventorySlot::ClearSlot()
{
	Item = nullptr;
	Count = 0;
	UpdateSlot();
}

void UCharacterInventoryComponent::OpenWidget(APlayerController* PlayerController)
{
	if(!IsValid(InventoryWidget))
	{
		CreateViewWidget(PlayerController);
	}

	if(!InventoryWidget->IsVisible())
	{
		InventoryWidget->AddToViewport();
	}
}

void UCharacterInventoryComponent::CloseWidget()
{
	if(InventoryWidget->IsVisible())
	{
		InventoryWidget->RemoveFromParent();
	}
}

bool UCharacterInventoryComponent::IsWidgetVisible() const
{
	bool Result = false;
	if(IsValid(InventoryWidget))
	{
		Result = InventoryWidget->IsVisible();
	}
	return Result;
}

bool UCharacterInventoryComponent::HasFreeSlots() const
{
	return ItemsInInventory < Capacity;
}

bool UCharacterInventoryComponent::AddItem(TWeakObjectPtr<UInventoryItem> ItemToAdd, int32 Amount)
{
	if(!ItemToAdd.IsValid() || Amount < 0)
	{
		return false;
	}

	bool Result = false;
	FInventorySlot* FreeSlot = FindFreeSlot();
	if(FreeSlot != nullptr)
	{
		FreeSlot->Item = ItemToAdd;
		FreeSlot->Count = Amount;
		ItemsInInventory++;
		Result = true;
		FreeSlot->UpdateSlot();
	}
	
	return Result;
}

bool UCharacterInventoryComponent::RemoveItem(FName ItemID)
{
	FInventorySlot* ItemSlot = FindItemSlot(ItemID);
	if(ItemSlot != nullptr)
	{
		InventorySlots.RemoveAll([=](const FInventorySlot& Slot) { return Slot.Item->GetDataTableId() == ItemID; });
		ItemsInInventory--;
		return true;
	}
	return false;
}

void UCharacterInventoryComponent::DebugItemsInArray()
{
	for(FName Name : GetAllItemsNames())
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, FString::Printf(TEXT("Items: %s"), *Name.ToString()));
	}
}

int32 UCharacterInventoryComponent::GetCapacity()
{
	return Capacity;
}

TArray<FInventorySlot> UCharacterInventoryComponent::GetAllItemsCopy() const
{
	return InventorySlots;
}

TArray<FName> UCharacterInventoryComponent::GetAllItemsNames() const
{
	TArray<FName> ResultArray;
	for(const FInventorySlot& Slot : InventorySlots)
	{
		if(Slot.Item.IsValid())
		{
			ResultArray.Add(Slot.Item->GetItemDescription().Name);
		}
	}

	return ResultArray;
}

void UCharacterInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	InventorySlots.AddDefaulted(Capacity);
	
}

void UCharacterInventoryComponent::CreateViewWidget(APlayerController* PlayerController)
{
	if(IsValid(InventoryWidget))
	{
		return;
	}
	
	if(!IsValid(PlayerController) || !IsValid(InventoryWidgetClass))
	{
		return;
	}
	

	InventoryWidget = CreateWidget<UInventoryWidget>(PlayerController, InventoryWidgetClass);
	InventoryWidget->InitializeWidget(InventorySlots);
}

FInventorySlot* UCharacterInventoryComponent::FindItemSlot(FName ItemID)
{
	return InventorySlots.FindByPredicate([=](const FInventorySlot& Slot) { return Slot.Item->GetDataTableId() == ItemID; });
}

FInventorySlot* UCharacterInventoryComponent::FindFreeSlot()
{
	return InventorySlots.FindByPredicate([=](const FInventorySlot& Slot) { return !Slot.Item.IsValid(); });
}

