// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/Items/InventoryItem.h"
#include "WeaponInventoryItem.generated.h"

/**
 * 
 */
class AEquipableItem;
UCLASS()
class COURSEGAMECODE_API UWeaponInventoryItem : public UInventoryItem
{
	GENERATED_BODY()

public:

	UWeaponInventoryItem();

	void SetEquipWeaponClass(TSubclassOf<AEquipableItem>& EquipableItem);
	TSubclassOf<AEquipableItem> GetEquipWeaponClass() const;

protected:
	TSubclassOf<AEquipableItem> EquipWeaponClass;
	
};
