// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryItem.h"

void UInventoryItem::Initialize(FName DataTableId_in, const FInventoryItemDescription Description_in)
{
	DataTableID = DataTableId_in;
	Description.Icon = Description_in.Icon;
	Description.Name = Description_in.Name;
	bIsInitialized = true;
}

const FName& UInventoryItem::GetDataTableId() const
{
	return DataTableID;
}

const FInventoryItemDescription UInventoryItem::GetItemDescription() const
{
	return Description;
}

bool UInventoryItem::GetIsConsumable() const
{
	return bIsConsumable;
}

bool UInventoryItem::GetIsEquipable() const
{
	return bIsEquipable;
}
