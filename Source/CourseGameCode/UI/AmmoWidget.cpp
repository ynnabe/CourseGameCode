// Fill out your copyright notice in the Description page of Project Settings.


#include "AmmoWidget.h"

#include "Actors/Equipment/RangeWeaponItems/RangeWeaponItem.h"
#include "Characters/CGCBaseCharacter.h"

void UAmmoWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	// ACGCBaseCharacter* BaseCharacter = Cast<ACGCBaseCharacter>(GetOwningPlayerPawn());
	// if(BaseCharacter->GetLocalRole() == ROLE_Authority)
	// {
	// 	UCharacterEquipmentComponent* CharacterEquipmentComponent = BaseCharacter->GetCharacterEquipmentComponent_Mutable();
	// 	CharacterEquipmentComponent->OnAmmoCountChanged.AddUObject(this, &UAmmoWidget::UpdateTotalAmmoCount);
	// 	CharacterEquipmentComponent->OnCurrentWeaponAmmoChangedEvent.AddUObject(this, &UAmmoWidget::UpdateAmmoCountInCurrentWeapon);
	// 	CharacterEquipmentComponent->OnFireModeChangedDelegate.AddUObject(this, &UAmmoWidget::UpdateFireModeIcon);
	// }
}

void UAmmoWidget::SubscribeDelegates(ACGCBaseCharacter* Character)
{
	if(!IsValid(Character))
	{
		return;
	}

	UCharacterEquipmentComponent* CharacterEquipmentComponent = Character->GetCharacterEquipmentComponent_Mutable();
	CharacterEquipmentComponent->OnAmmoCountChanged.AddUObject(this, &UAmmoWidget::UpdateTotalAmmoCount);
	CharacterEquipmentComponent->OnCurrentWeaponAmmoChangedEvent.AddUObject(this, &UAmmoWidget::UpdateAmmoCountInCurrentWeapon);
	CharacterEquipmentComponent->OnFireModeChangedDelegate.AddUObject(this, &UAmmoWidget::UpdateFireModeIcon);
}

void UAmmoWidget::UpdateTotalAmmoCount(EAmmoType AmmoType, int32 NewTotalAmmo)
{
	if(AmmoType == EAmmoType::FragGrenades)
	{
		FragGrenade = NewTotalAmmo;
	}
	else
	{
		TotalAmmo = NewTotalAmmo;
	}
}

void UAmmoWidget::UpdateAmmoCountInCurrentWeapon(int32 NewAmmo)
{
	Ammo = NewAmmo;
}

void UAmmoWidget::UpdateFireModeIcon(EWeaponFireMode NewFireMode)
{
	switch(NewFireMode)
	{
	case EWeaponFireMode::Single:
		{
			CurrentFireModeIcon = SingeFireModeIcon;
			break;
		}
	case EWeaponFireMode::FullAuto:
		{
			CurrentFireModeIcon = FullAutoFireModeIcon;
			break;
		}
	}
	
}
