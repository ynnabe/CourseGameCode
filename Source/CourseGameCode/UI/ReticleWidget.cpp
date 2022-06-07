
// Fill out your copyright notice in the Description page of Project Settings.


#include "ReticleWidget.h"

#include "Actors/Equipment/EquipableItem.h"

void UReticleWidget::OnEquipedItemChanged_Implementation(const AEquipableItem* EquipableItem)
{
	CurrentEquipedItem = EquipableItem;
	SetupCurrentReticle();
}

void UReticleWidget::OnAimStateChanged_Implementation(bool bIsAiming)
{
	SetupCurrentReticle();
}

void UReticleWidget::SetupCurrentReticle()
{
	ReticleType = CurrentEquipedItem.IsValid() ? CurrentEquipedItem->GetReticleType() : EReticleType::None;
}
