// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CGCCollisionTypes.h"
#include "Blueprint/UserWidget.h"
#include "Inventory/Items/InventoryItem.h"
#include "WeaponWheelWidget.generated.h"

class UTextBlock;
class UImage;
class UCharacterEquipmentComponent;
/**
 * 
 */
UCLASS()
class COURSEGAMECODE_API UWeaponWheelWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	
	void InitializeWeaponWheelWidget(UCharacterEquipmentComponent* EquipmentComponent);

	void NextSegment();
	void PreviosSegment();
	void ConfirmSelectedSegment();

protected:

	virtual void NativeConstruct() override;
	void SelectSegment();
	
	UPROPERTY(meta=(BindWidget))
	UImage* RadialBackground;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* WeaponNameText;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon wheel settings")
	TArray<EEquipmentSlots> EquipmentSlotsSegments;

private:
	FWeaponTableRow* GetTableRowForSegment(int32 SegmentIndex) const;
	
	int32 CurrentSegmentIndex;
	TWeakObjectPtr<UMaterialInstanceDynamic> BackgroundMaterialInstance;
	TWeakObjectPtr<UCharacterEquipmentComponent> LinkedEquipmentComponent;
	
};
