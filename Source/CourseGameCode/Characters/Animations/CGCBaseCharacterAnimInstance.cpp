// Fill out your copyright notice in the Description page of Project Settings.


#include "CGCBaseCharacterAnimInstance.h"

#include "Actors/Equipment/RangeWeaponItems/RangeWeaponItem.h"
#include "CourseGameCode/Characters/CGCBaseCharacter.h"
#include "CourseGameCode/Components/MovementComponents/GCBaseCharacterMovementComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

void UCGCBaseCharacterAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	checkf(TryGetPawnOwner()->IsA<ACGCBaseCharacter>(), TEXT("UCGCBaseCharacterAnimInstance::NativeBeginPlay can work only with ACGCBaseCharacter"));
	CachedBaseCharacter = StaticCast<ACGCBaseCharacter*>(TryGetPawnOwner());
	CachedCharacterAttributesComponent = CachedBaseCharacter->GetCharacterAttributeComponent_Mutable();
	if(CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->OnChangingSlideState.BindUObject(this, &UCGCBaseCharacterAnimInstance::ChangeSlideState);
		CachedBaseCharacter->OnHardLanding.BindUObject(this, &UCGCBaseCharacterAnimInstance::PlayHardLanding);
		CachedCharacterAttributesComponent->OnDeathEvent.AddUObject(this, &UCGCBaseCharacterAnimInstance::PlayDeathForwardMontage);
	}
}

void UCGCBaseCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if(!CachedBaseCharacter.IsValid())
	{
		return;
	}
	UGCBaseCharacterMovementComponent* BaseCharacterMovementComponent = CachedBaseCharacter->GetBaseCharacterMovementComponent();
	Speed = BaseCharacterMovementComponent->Velocity.Size();
	bIsFalling = BaseCharacterMovementComponent->IsFalling();
	bIsCrouching = BaseCharacterMovementComponent->IsCrouching();
	bIsSprinting = BaseCharacterMovementComponent->GetIsSprinting();
	bIsOutOfStamina = BaseCharacterMovementComponent->IsOutOfStamina();
	bIsProning = BaseCharacterMovementComponent->IsProning();
	bIsSwimming = BaseCharacterMovementComponent->IsSwimming();
	bIsOnLadder = BaseCharacterMovementComponent->IsOnLadder();
	if(bIsOnLadder)
	{
		LadderSpeedRatio = BaseCharacterMovementComponent->GetLadderSpeedRatio();
	}

	bIsStrafing = !BaseCharacterMovementComponent->bOrientRotationToMovement;
	Direction = CalculateDirection(BaseCharacterMovementComponent->Velocity, CachedBaseCharacter->GetActorRotation());
		
	bIsOnZipline = BaseCharacterMovementComponent->IsOnZipline();
	bIsWallRunning = BaseCharacterMovementComponent->IsWallRuning();
	CurrentWallRunSide = BaseCharacterMovementComponent->GetSide();

	LeftFootEffectorLocation = FVector(-(CachedBaseCharacter->GetIKLeftFootOffset() + CachedBaseCharacter->GetIKPelvisOffset()), 0.0f, 0.0f);
	RightFootEffectorLocation = FVector((CachedBaseCharacter->GetIKRightFootOffset() + CachedBaseCharacter->GetIKPelvisOffset()), 0.0f , 0.0f);
	PelvisOffset = FVector(0.0f, 0.0f, CachedBaseCharacter->GetIKPelvisOffset());
	PronePelvisOffset = CachedBaseCharacter->GetPronePelvisOffset();

	const UCharacterEquipmentComponent* CharacterEquipmentComponent = CachedBaseCharacter->GetCharacterEquipmentComponent();
	CurrentEquippedItem = CharacterEquipmentComponent->GetCurrentEquippedItemType();

	AimRotation = CachedBaseCharacter->GetAimOffset();

	ARangeWeaponItem* CurrentRangeWeapon = CharacterEquipmentComponent->GetCurrentRangeWeapon();
	if(IsValid(CurrentRangeWeapon))
	{
		ForeGripSocketTransform = CurrentRangeWeapon->GetForeGripTransform();
	}

	bIsAiming = CachedBaseCharacter->IsAiming();
}

void UCGCBaseCharacterAnimInstance::ChangeSlideState(bool State)
{
	if(State)
	{
		Montage_Play(SlideMontage, 1.0f, EMontagePlayReturnType::Duration);
	}
	else
	{
		Montage_Stop(0.5f, SlideMontage);
	}
}

void UCGCBaseCharacterAnimInstance::PlayHardLanding()
{
	Montage_Play(HardLandingMontage);
}

void UCGCBaseCharacterAnimInstance::PlayDeathForwardMontage()
{
	Montage_Play(OnDeathForward, 1.0f, EMontagePlayReturnType::Duration);
}
