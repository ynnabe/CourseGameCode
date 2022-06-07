// Fill out your copyright notice in the Description page of Project Settings.


#include "AttachEquipedAnimNotify.h"

#include "Characters/CGCBaseCharacter.h"

void UAttachEquipedAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	ACGCBaseCharacter* CharacterOwner = Cast<ACGCBaseCharacter>(MeshComp->GetOwner());
	if(!IsValid(CharacterOwner))
	{
		return;
	}

	CharacterOwner->GetCharacterEquipmentComponent_Mutable()->AttachCurrentItemToEquipSocket();
}
