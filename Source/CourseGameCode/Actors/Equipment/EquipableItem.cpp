// Fill out your copyright notice in the Description page of Project Settings.


#include "EquipableItem.h"
#include "Characters/CGCBaseCharacter.h"


AEquipableItem::AEquipableItem()
{
	SetReplicates(true);
}

void AEquipableItem::SetOwner(AActor* NewOwner)
{
	Super::SetOwner(NewOwner);
	 if(IsValid(NewOwner))
	 {
	 	checkf(GetOwner()-IsA<ACGCBaseCharacter>(), TEXT("AEquipableItem::SetOwner() can work only with ACGCBaseCharacter"))
	 	CachedBaseCharacter = StaticCast<ACGCBaseCharacter*>(GetOwner());
	 	if(GetLocalRole() == ROLE_Authority)
	 	{
	 		SetAutonomousProxy(true);
	 	}
	 }
	 else
	 {
	 	CachedBaseCharacter = nullptr;
	 }
}

EEquipableItemType AEquipableItem::GetItemType() const
{
	return ItemType;
}

FName AEquipableItem::GetUnEquipSocketName() const
{
	return UnEquipSocketName;
}

FName AEquipableItem::GetEquipSocketName() const
{
	return EquipSocketName;
}

UAnimMontage* AEquipableItem::GetCharacterEquipAnimMontage() const
{
	return CharacterEquipAnimMontage;
}

EReticleType AEquipableItem::GetReticleType() const
{
	return ReticleType;
}

ACGCBaseCharacter* AEquipableItem::GetCharacterOwner() const
{
	return CachedBaseCharacter.IsValid() ? CachedBaseCharacter.Get() : nullptr;
}

FName AEquipableItem::GetDataTableId() const
{
	return DataTableID;
}

bool AEquipableItem::IsSlotCompatable(EEquipmentSlots Slots)
{
	return CompetableEquipmentSlots.Contains(Slots);
}
