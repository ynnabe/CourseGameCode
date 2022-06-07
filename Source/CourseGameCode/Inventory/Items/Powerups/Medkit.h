// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/Items/InventoryItem.h"
#include "Medkit.generated.h"

/**
 * 
 */
UCLASS()
class COURSEGAMECODE_API UMedkit : public UInventoryItem
{
	GENERATED_BODY()

public:

	virtual bool Consume(ACGCBaseCharacter* ConsumeTarget) override;

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Medkit")
	float HealthRestore = 100.0f;

	
	
};
