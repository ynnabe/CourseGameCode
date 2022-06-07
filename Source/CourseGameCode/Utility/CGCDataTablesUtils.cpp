	// Fill out your copyright notice in the Description page of Project Settings.


#include "CGCDataTablesUtils.h"
#include "CGCCollisionTypes.h"
#include "Inventory/Items/InventoryItem.h"


FWeaponTableRow* CGCDataTablesUtils::FindWeaponData(FName WeaponID)
{
	static const FString contextString(TEXT("Find weapon data"));

	UDataTable* WeaponDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/CourseGameCode/Core/Data/DataTables/DT_WeaponList.DT_WeaponList"));

	if(WeaponDataTable == nullptr)
	{
		return nullptr;
	}

	return WeaponDataTable->FindRow<FWeaponTableRow>(WeaponID, contextString);
}

FItemTableRow* CGCDataTablesUtils::FindInventoryItemData(const FName ItemId)
{
	static const FString contextString(TEXT("Find item data"));

	UDataTable* InventoryItemsDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/CourseGameCode/Core/Data/DataTables/DT_InventoryItemList.DT_InventoryItemList"));

	if(InventoryItemsDataTable == nullptr)
	{
		return nullptr;
	}

	return InventoryItemsDataTable->FindRow<FItemTableRow>(ItemId, contextString);
}
