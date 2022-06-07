// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponSlotWidget.h"
#include "Blueprint/UserWidget.h"
#include "Components/VerticalBox.h"
#include "EquipmentViewrWidget.generated.h"

/**
 * 
 */
class UCharacterEquipmentComponent;

UCLASS()
class COURSEGAMECODE_API UEquipmentViewrWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void InitializeEquipmentWidget(UCharacterEquipmentComponent* EquipmentComponent);

protected:
	void AddWeaponSlotView(AEquipableItem* LinkToWeapon, int32 SlotIndex);
	void UpdateSlot(int32 SlotIndex);

	bool EquipWeaponToSlot(const TSubclassOf<AEquipableItem>& WeaponClass, int32 SenderIndex);
	void RemoveWeaponFromSlot(int32 SlotIndex);

	UPROPERTY(meta=(BindWidget))
	UVerticalBox* VBWeaponSlots;

	UPROPERTY(EditDefaultsOnly, Category="ItemContainer View settings")
	TSubclassOf<UWeaponSlotWidget> DefaultSlotViewClass;

	TWeakObjectPtr<UCharacterEquipmentComponent> LinkedEquipmentComponent;
	
};
