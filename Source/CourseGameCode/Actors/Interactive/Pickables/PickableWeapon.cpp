// Fill out your copyright notice in the Description page of Project Settings.


#include "PickableWeapon.h"

#include "CGCCollisionTypes.h"
#include "Characters/CGCBaseCharacter.h"
#include "Engine/DataTable.h"
#include "Inventory/Items/InventoryItem.h"
#include "Inventory/Items/Weapons/WeaponInventoryItem.h"
#include "Utility/CGCDataTablesUtils.h"

APickableWeapon::APickableWeapon()
{
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);
}

void APickableWeapon::Interact(ACGCBaseCharacter* Character)
{
	FWeaponTableRow* WeaponRow = CGCDataTablesUtils::FindWeaponData(DataTableID);
	if(WeaponRow)
	{
		TWeakObjectPtr<UWeaponInventoryItem> Weapon = NewObject<UWeaponInventoryItem>(Character);
		Weapon->Initialize(DataTableID, WeaponRow->WeaponItemDescription);
		Weapon->SetEquipWeaponClass(WeaponRow->EquipableActor);
		Character->PickUp(Weapon);
		Destroy();
	}
}

FName APickableWeapon::GetActionEventName() const
{
	return ActionInteract;
}
