// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UI/Widgets/Inventory/InventoryWidget.h"
#include "CharacterInventoryComponent.generated.h"

class UInventoryItem;

USTRUCT(BlueprintType)
struct FInventorySlot
{
	GENERATED_BODY()

	DECLARE_DELEGATE(FInventorySlotUpdateDelegate);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TWeakObjectPtr<UInventoryItem> Item;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Count = 0;

	void BindOnInventorySlotUpdateDelegate(const FInventorySlotUpdateDelegate& Callback) const;
	void UnBindOnInventorySlotUpdateDelegate();
	void UpdateSlot();
	void ClearSlot();

private:
	mutable FInventorySlotUpdateDelegate OnInventorySlotUpdateDelegate;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class COURSEGAMECODE_API UCharacterInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	void OpenWidget(APlayerController* PlayerController);
	void CloseWidget();

	bool IsWidgetVisible() const;
	bool HasFreeSlots() const;
	
	bool AddItem(TWeakObjectPtr<UInventoryItem> ItemToAdd, int32 Amount);
	bool RemoveItem(FName ItemID);

	void DebugItemsInArray();
	
	int32 GetCapacity();

	TArray<FInventorySlot> GetAllItemsCopy() const;
	TArray<FName> GetAllItemsNames() const;

protected:
	
	virtual void BeginPlay() override;

	void CreateViewWidget(APlayerController* PlayerController);

	FInventorySlot* FindItemSlot(FName ItemID);

	FInventorySlot* FindFreeSlot();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Items")
	TArray<FInventorySlot> InventorySlots;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="View settings")
	TSubclassOf<UInventoryWidget> InventoryWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inventory settings", meta=(ClampMin = 1, UIMin = 1))
	int32 Capacity = 16;

private:
	UPROPERTY()
	UInventoryWidget* InventoryWidget;

	int32 ItemsInInventory;

		
};
