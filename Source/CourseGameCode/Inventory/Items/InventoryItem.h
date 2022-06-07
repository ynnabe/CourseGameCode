// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/CGCBaseCharacter.h"
#include "Engine/DataTable.h"
#include "InventoryItem.generated.h"

class APickableItem;
class AEquipableItem;
class UInventoryItem;

USTRUCT(BlueprintType)
struct FInventoryItemDescription : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item view")
	FName Name;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item view")
	UTexture2D* Icon;
	
};

USTRUCT()
struct FWeaponTableRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon view")
	TSubclassOf<APickableItem> PickableActor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon view")
	TSubclassOf<AEquipableItem> EquipableActor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon view")
	FInventoryItemDescription WeaponItemDescription;
};

USTRUCT(BlueprintType)
struct FItemTableRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item view")
	TSubclassOf<APickableItem> PickableActorClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item view")
	FInventoryItemDescription ItemDescription;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item view")
	TSubclassOf<UInventoryItem> InventoryItemClass;
	
};

UCLASS(Blueprintable)
class COURSEGAMECODE_API UInventoryItem : public UObject
{
	GENERATED_BODY()

public:

	void Initialize(FName DataTableId_in,const FInventoryItemDescription Description_in );
	
	const FName& GetDataTableId() const;
	const FInventoryItemDescription GetItemDescription() const;

	virtual bool GetIsConsumable() const;
	virtual bool GetIsEquipable() const;

	virtual bool Consume(ACGCBaseCharacter* ConsumeTarget) PURE_VIRTUAL(UInventoryItem::Consume, return false;);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inventory item")
	FName DataTableID = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inventory item")
	FInventoryItemDescription Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inventory item")
	bool bIsConsumable = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inventory item")
	bool bIsEquipable = false;

private:
	bool bIsInitialized = false;
};
