// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Interactive/Pickables/PickableItem.h"
#include "Inventory/Items/InventoryItem.h"
#include "PickablesPowerUps.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class COURSEGAMECODE_API APickablesPowerUps : public APickableItem
{
	GENERATED_BODY()

public:
    APickablesPowerUps();
	
	virtual void Interact(ACGCBaseCharacter* Character) override;
	virtual FName GetActionEventName() const override;

protected:

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* MeshComponent;

	UPROPERTY()
	TWeakObjectPtr<UInventoryItem> ItemToAdd;
	
};
