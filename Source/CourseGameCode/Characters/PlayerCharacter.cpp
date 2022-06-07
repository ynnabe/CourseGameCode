// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"

#include "Actors/Equipment/RangeWeaponItems/RangeWeaponItem.h"
#include "CourseGameCode/Components/MovementComponents/GCBaseCharacterMovementComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"

APlayerCharacter::APlayerCharacter(const FObjectInitializer& ObjectInitializer) :
Super(ObjectInitializer)
{
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring arm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->bUsePawnControlRotation = 1;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	Team = ETeams::Player;
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	DefaultSpringArmLength = SpringArm->TargetArmLength;
	if(IsValid(SprintArmLengthCurveFloat))
	{
		FOnTimelineFloatStatic SpringArmLengthDelegate;
		SpringArmLengthDelegate.BindUObject(this, &APlayerCharacter::SprintArmLengthTimelineUpdate);
		SpringArmLengthTimeline.AddInterpFloat(SprintArmLengthCurveFloat, SpringArmLengthDelegate);
	}
	if(IsValid(AimFOVCurveFloat))
	{
		FOnTimelineFloatStatic AimFOVDelegate;
		AimFOVDelegate.BindUObject(this, &APlayerCharacter::AimFOVTimelineUpdate);
		AimFOVTimeline.AddInterpFloat(AimFOVCurveFloat, AimFOVDelegate);
	}
}

void APlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	SpringArmLengthTimeline.TickTimeline(DeltaSeconds);
	AimFOVTimeline.TickTimeline(DeltaSeconds);
}

void APlayerCharacter::MoveForward(float Value)
{
	Super::MoveForward(Value);
	if((GetCharacterMovement()->IsMovingOnGround() || GetCharacterMovement()->IsFalling()) && !FMath::IsNearlyZero(Value, 1e-6f) && !CGCBaseCharacterMovementComponent->IsSliding())
	{
		FRotator YawRotator(0.0f, GetControlRotation().Yaw, 0.0f);
		FVector ForwardVector = YawRotator.RotateVector(FVector::ForwardVector);
		AddMovementInput(ForwardVector, Value);
	}
}

void APlayerCharacter::MoveRight(float Value)
{
	Super::MoveRight(Value);
	if((GetCharacterMovement()->IsMovingOnGround() || GetCharacterMovement()->IsFalling()) && !FMath::IsNearlyZero(Value, 1e-6f) && !CGCBaseCharacterMovementComponent->IsSliding())
	{
		FRotator YawRotator(0.0f, GetControlRotation().Yaw, 0.0f);
		FVector RightVector = YawRotator.RotateVector(FVector::RightVector);
		AddMovementInput(RightVector, Value);
	}
}

void APlayerCharacter::TurnUp(float Value)
{
	Super::TurnUp(Value);
	if(IsAiming())
	{
		ARangeWeaponItem* CurrentRangeWeapon = GetCharacterEquipmentComponent()->GetCurrentRangeWeapon();
		AddControllerPitchInput(Value * CurrentRangeWeapon->GetAimLookUpModifier());
	}
	else
	{
		AddControllerPitchInput(Value);
	}
}

void APlayerCharacter::TurnRight(float Value)
{
	Super::TurnRight(Value);
	if(IsAiming())
	{
		ARangeWeaponItem* CurrentRangeWeapon = GetCharacterEquipmentComponent()->GetCurrentRangeWeapon();
		AddControllerYawInput(Value * CurrentRangeWeapon->GetAimTurnModifier());
	}
	else
	{
		AddControllerYawInput(Value);
	}
}

void APlayerCharacter::SwimForward(float Value)
{
	Super::SwimForward(Value);
	if(GetMovementComponent()->IsSwimming() && !FMath::IsNearlyZero(Value, 1e-6f))
	{
		FRotator PitchYawRotator(0.0f, GetControlRotation().Yaw, 0.0f);
		FVector ForwardVector = PitchYawRotator.RotateVector(FVector::ForwardVector);
		AddMovementInput(ForwardVector, Value);
	}
}

void APlayerCharacter::SwimRight(float Value)
{
	Super::SwimRight(Value);
	if(GetMovementComponent()->IsSwimming() && !FMath::IsNearlyZero(Value, 1e-6f))
	{
		FRotator YawRotator(0.0f, GetControlRotation().Yaw, 0.0f);
		FVector RightVector = YawRotator.RotateVector(FVector::RightVector);
		AddMovementInput(RightVector, Value);
	}
}

