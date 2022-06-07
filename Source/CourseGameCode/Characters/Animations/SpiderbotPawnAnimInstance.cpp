// Fill out your copyright notice in the Description page of Project Settings.


#include "SpiderbotPawnAnimInstance.h"
#include "CourseGameCode/Characters/SpiderbotPawn.h"

void USpiderbotPawnAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	checkf(TryGetPawnOwner()->IsA<ASpiderbotPawn>(), TEXT("USpiderbotPawnAnimInstance can used with SpiderbotPawnAnimInstance only"));
	CachedSpiderbotPawnOwner = StaticCast<ASpiderbotPawn*>(TryGetPawnOwner());
}

void USpiderbotPawnAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if(!CachedSpiderbotPawnOwner.IsValid())
	{
		return;
	}
	
	RightFrontEffectorLocation = FVector(CachedSpiderbotPawnOwner->GetRightFrontFootOffset(), 0.0f, 0.0f);
	RightRearEffectorLocation = FVector(CachedSpiderbotPawnOwner->GetRightRearFootOffset(), 0.0f, 0.0f);
	LeftFrontEffectorLocation = FVector(CachedSpiderbotPawnOwner->GetLeftFrontFootOffset(), 0.0f, 0.0f);
	LeftRearEffectorLocation = FVector(CachedSpiderbotPawnOwner->GetLeftRearFootOffset(), 0.0f, 0.0f);
}
