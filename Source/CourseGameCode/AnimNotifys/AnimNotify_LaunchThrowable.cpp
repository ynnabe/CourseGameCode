// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_LaunchThrowable.h"
#include "Characters/CGCBaseCharacter.h"

void UAnimNotify_LaunchThrowable::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	ACGCBaseCharacter* CharacterOwner = Cast<ACGCBaseCharacter>(MeshComp->GetOwner());
	if(!IsValid(CharacterOwner))
	{
		return;
	}

	CharacterOwner->GetCharacterEquipmentComponent_Mutable()->LaunchCurrentThrowableItem();
}
