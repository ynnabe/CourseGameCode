// Fill out your copyright notice in the Description page of Project Settings.


#include "PickablesPowerUps.h"

#include "CGCCollisionTypes.h"
#include "Inventory/Items/InventoryItem.h"
#include "Utility/CGCDataTablesUtils.h"

APickablesPowerUps::APickablesPowerUps()
{
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	SetRootComponent(MeshComponent);
}

void APickablesPowerUps::Interact(ACGCBaseCharacter* Character)
{
	FItemTableRow* ItemData = CGCDataTablesUtils::FindInventoryItemData(GetDataTableID());

	if(ItemData == nullptr)
	{
		return;
	}

	ItemToAdd = TWeakObjectPtr<UInventoryItem>(NewObject<UInventoryItem>(Character, ItemData->InventoryItemClass));
	ItemToAdd->Initialize(DataTableID, ItemData->ItemDescription);

	const bool bPickedUp = Character->PickUp(ItemToAdd);
	if(bPickedUp)
	{
		Destroy();
	}
}

FName APickablesPowerUps::GetActionEventName() const
{
	return ActionInteract;
}
