// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_EndSlide.h"

#include "CourseGameCode/Characters/CGCBaseCharacter.h"
#include "CourseGameCode/Components/MovementComponents/GCBaseCharacterMovementComponent.h"

void UAnimNotify_EndSlide::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	ACGCBaseCharacter* BaseCharacter = Cast<ACGCBaseCharacter>(MeshComp->GetOwner());
	if(IsValid(BaseCharacter))
	{
		BaseCharacter->GetBaseCharacterMovementComponent()->StopSlide();
	}
}
