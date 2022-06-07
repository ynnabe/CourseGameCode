// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CGCCollisionTypes.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/CharacterComponents/CharacterEquipmentComponent.h"
#include "Components/Weapon/WeaponBarrelComponent.h"
#include "AmmoWidget.generated.h"

/**
 * 
 */
UCLASS()
class COURSEGAMECODE_API UAmmoWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeOnInitialized() override;

	void SubscribeDelegates(ACGCBaseCharacter* Character);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ammo")
	int32 Ammo;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ammo")
	int32 TotalAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ammo")
	int32 FragGrenade;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Ammo | Icon")
	UTexture* CurrentFireModeIcon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ammo | Icon")
	UTexture* SingeFireModeIcon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ammo | Icon")
	UTexture* FullAutoFireModeIcon;
	

private:
	
	void UpdateTotalAmmoCount(EAmmoType AmmoType, int32 NewTotalAmmo);

	void UpdateAmmoCountInCurrentWeapon(int32 NewAmmo);

	void UpdateFireModeIcon(EWeaponFireMode NewFireMode);
};
