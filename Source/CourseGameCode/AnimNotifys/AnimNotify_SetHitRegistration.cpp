// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_SetHitRegistration.h"
#include "Characters/CGCBaseCharacter.h"

void UAnimNotify_SetHitRegistration::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);
	ACGCBaseCharacter* CharacterOwner = Cast<ACGCBaseCharacter>(MeshComp->GetOwner());
	if(!IsValid(CharacterOwner))
	{
		return;
	}

	AMeleeWeaponItem* MeleeWeaponItem = CharacterOwner->GetCharacterEquipmentComponent()->GetCurrentMeleeWeapon();
	if(IsValid(MeleeWeaponItem))
	{
		MeleeWeaponItem->SetIsHitregistrationEnabled(bIsHitRegistrationEnabled);
	}
}
