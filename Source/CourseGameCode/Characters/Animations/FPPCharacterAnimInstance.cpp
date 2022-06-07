// Fill out your copyright notice in the Description page of Project Settings.


#include "FPPCharacterAnimInstance.h"

#include "CourseGameCode/Characters/FPPlayerCharacter.h"

void UFPPCharacterAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	checkf(TryGetPawnOwner()->IsA<AFPPlayerCharacter>(), TEXT("UFPPCharacterAnimInstance::NativeBeginPlay() only AFPPlayerCharacter can use UFPPCharacterAnimInstance"));
	CachedFPPlayerCharacterOwner = StaticCast<AFPPlayerCharacter*>(TryGetPawnOwner());
}

void UFPPCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if(!CachedFPPlayerCharacterOwner.IsValid())
	{
		return;
	}
	PlayerCameraPitchAngle = CalculateCameraPitchAngle();
	
}

float UFPPCharacterAnimInstance::CalculateCameraPitchAngle() const
{
	float Result = 0.0f;

	ACGCPlayerController* Controller = CachedFPPlayerCharacterOwner->GetController<ACGCPlayerController>();
	if(IsValid(Controller) && !Controller->GetIgnoreCameraPitch())
	{
		Result = Controller->GetControlRotation().Pitch;
	}

	return Result;
}
