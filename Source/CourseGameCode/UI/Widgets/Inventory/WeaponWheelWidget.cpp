// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponWheelWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Utility/CGCDataTablesUtils.h"

void UWeaponWheelWidget::InitializeWeaponWheelWidget(UCharacterEquipmentComponent* EquipmentComponent)
{
	LinkedEquipmentComponent = EquipmentComponent;
}

void UWeaponWheelWidget::NextSegment()
{
	CurrentSegmentIndex++;
	if(CurrentSegmentIndex == EquipmentSlotsSegments.Num())
	{
		CurrentSegmentIndex = 0;
	}
	SelectSegment();
}

void UWeaponWheelWidget::PreviosSegment()
{
	CurrentSegmentIndex--;
	if(CurrentSegmentIndex < 0)
	{
		CurrentSegmentIndex = EquipmentSlotsSegments.Num() - 1;
	}
	SelectSegment();
}

void UWeaponWheelWidget::ConfirmSelectedSegment()
{
	EEquipmentSlots SelectedSlot = EquipmentSlotsSegments[CurrentSegmentIndex];
	LinkedEquipmentComponent->EquipItemInSlot(SelectedSlot, false);
	RemoveFromParent();
}

void UWeaponWheelWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if(IsValid(RadialBackground) && !BackgroundMaterialInstance.IsValid())
	{
		BackgroundMaterialInstance = RadialBackground->GetDynamicMaterial();
		BackgroundMaterialInstance->SetScalarParameterValue(FName("Segments"), EquipmentSlotsSegments.Num());
	}

	for(int i = 0; i < EquipmentSlotsSegments.Num(); ++i)
	{
		FName WidgetName = FName(FString::Printf(TEXT("Image_Segment%i"), i));
		UImage* WeaponImage = WidgetTree->FindWidget<UImage>(WidgetName);
		if(!IsValid(WeaponImage))
		{
			continue;
		}

		FWeaponTableRow* WeaponData = GetTableRowForSegment(i);
		if(WeaponData == nullptr)
		{
			WeaponImage->SetOpacity(0.0f);
		}
		else
		{
			WeaponImage->SetBrushFromTexture(WeaponData->WeaponItemDescription.Icon);
		}
	}
}

void UWeaponWheelWidget::SelectSegment()
{
	BackgroundMaterialInstance->SetScalarParameterValue(FName("Index"), CurrentSegmentIndex);
	FWeaponTableRow* CurrentWeaponData = GetTableRowForSegment(CurrentSegmentIndex);
	if(CurrentWeaponData == nullptr)
	{
		WeaponNameText->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		WeaponNameText->SetVisibility(ESlateVisibility::Visible);
		const FName CurrentWeaponName = CurrentWeaponData->WeaponItemDescription.Name;
		WeaponNameText->SetText(FText::FromName(CurrentWeaponName));
	}
}

FWeaponTableRow* UWeaponWheelWidget::GetTableRowForSegment(int32 SegmentIndex) const
{
	const EEquipmentSlots& CurrentEquipmentSlot = EquipmentSlotsSegments[SegmentIndex];
	AEquipableItem* EquipableItem = LinkedEquipmentComponent->GetItems()[(int32)CurrentEquipmentSlot];
	if(!IsValid(EquipableItem))
	{
		WeaponNameText->SetVisibility(ESlateVisibility::Hidden);
		return nullptr;
	}
	
	return CGCDataTablesUtils::FindWeaponData(EquipableItem->GetDataTableId());
}