void APlayerCharacter::SwimUp(float Value)
{
	Super::SwimUp(Value);
	if(GetMovementComponent()->IsSwimming() && !FMath::IsNearlyZero(Value, 1e-6f))
	{
		AddMovementInput(FVector::UpVector, Value);
	}
}

void APlayerCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	SpringArm->TargetOffset += FVector(0.0f, 0.0f, SpringArmCrouchTargetOffset);
}

void APlayerCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	SpringArm->TargetOffset -= FVector(0.0f, 0.0f, SpringArmCrouchTargetOffset);
}

bool APlayerCharacter::CanJumpInternal_Implementation() const
{
	if(CGCBaseCharacterMovementComponent->IsOutOfStamina())
	{
		return false;
	}
	else
	{
		return !CGCBaseCharacterMovementComponent->IsMantling() && bIsCrouched || Super::CanJumpInternal_Implementation();
	}
}

void APlayerCharacter::OnJumped_Implementation()
{
	if(bIsCrouched)
	{
		UnCrouch();
	}
}

void APlayerCharacter::OnSprintStart_Implementation()
{
	Super::OnSprintStart_Implementation();
	SpringArmLengthTimeline.Play();
}

void APlayerCharacter::OnSprintEnd_Implementation()
{
	Super::OnSprintEnd_Implementation();
	SpringArmLengthTimeline.Reverse();
}

void APlayerCharacter::OnStartProne(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnStartProne(HalfHeightAdjust, ScaledHalfHeightAdjust);
	SpringArm->TargetOffset += FVector(0.f, 0.f, SpringArmProneTargetOffset);
}

void APlayerCharacter::OnEndProne(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnEndProne(HalfHeightAdjust, ScaledHalfHeightAdjust);
	SpringArm->TargetOffset -= FVector(0.f, 0.f, SpringArmProneTargetOffset);
}

void APlayerCharacter::SprintArmLengthTimelineUpdate(const float Alpha)
{
	const float SpringArmLength = FMath::Lerp(DefaultSpringArmLength, MaxSpringArmLength, Alpha);
	SpringArm->TargetArmLength = SpringArmLength;
}

void APlayerCharacter::AimFOVTimelineUpdate(const float Alpha)
{
	APlayerController* FPPlayerController = GetController<APlayerController>();
	if(!IsValid(FPPlayerController))
	{
		return;
	}
	
	APlayerCameraManager* CameraManager = FPPlayerController->PlayerCameraManager;
	ARangeWeaponItem* CurrentRangeWeapon = GetCharacterEquipmentComponent()->GetCurrentRangeWeapon();
	if(IsValid(CameraManager) && IsValid(CurrentRangeWeapon))
	{
		if(IsValid(AimFOVCurveFloat))
		{
			const float AimFOV = FMath::Lerp(CameraManager->DefaultFOV, CurrentRangeWeapon->GetAimFov(), Alpha);
			CameraManager->SetFOV(AimFOV);
		}
		else
		{
			CameraManager->SetFOV(CurrentRangeWeapon->GetAimFov());
		}
	}
}

void APlayerCharacter::OnStartAimingInternal()
{
	Super::OnStartAimingInternal();
	if(IsValid(AimFOVCurveFloat))
	{
		AimFOVTimeline.Play();
	}
	else
	{
		APlayerController* FPPlayerController = GetController<APlayerController>();
		if(!IsValid(FPPlayerController))
		{
			return;
		}
	
		APlayerCameraManager* CameraManager = FPPlayerController->PlayerCameraManager;
		ARangeWeaponItem* CurrentRangeWeapon = GetCharacterEquipmentComponent()->GetCurrentRangeWeapon();
		if(IsValid(CameraManager) && IsValid(CurrentRangeWeapon))
		{
			CameraManager->SetFOV(CurrentRangeWeapon->GetAimFov());
		}
	}
	
}

void APlayerCharacter::OnStopAimingInternal()
{
	Super::OnStopAimingInternal();
	if(IsValid(AimFOVCurveFloat))
	{
		AimFOVTimeline.Reverse();
	}
	else
	{
		APlayerController* FPPlayerController = GetController<APlayerController>();
		if(!IsValid(FPPlayerController))
		{
			return;
		}
	
		APlayerCameraManager* CameraManager = FPPlayerController->PlayerCameraManager;
		if(IsValid(CameraManager))
		{
			CameraManager->UnlockFOV();
		}
	}
}
