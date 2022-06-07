// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifyState_EndHardLanding.h"

#include "CourseGameCode/Characters/CGCBaseCharacter.h"

void UAnimNotifyState_EndHardLanding::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::NotifyEnd(MeshComp, Animation);

	ACGCBaseCharacter* BaseCharacter = Cast<ACGCBaseCharacter>(MeshComp->GetOwner());
	if(IsValid(BaseCharacter))
	{
		BaseCharacter->SetIgnoreLookMoveInput(false);
	}
}
