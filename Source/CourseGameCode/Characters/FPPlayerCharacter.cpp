// Fill out your copyright notice in the Description page of Project Settings.


#include "FPPlayerCharacter.h"

#include "Actors/Equipment/RangeWeaponItems/RangeWeaponItem.h"
#include "Components/CapsuleComponent.h"
#include "Controllers/CGCPlayerController.h"
#include "CourseGameCode/CGCCollisionTypes.h"
#include "CourseGameCode/Actors/Interactive/Environment/Ladder.h"
#include "CourseGameCode/Actors/Interactive/Environment/Zipline.h"
#include "CourseGameCode/Components/MovementComponents/GCBaseCharacterMovementComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

AFPPlayerCharacter::AFPPlayerCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	FirstPersonMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("First Person Mesh"));
	FirstPersonMeshComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonMeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, -86.0f));
	FirstPersonMeshComponent->CastShadow = false;
	FirstPersonMeshComponent->bCastDynamicShadow = false;
	FirstPersonMeshComponent->SetOnlyOwnerSee(true);
	
	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("First Person Camera"));
	FirstPersonCamera->SetupAttachment(FirstPersonMeshComponent, SocketFPCamera);
	FirstPersonCamera->bUsePawnControlRotation = true;
	
	GetMesh()->SetOwnerNoSee(true);
	GetMesh()->bCastHiddenShadow = true;

	Camera->bAutoActivate = false;
	SpringArm->bAutoActivate = false;
	SpringArm->bUsePawnControlRotation = false;

	GetCharacterMovement()->bOrientRotationToMovement = false;

	bUseControllerRotationYaw = true;
}

void AFPPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	PlayerController = Cast<ACGCPlayerController>(NewController);
}

void AFPPlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if(IsFPMontagePlaying() && PlayerController.IsValid())
	{
		FRotator TargetControlRotation = PlayerController->GetControlRotation();
		TargetControlRotation.Pitch = 0.0f;
		float BlendSpeed = 600.0f;
		TargetControlRotation = FMath::RInterpTo(PlayerController->GetControlRotation(), TargetControlRotation, DeltaSeconds, BlendSpeed);
		PlayerController->SetControlRotation(TargetControlRotation);
	}
}

void AFPPlayerCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	const AFPPlayerCharacter* DefaultCharacter = GetDefault<AFPPlayerCharacter>(GetClass());
	FVector& FirstPersonMeshRelativeLocation = FirstPersonMeshComponent->GetRelativeLocation_DirectMutable();
	FirstPersonMeshRelativeLocation.Z = DefaultCharacter->FirstPersonMeshComponent->GetRelativeLocation().Z + HalfHeightAdjust;
}

void AFPPlayerCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	const AFPPlayerCharacter* DefaultCharacter = GetDefault<AFPPlayerCharacter>(GetClass());
	FVector& FirstPersonMeshRelativeLocation = FirstPersonMeshComponent->GetRelativeLocation_DirectMutable();
	FirstPersonMeshRelativeLocation.Z = DefaultCharacter->FirstPersonMeshComponent->GetRelativeLocation().Z;
}

FRotator AFPPlayerCharacter::GetViewRotation() const
{
	FRotator Result = Super::GetViewRotation();
	
	if(IsFPMontagePlaying())
	{
		FRotator SocketRotation = FirstPersonMeshComponent->GetSocketRotation(SocketFPCamera);
		Result.Pitch += SocketRotation.Pitch;
		Result.Yaw = SocketRotation.Yaw;
		Result.Roll = SocketRotation.Roll;
	}
	
	return Result;
}

void AFPPlayerCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);
	if(PlayerController.IsValid())
	{
		if(GetBaseCharacterMovementComponent()->IsOnLadder())
		{
			const ALadder* CurrentLadder = GetBaseCharacterMovementComponent()->GetCurrentLadder();
			FVector CurrentLadderForwardVector = -CurrentLadder->GetActorForwardVector();
			FRotator CurrentLadderRotation = CurrentLadderForwardVector.ToOrientationRotator();
			PlayerController->SetIgnoreCameraPitch(true);
			bUseControllerRotationYaw = false;
			APlayerCameraManager* PlayerCameraManager = PlayerController->PlayerCameraManager;
			PlayerCameraManager->ViewPitchMin = CurrentLadderRotation.Pitch - LadderCameraMinPitch;
			PlayerCameraManager->ViewPitchMax = CurrentLadderRotation.Pitch + LadderCameraMaxPitch;
			PlayerCameraManager->ViewYawMin = CurrentLadderRotation.Yaw - LadderCameraMinYaw;
			PlayerCameraManager->ViewYawMax = CurrentLadderRotation.Yaw + LadderCameraMaxYaw;
		}
		else if(PreviousCustomMode == (uint8)ECustomMovementMode::CMOVE_OnLadder)
		{
		
			APlayerCameraManager* PlayerCameraManager = PlayerController->PlayerCameraManager;
			APlayerCameraManager* DefaultCameraManager = PlayerCameraManager->GetClass()->GetDefaultObject<APlayerCameraManager>();
			PlayerController->SetIgnoreCameraPitch(false);
			bUseControllerRotationYaw = true;
			PlayerCameraManager->ViewPitchMin = DefaultCameraManager->ViewPitchMin;
			PlayerCameraManager->ViewPitchMax = DefaultCameraManager->ViewPitchMax;
			PlayerCameraManager->ViewYawMin = DefaultCameraManager->ViewYawMin;
			PlayerCameraManager->ViewYawMax = DefaultCameraManager->ViewYawMax;
		
		}
		if(GetBaseCharacterMovementComponent()->IsOnZipline())
		{
		
			const AZipline* CurrentZipline = GetBaseCharacterMovementComponent()->GetCurrentZipline();
			FVector CurrentZiplineCabelDirection = CurrentZipline->GetCabelDirection();
			FRotator CurrentZiplineCabelRotator = CurrentZiplineCabelDirection.ToOrientationRotator();
			PlayerController->SetIgnoreCameraPitch(true);
			bUseControllerRotationYaw = false;
			APlayerCameraManager* PlayerCameraManager = PlayerController->PlayerCameraManager;
			PlayerCameraManager->ViewPitchMin = CurrentZiplineCabelRotator.Pitch - ZiplineCameraMinPitch;
			PlayerCameraManager->ViewPitchMax = CurrentZiplineCabelRotator.Pitch + ZiplineCameraMaxPitch;
			PlayerCameraManager->ViewYawMin = CurrentZiplineCabelRotator.Yaw - ZiplineCameraMinYaw;
			PlayerCameraManager->ViewYawMax = CurrentZiplineCabelRotator.Yaw + ZiplineCameraMaxYaw;
		}
		else if(PreviousCustomMode == (uint8)ECustomMovementMode::CMOVE_OnZipline)
		{
		
			APlayerCameraManager* PlayerCameraManager = PlayerController->PlayerCameraManager;
			APlayerCameraManager* DefaultCameraManager = PlayerCameraManager->GetClass()->GetDefaultObject<APlayerCameraManager>();
			PlayerController->SetIgnoreCameraPitch(false);
			bUseControllerRotationYaw = true;
			PlayerCameraManager->ViewPitchMin = DefaultCameraManager->ViewPitchMin;
			PlayerCameraManager->ViewPitchMax = DefaultCameraManager->ViewPitchMax;
			PlayerCameraManager->ViewYawMin = DefaultCameraManager->ViewYawMin;
			PlayerCameraManager->ViewYawMax = DefaultCameraManager->ViewYawMax;
		}
	}
}

void AFPPlayerCharacter::OnMantle(const FMantlingSettings& MantlingSettings, float MantlingAnimationStartTime)
{
	Super::OnMantle(MantlingSettings, MantlingAnimationStartTime);
	UAnimInstance* FPAnimInstance = FirstPersonMeshComponent->GetAnimInstance();
	if(IsValid(FPAnimInstance) && IsValid(MantlingSettings.FPMantlingMontage))
	{
		if(PlayerController.IsValid())
		{
			PlayerController->SetIgnoreLookInput(true);
			PlayerController->SetIgnoreMoveInput(true);
		}
		float MontageDuration = FPAnimInstance->Montage_Play(MantlingSettings.FPMantlingMontage, 1.0f, EMontagePlayReturnType::Duration, MantlingAnimationStartTime);
		GetWorld()->GetTimerManager().SetTimer(FPMontageTimerHandle, this, &AFPPlayerCharacter::OnFPMontageTimerElapsed, MontageDuration, false);
	}
}

bool AFPPlayerCharacter::IsFPMontagePlaying() const
{
	UAnimInstance* FPAnimInstance = FirstPersonMeshComponent->GetAnimInstance();
	return (IsValid(FPAnimInstance) && FPAnimInstance->IsAnyMontagePlaying());
}

void AFPPlayerCharacter::OnStartAimingInternal()
{
	Super::OnStartAimingInternal();
	UE_LOG(LogTemp, Warning, TEXT("DA"));
	APlayerController* FPPlayerController = GetController<APlayerController>();
	if(!IsValid(FPPlayerController))
	{
		return;
	}
	APlayerCameraManager* CameraManager = FPPlayerController->PlayerCameraManager;
	if(IsValid(CameraManager))
	{
		ARangeWeaponItem* CurrentRangeWeapon = GetCharacterEquipmentComponent()->GetCurrentRangeWeapon();
		CameraManager->SetFOV(CurrentRangeWeapon->GetAimFov());
	}
}

void AFPPlayerCharacter::OnStopAimingInternal()
{
	Super::OnStopAimingInternal();
	APlayerController* FPPlayerController = GetController<APlayerController>();
	if(!IsValid(FPPlayerController))
	{
		return;
	}
	APlayerCameraManager* CameraManager = FPPlayerController->PlayerCameraManager;
	if(IsValid(CameraManager))
	{
		ARangeWeaponItem* CurrentRangeWeapon = GetCharacterEquipmentComponent()->GetCurrentRangeWeapon();
		CameraManager->UnlockFOV();
	}
}

void AFPPlayerCharacter::OnFPMontageTimerElapsed()
{
	if(PlayerController.IsValid())
	{
		PlayerController->SetIgnoreLookInput(false);
		PlayerController->SetIgnoreMoveInput(false);
	}
}
