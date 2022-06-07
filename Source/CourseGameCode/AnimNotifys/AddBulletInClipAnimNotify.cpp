// Fill out your copyright notice in the Description page of Project Settings.


#include "AddBulletInClipAnimNotify.h"

#include "Characters/CGCBaseCharacter.h"

void UAddBulletInClipAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);
	
	ACGCBaseCharacter* CharacterOwner = Cast<ACGCBaseCharacter>(MeshComp->GetOwner());
	if(!IsValid(CharacterOwner))
	{
		return;
	}

	CharacterOwner->GetCharacterEquipmentComponent_Mutable()->ReloadAmmoInCurrentWeapon(BulletsAmount, true);
}
