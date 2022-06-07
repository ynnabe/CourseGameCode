// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_EndHardLanding.h"

#include "CourseGameCode/Characters/CGCBaseCharacter.h"

void UAnimNotify_EndHardLanding::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	ACGCBaseCharacter* BaseCharacter = Cast<ACGCBaseCharacter>(MeshComp->GetOwner());
	if(IsValid(BaseCharacter))
	{
		BaseCharacter->SetIgnoreLookMoveInput(false);
	}
}
