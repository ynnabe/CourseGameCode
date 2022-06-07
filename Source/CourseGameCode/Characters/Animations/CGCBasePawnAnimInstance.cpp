// Fill out your copyright notice in the Description page of Project Settings.


#include "CGCBasePawnAnimInstance.h"

#include "CourseGameCode/Characters/CourseGameCodePawn.h"
#include "CourseGameCode/Components/MovementComponents/CGCBaseMovementComponent.h"

void UCGCBasePawnAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	checkf(TryGetPawnOwner()->IsA<ACourseGameCodePawn>(), TEXT("UCGCBasePawnAnimInstance::NativeBeginPlay() only CourseGameCodePawnBase can work with UCGCBasePawnAnimInstance"))
	CachedBasePawn = StaticCast<ACourseGameCodePawn*>(TryGetPawnOwner());
}

void UCGCBasePawnAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if(!CachedBasePawn.IsValid())
	{
		return;
	}

	InputForward = CachedBasePawn->GetInputForwrd();
	InputRight = CachedBasePawn->GetInputRight();
	bIsInAir = CachedBasePawn->GetMovementComponent()->IsFalling();
}
