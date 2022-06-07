// Fill out your copyright notice in the Description page of Project Settings.


#include "GCBaseCharacterMovementComponent.h"

#include "DrawDebugHelpers.h"
#include "Components/CapsuleComponent.h"
#include "CourseGameCode/CGCCollisionTypes.h"
#include "CourseGameCode/Actors/Interactive/Environment/Ladder.h"
#include "CourseGameCode/Actors/Interactive/Environment/Zipline.h"
#include "Curves/CurveVector.h"
#include "ProfilingDebugging/CookStats.h"

#define FLAG_IsSprinting FLAG_Custom_0;
#define FLAG_IsMantling FLAG_Custom_1;
#define FLAG_IsSliding FLAG_Custom_2;

void UGCBaseCharacterMovementComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

FNetworkPredictionData_Client* UGCBaseCharacterMovementComponent::GetPredictionData_Client() const
{
	if(ClientPredictionData == nullptr)
	{
		UGCBaseCharacterMovementComponent* MutableThis = const_cast<UGCBaseCharacterMovementComponent*>(this);
		MutableThis->ClientPredictionData = new FNetworkPredictionData_Client_CharacterCGC(*this);
	}
	return ClientPredictionData;
}

void UGCBaseCharacterMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);
	
	bool bWasMantling = BaseCharacterOwner->bIsMantling;
	bool bWasSliding = BaseCharacterOwner->bIsSliding;
	bIsSprinting = (Flags & FSavedMove_Character::FLAG_Custom_0) != 0;
	bool bIsMantling = (Flags & FSavedMove_Character::FLAG_Custom_1) != 0;
	bool bSliding  = (Flags & FSavedMove_Character::FLAG_Custom_2) != 0;

	if(BaseCharacterOwner->GetLocalRole() == ROLE_Authority)
	{
		if(!bWasMantling && bIsMantling)
		{
			BaseCharacterOwner->Mantle(true);
		}
		if(!bWasSliding && bSliding)
		{
			BaseCharacterOwner->Slide(true);
		}
	}
	
}

void UGCBaseCharacterMovementComponent::SetIsOutOfStamina(bool bIsOutOfStamina_in)
{
	bIsOutOfStamina = bIsOutOfStamina_in;
	StopSprint();
}

float UGCBaseCharacterMovementComponent::GetMaxSpeed() const
{
	float Result = Super::GetMaxSpeed();
	if(bIsSprinting)
	{
		Result = SprintSpeed;
	}
	else if(bIsOutOfStamina)
	{
		Result = OutOfStaminaSpeed;
	}
	else if(IsProning())
	{
		Result = ProneSpeed;
	}
	else if(IsOnLadder())
	{
		Result = ClimbingOnLadderSpeed;
	}
	else if(BaseCharacterOwner->bIsSliding)
	{
		Result = SlideSpeed;
	}
	// else if(BaseCharacterOwner->IsAiming())
	// {
	//  	Result = BaseCharacterOwner->GetAimingMovementSpeed();
	// }
	return Result;
}

void UGCBaseCharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();
	BaseCharacterOwner = StaticCast<ACGCBaseCharacter*>(GetOwner());

	CharacterOwner->GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &UGCBaseCharacterMovementComponent::OnPlayerCapsuleHit);
}

void UGCBaseCharacterMovementComponent::StartSprint()
{
	bIsSprinting = true;
	bOrientRotationToMovement = true;
	bForceMaxAccel = 1;
}

void UGCBaseCharacterMovementComponent::StopSprint()
{
	bIsSprinting = false;
	bOrientRotationToMovement = false;
	bForceMaxAccel = 0;
}

void UGCBaseCharacterMovementComponent::StartMantle(const FMantlingMovementParameters& MantlingParameters)
{
	CurrentMantlingParameters = MantlingParameters;
	SetMovementMode(EMovementMode::MOVE_Custom, (uint8)ECustomMovementMode::CMOVE_Mantling);
}

void UGCBaseCharacterMovementComponent::EndMantle()
{
	BaseCharacterOwner->bIsMantling = false;
	SetMovementMode(MOVE_Walking);
}

bool UGCBaseCharacterMovementComponent::IsMantling() const
{
	return UpdatedComponent && MovementMode == MOVE_Custom && CustomMovementMode == (uint8)ECustomMovementMode::CMOVE_Mantling;
}

void UGCBaseCharacterMovementComponent::StartWallRun()
{
	SetMovementMode(EMovementMode::MOVE_Custom, (uint8)ECustomMovementMode::CMOVE_WallRun);
	GetWorld()->GetTimerManager().SetTimer(WallRunTimer, this, &UGCBaseCharacterMovementComponent::StopWallRun, WallRunTime, false);
}

void UGCBaseCharacterMovementComponent::StopWallRun()
{
	SetMovementMode(MOVE_Falling);
}

bool UGCBaseCharacterMovementComponent::IsWallRuning() const
{
	return UpdatedComponent && MovementMode == MOVE_Custom && CustomMovementMode == (uint8)ECustomMovementMode::CMOVE_WallRun;
}

void UGCBaseCharacterMovementComponent::AttachToLadder(const ALadder* Ladder)
{
	CurrentLadder = Ladder;
	FRotator TargetOrientationRotation = CurrentLadder->GetActorForwardVector().ToOrientationRotator();
	TargetOrientationRotation.Yaw += 180.0f;

	FVector LadderUpVector = CurrentLadder->GetActorUpVector();
	FVector LadderForwardVector = CurrentLadder->GetActorForwardVector();
	float Projection = GetActorToCurrentLadderProjection(GetActorLocation());

	FVector NewCharacterLocation = CurrentLadder->GetActorLocation() + Projection * LadderUpVector + LadderToCharacterOffset * LadderForwardVector;
	if(CurrentLadder->GetIsOnTop())
	{
		NewCharacterLocation = CurrentLadder->GetAttachFromTopAnimMontageStartingLocation();
	}
	GetOwner()->SetActorLocation(NewCharacterLocation);
	GetOwner()->SetActorRotation(TargetOrientationRotation);
	SetMovementMode(MOVE_Custom, (uint8)ECustomMovementMode::CMOVE_OnLadder);
}

void UGCBaseCharacterMovementComponent::DetachFromLadder(EDetachFromLadderMethod DetachFromLadderMethod)
{
	switch (DetachFromLadderMethod)
	{
	case EDetachFromLadderMethod::JumpOff:
		{
			FVector JumpDirection = CurrentLadder->GetActorForwardVector();
			SetMovementMode(MOVE_Falling);
			FVector JumpVelocity = JumpDirection * JumpOffFromLadderSpeed;
			Launch(JumpVelocity);
			break;
		}
	case EDetachFromLadderMethod::ReachingTheTop:
		{
			BaseCharacterOwner->Mantle(true);
			break;
		}
	case EDetachFromLadderMethod::ReachingTheBottom:
		{
			SetMovementMode(MOVE_Walking);
			break;
		}
	case EDetachFromLadderMethod::Fall:
	default:
		{
			SetMovementMode(MOVE_Falling);
			break;
		}
	}
}

bool UGCBaseCharacterMovementComponent::IsOnLadder() const
{
	return UpdatedComponent && MovementMode == MOVE_Custom && CustomMovementMode == (uint8)ECustomMovementMode::CMOVE_OnLadder;
}

const ALadder* UGCBaseCharacterMovementComponent::GetCurrentLadder()
{
	return CurrentLadder;
}

void UGCBaseCharacterMovementComponent::PhysicsRotation(float DeltaTime)
{
	if(bForceRotation)
	{
		FRotator CurrentRotation = UpdatedComponent->GetComponentRotation(); // Normalized
		CurrentRotation.DiagnosticCheckNaN(TEXT("CharacterMovementComponent::PhysicsRotation(): CurrentRotation"));

		FRotator DeltaRot = GetDeltaRotation(DeltaTime);
		DeltaRot.DiagnosticCheckNaN(TEXT("CharacterMovementComponent::PhysicsRotation(): GetDeltaRotation"));

		const float AngleTolerance = 1e-3f;

		if (!CurrentRotation.Equals(ForceTargetRotation, AngleTolerance))
		{
			FRotator DesiredRotation = ForceTargetRotation;
			// PITCH
			if (!FMath::IsNearlyEqual(CurrentRotation.Pitch, DesiredRotation.Pitch, AngleTolerance))
			{
				DesiredRotation.Pitch = FMath::FixedTurn(CurrentRotation.Pitch, DesiredRotation.Pitch, DeltaRot.Pitch);
			}

			// YAW
			if (!FMath::IsNearlyEqual(CurrentRotation.Yaw, DesiredRotation.Yaw, AngleTolerance))
			{
				DesiredRotation.Yaw = FMath::FixedTurn(CurrentRotation.Yaw, DesiredRotation.Yaw, DeltaRot.Yaw);
			}

			// ROLL
			if (!FMath::IsNearlyEqual(CurrentRotation.Roll, DesiredRotation.Roll, AngleTolerance))
			{
				DesiredRotation.Roll = FMath::FixedTurn(CurrentRotation.Roll, DesiredRotation.Roll, DeltaRot.Roll);
			}

			// Set the new rotation.
			DesiredRotation.DiagnosticCheckNaN(TEXT("CharacterMovementComponent::PhysicsRotation(): DesiredRotation"));
			MoveUpdatedComponent( FVector::ZeroVector, DesiredRotation, /*bSweep*/ false );
		}
		else
		{
			ForceTargetRotation = FRotator::ZeroRotator;
			bForceRotation = false;
		}
		return;
	}
	if(IsOnLadder())
	{
		return;
	}
	Super::PhysicsRotation(DeltaTime);
}

float UGCBaseCharacterMovementComponent::GetLadderSpeedRatio() const
{
	checkf(IsValid(CurrentLadder), TEXT("UGCBaseCharacterMovementComponent::GetLadderSpeedRatio can not be invoked when current ladder is null"));

	FVector LadderUpVector = CurrentLadder->GetActorUpVector();
	return FVector::DotProduct(LadderUpVector, Velocity) / ClimbingOnLadderSpeed;
}

void UGCBaseCharacterMovementComponent::AttachToZipline(const AZipline* Zipline)
{
	CurrentZipline = Zipline;
	FRotator TargetOrientationRotation = CurrentZipline->GetCabel()->GetForwardVector().ToOrientationRotator();
	
	if(TargetOrientationRotation.Pitch > 0.0f)
	{
		TargetOrientationRotation.Yaw += 180.0f;
	}
	
	TargetOrientationRotation.Pitch = 0.0f;
	
	float Projection = GetActorToCurrentZiplineProjection(GetActorLocation());
	FVector NewCharacterLocation = CurrentZipline->GetCabel()->GetComponentLocation() + Projection * CurrentZipline->GetCabel()->GetForwardVector() - ZiplineToCharacterOffset * CurrentZipline->GetCabel()->GetUpVector();

	GetOwner()->SetActorLocation(NewCharacterLocation);
	GetOwner()->SetActorRotation(TargetOrientationRotation);
	SetMovementMode(MOVE_Custom, (uint8)ECustomMovementMode::CMOVE_OnZipline);
}

void UGCBaseCharacterMovementComponent::DetachFromZipline()
{
	SetMovementMode(MOVE_Falling);
}

bool UGCBaseCharacterMovementComponent::IsOnZipline() const
{
	return UpdatedComponent && MovementMode == MOVE_Custom && CustomMovementMode == (uint8)ECustomMovementMode::CMOVE_OnZipline; 
}

const AZipline* UGCBaseCharacterMovementComponent::GetCurrentZipline()
{
	return CurrentZipline;
}

EWallRunSide UGCBaseCharacterMovementComponent::GetSide()
{
	return CurrentWallRunsSide;
}

FVector UGCBaseCharacterMovementComponent::GetWallRunDirection()
{
	return WallRunDirection;
}

float UGCBaseCharacterMovementComponent::GetJumpOffFromWall()
{
	return JumpOffFromWall;
}

void UGCBaseCharacterMovementComponent::StartSlide()
{
	const float ComponentScale = CharacterOwner->GetCapsuleComponent()->GetShapeScale();
	const float OldUnscaledRadius = CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleRadius();
	const float OldUnscaledHalfHeight = CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	const float ClampedSlidedHalfHeight = FMath::Max3(0.f, OldUnscaledRadius, SlideCapsuleHalfHeight);
	CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(OldUnscaledRadius, ClampedSlidedHalfHeight);
	float HalfHeightAdjust = OldUnscaledHalfHeight - SlideCapsuleHalfHeight;
	float ScaledHalfHeightAdjust = HalfHeightAdjust * ComponentScale;
	UpdatedComponent->MoveComponent(FVector(0.f,0.f, -ScaledHalfHeightAdjust), UpdatedComponent->GetComponentQuat(), true, nullptr, EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);

	ACharacter* DefaultCharacter = CharacterOwner->GetClass()->GetDefaultObject<ACharacter>();
	HalfHeightAdjust = (DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() - SlideCapsuleHalfHeight - ClampedSlidedHalfHeight);
	ScaledHalfHeightAdjust = HalfHeightAdjust * ComponentScale;
	BaseCharacterOwner->bIsSliding = true;

	BaseCharacterOwner->OnStartSlide(HalfHeightAdjust, ScaledHalfHeightAdjust);
	SetMovementMode(MOVE_Custom, (uint8)ECustomMovementMode::CMOVE_Slide);
}

void UGCBaseCharacterMovementComponent::StopSlide()
{
	ACharacter* DefaultCharacter = CharacterOwner->GetClass()->GetDefaultObject<ACharacter>();
	float ComponentScale = CharacterOwner->GetCapsuleComponent()->GetShapeScale();
	float DefaultHalfHeight = DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	float HalfHeightAdjust = DefaultHalfHeight - CrouchedHalfHeight;
	float ScaleHalfHeightAdjust = HalfHeightAdjust * ComponentScale;
	CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleRadius(), DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight());
	BaseCharacterOwner->bIsSliding = false;
	SetMovementMode(MOVE_Walking);
	if(IsNotEnoughSpaceToStandUp())
	{
		Crouch();
		BaseCharacterOwner->OnEndSlide(HalfHeightAdjust, ScaleHalfHeightAdjust);
	}
	else
	{
		BaseCharacterOwner->OnEndSlide(0.0f, 0.0f);
	}
}

bool UGCBaseCharacterMovementComponent::IsSliding()
{
	return bIsSliding;
}

void UGCBaseCharacterMovementComponent::Prone()
{
	if(!HasValidData())
	{
		return;
	}

	if(!CanProneInCurrentState())
	{
		return;
	}

	if(CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() == ProneCapsuleHalfHeight)
	{
		BaseCharacterOwner->bIsProned = true;
		bIsWantsToProne = false;
		BaseCharacterOwner->bIsCrouched = false;
		BaseCharacterOwner->OnStartProne(0.f, 0.f);
		return;
	}

	const float ComponentScale = CharacterOwner->GetCapsuleComponent()->GetShapeScale();
	const float OldUnscaledHalfHeight = CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	const float OldUnscaledRadius = CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleRadius();
	const float ClampedPronedHalfHeight = FMath::Max3(0.f, OldUnscaledRadius, ProneCapsuleHalfHeight);
	CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(OldUnscaledRadius, ClampedPronedHalfHeight);
	float HalfHeightAdjust = (OldUnscaledHalfHeight - ClampedPronedHalfHeight);
	float ScaledHalfHeightAdjust = HalfHeightAdjust * ComponentScale;

	if(ClampedPronedHalfHeight > OldUnscaledHalfHeight)
	{
		FCollisionQueryParams CapsuleParams(SCENE_QUERY_STAT(ProneTrace), false, CharacterOwner);
		FCollisionResponseParams ResponseParam;
		InitCollisionParams(CapsuleParams, ResponseParam);
		const bool bEncroached = GetWorld()->OverlapBlockingTestByChannel(UpdatedComponent->GetComponentLocation() - FVector(0.f, 0.f, ScaledHalfHeightAdjust), FQuat::Identity,
			UpdatedComponent->GetCollisionObjectType(), GetPawnCapsuleCollisionShape(SHRINK_None), CapsuleParams, ResponseParam);

		if(bEncroached)
		{
			CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(OldUnscaledRadius, OldUnscaledHalfHeight);
			return;
		}
	}

	if(bProneMaintainsBaseLocation)
	{
		UpdatedComponent->MoveComponent(FVector(0.f,0.f, -ScaledHalfHeightAdjust), UpdatedComponent->GetComponentQuat(), true, nullptr, EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
	}

	BaseCharacterOwner->bIsProned = true;
	bWantsToCrouch = false;
	BaseCharacterOwner->bIsCrouched = false;

	bForceNextFloorCheck = true;

	ACharacter* DefaultCharacter = CharacterOwner->GetClass()->GetDefaultObject<ACharacter>();
	HalfHeightAdjust = (DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() - CrouchedHalfHeight - ClampedPronedHalfHeight);
	ScaledHalfHeightAdjust = HalfHeightAdjust * ComponentScale;
	
	BaseCharacterOwner->OnStartProne(HalfHeightAdjust, ScaledHalfHeightAdjust);
}

void UGCBaseCharacterMovementComponent::UnProne()
{
	if(!HasValidData())
	{
		return;
	}

	ACharacter* DefaultCharacter = CharacterOwner->GetClass()->GetDefaultObject<ACharacter>();
	float DesiredHeight = bIsFullHeight ?
		DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() : CrouchedHalfHeight;

	if(CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() == DesiredHeight)
	{
		BaseCharacterOwner->bIsProned = false;
		bWantsToCrouch = !bIsFullHeight;
		BaseCharacterOwner->bIsCrouched = !bIsFullHeight;
		BaseCharacterOwner->OnEndProne(0.f, 0.f);
		return;
	}

	const float CurrentPronedHalfHeight = CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	const float ComponentScale = CharacterOwner->GetCapsuleComponent()->GetShapeScale();
	const float OldUnscaledHalfHeight = CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	float HalfHeightAdjust = DesiredHeight - OldUnscaledHalfHeight;
	float ScaledHalfHeightAdjust = HalfHeightAdjust * ComponentScale;
	const FVector PawnLocation = UpdatedComponent->GetComponentLocation();

	check(CharacterOwner->GetCapsuleComponent());

	const UWorld* MyWorld = GetWorld();
	const float SweepInflation = KINDA_SMALL_NUMBER * 10.f;
	FCollisionQueryParams CapsuleParams(SCENE_QUERY_STAT(ProneTrace), false, CharacterOwner);
	FCollisionResponseParams ResponseParam;
	InitCollisionParams(CapsuleParams, ResponseParam);

	const FCollisionShape StandingCapsuleShape = GetPawnCapsuleCollisionShape(SHRINK_HeightCustom, -SweepInflation - ScaledHalfHeightAdjust);
	const ECollisionChannel CollisionChannel = UpdatedComponent->GetCollisionObjectType();
	bool bEncroached = true;

	if(!bProneMaintainsBaseLocation)
	{
		bEncroached = MyWorld->OverlapBlockingTestByChannel(PawnLocation, FQuat::Identity, CollisionChannel, StandingCapsuleShape, CapsuleParams, ResponseParam);

		if(bEncroached)
		{
			if(ScaledHalfHeightAdjust > 0.f)
			{
				float PawnRadius, PawnHalfHeight;
				CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleSize(PawnRadius, PawnHalfHeight);
				const float ShrinkHalfHeight = PawnHalfHeight - PawnRadius;
				const float TraceDist = PawnHalfHeight - ShrinkHalfHeight;
				const FVector Down = FVector(0.f, 0.f, -TraceDist);

				FHitResult Hit(1.f);
				const FCollisionShape ShortCapsuleShape = GetPawnCapsuleCollisionShape(SHRINK_HeightCustom, ShrinkHalfHeight);
				const bool bBlockingHit = MyWorld->SweepSingleByChannel(Hit, PawnLocation, PawnLocation + Down, FQuat::Identity, CollisionChannel, ShortCapsuleShape, CapsuleParams);
				if(Hit.bStartPenetrating)
				{
					bEncroached = true;
				}
				else
				{
					const float DistanceToBase = (Hit.Time * TraceDist) + ShortCapsuleShape.Capsule.HalfHeight;
					const FVector NewLoc = FVector(PawnLocation.X, PawnLocation.Y, PawnLocation.Z - DistanceToBase + StandingCapsuleShape.Capsule.HalfHeight + SweepInflation + MIN_FLOOR_DIST / 2.f);
					bEncroached = MyWorld->OverlapBlockingTestByChannel(NewLoc, FQuat::Identity, CollisionChannel, StandingCapsuleShape, CapsuleParams, ResponseParam);
					if(!bEncroached)
					{
						UpdatedComponent->MoveComponent(NewLoc-PawnLocation, UpdatedComponent->GetComponentQuat(),false, nullptr, EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
					}
				}
			}
		}
	}
	else
	{
		FVector StandingLocation = PawnLocation + FVector(0.f, 0.f, StandingCapsuleShape.GetCapsuleHalfHeight() - CurrentPronedHalfHeight);
		bEncroached = MyWorld->OverlapBlockingTestByChannel(StandingLocation, FQuat::Identity, CollisionChannel, StandingCapsuleShape, CapsuleParams, ResponseParam);

		if(bEncroached)
		{
			if(IsMovingOnGround())
			{
				const float MinFloorDist = KINDA_SMALL_NUMBER * 10.0f;
				if(CurrentFloor.bBlockingHit && CurrentFloor.FloorDist > MinFloorDist)
				{
					StandingLocation.Z -= CurrentFloor.FloorDist - MinFloorDist;
					bEncroached = MyWorld->OverlapBlockingTestByChannel(StandingLocation, FQuat::Identity, CollisionChannel, StandingCapsuleShape, CapsuleParams, ResponseParam);
				}
			}
		}

		if(!bEncroached)
		{
			UpdatedComponent->MoveComponent(StandingLocation - PawnLocation, UpdatedComponent->GetComponentQuat(), false, nullptr, EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
			bForceNextFloorCheck = true;
		}
	}

	if(bEncroached)
	{
		return;
	}

	BaseCharacterOwner->bIsProned = false;
	bWantsToCrouch = !bIsFullHeight;
	BaseCharacterOwner->bIsCrouched = !bIsFullHeight;
	CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleRadius(), DesiredHeight, true);
	HalfHeightAdjust = bIsFullHeight ?
		CrouchedHalfHeight - ProneCapsuleHalfHeight : DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() - DesiredHeight - ProneCapsuleHalfHeight;
	ScaledHalfHeightAdjust = HalfHeightAdjust * ComponentScale;

	AdjustProxyCapsuleSize();
	BaseCharacterOwner->OnEndProne(HalfHeightAdjust, ScaledHalfHeightAdjust);
}

void UGCBaseCharacterMovementComponent::UpdateCharacterStateBeforeMovement(float DeltaSeconds)
{
	Super::UpdateCharacterStateBeforeMovement(DeltaSeconds);
	
	const bool bIsProning = IsProning();
	if(bIsProning && (!bIsWantsToProne || !CanProneInCurrentState()))
	{
		UnProne();
	}
	else if(!bIsProning && bIsWantsToProne && CanProneInCurrentState())
	{
		Prone();
	}
	
}

void UGCBaseCharacterMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode,
	uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);
	if(MovementMode == MOVE_Walking)
	{
		bProneMaintainsBaseLocation = true;
		CurrentWall = nullptr;
		bNotifyApex = true;
	}
	else
	{
		bProneMaintainsBaseLocation = false;
	}

	if(MovementMode == MOVE_Swimming)
	{
		CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(SwimmingCapsuleRadius, SwimmingCapsuleHalfHeight);
		bUseControllerDesiredRotation = true;
		bOrientRotationToMovement = false;
	}
	else if(PreviousMovementMode == MOVE_Swimming)
	{
		ACharacter* DefaultCharacter = CharacterOwner->GetClass()->GetDefaultObject<ACharacter>();
		CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleRadius(), DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight(), true);
		bUseControllerDesiredRotation = false;
		bOrientRotationToMovement = true;
	}

	if(PreviousMovementMode == MOVE_Custom && PreviousCustomMode == (uint8)ECustomMovementMode::CMOVE_OnLadder)
	{
		CurrentLadder = nullptr;
	}

	if(PreviousMovementMode == MOVE_Custom && PreviousCustomMode == (uint8)ECustomMovementMode::CMOVE_OnZipline)
	{
		CurrentZipline = nullptr;
	}

	if(MovementMode == MOVE_Custom)
	{
		Velocity = FVector::ZeroVector;
		switch (CustomMovementMode)
		{
			case (uint8)ECustomMovementMode::CMOVE_Mantling:
			{
				GetWorld()->GetTimerManager().SetTimer(MantlingTimer, this, &UGCBaseCharacterMovementComponent::EndMantle, CurrentMantlingParameters.Duration, false);
			}
			default:
				break;
		}
	}
	if(MovementMode == MOVE_Falling && PreviousCustomMode == (uint8)ECustomMovementMode::CMOVE_WallRun)
	{
		GetWorld()->GetTimerManager().ClearTimer(WallRunTimer);
	}
}

void UGCBaseCharacterMovementComponent::PhysCustom(float DeltaTime, int32 Iterations)
{
	if(BaseCharacterOwner->GetLocalRole() == ROLE_SimulatedProxy)
	{
		return;
	}
	
	switch (CustomMovementMode)
	{
	case (uint8)ECustomMovementMode::CMOVE_Mantling:
		{
			PhysMantle(DeltaTime, Iterations);
			break;
		}
	case (uint8)ECustomMovementMode::CMOVE_OnLadder:
		{
			PhysOnLadder(DeltaTime, Iterations);
			break;
		}
	case (uint8)ECustomMovementMode::CMOVE_OnZipline:
		{
			PhysOnZipline(DeltaTime, Iterations);
			break;
		}
	case (uint8)ECustomMovementMode::CMOVE_WallRun:
		{
			PhysWallRun(DeltaTime, Iterations);
			break;
		}
	case (uint8)ECustomMovementMode::CMOVE_Slide:
		{
			PhysSlide(DeltaTime, Iterations);
			break;
		}
		default:
			break;
	}
	Super::PhysCustom(DeltaTime, Iterations);
}

void UGCBaseCharacterMovementComponent::PhysMantle(float DeltaTime, int32 Iterations)
{
	float ElapsedTime = GetWorld()->GetTimerManager().GetTimerElapsed(MantlingTimer) + CurrentMantlingParameters.StartTime;

	FVector CurveValue = CurrentMantlingParameters.MantlingCurve->GetVectorValue(ElapsedTime);
	float PositionAlpha = CurveValue.X;
	float XYCorrectionAlpha = CurveValue.Y;
	float ZCorrectionAlpha = CurveValue.Z;

	FVector CorrectedInitialLocation = FMath::Lerp(CurrentMantlingParameters.InitialLocation, CurrentMantlingParameters.InitialAnimationLocation, XYCorrectionAlpha);
	CorrectedInitialLocation.Z = FMath::Lerp(CurrentMantlingParameters.InitialLocation.Z, CurrentMantlingParameters.InitialAnimationLocation.Z, ZCorrectionAlpha);
			
	FVector NewLocation = FMath::Lerp(CorrectedInitialLocation, CurrentMantlingParameters.TargetLocation, PositionAlpha);
	FRotator NewRotation = FMath::Lerp(CurrentMantlingParameters.InitialRotation, CurrentMantlingParameters.TargetRotation, PositionAlpha);
	FVector Delta = NewLocation - GetActorLocation();
			
	FVector TargetDelta = Delta + (CurrentMantlingParameters.PrimitiveComponent->GetComponentLocation() - CurrentMantlingParameters.PrimitiveComponentInitialLocation);

	Velocity = Delta / DeltaTime;
			
	FHitResult HitResult;
	SafeMoveUpdatedComponent(TargetDelta, NewRotation, false, HitResult);
}

void UGCBaseCharacterMovementComponent::PhysOnLadder(float DeltaTime, int32 Iterations)
{
	CalcVelocity(DeltaTime, 1.0f, false, ClimbingOnLadderBrakingDecelaration);
	FVector Delta = Velocity * DeltaTime;

	if(HasAnimRootMotion())
	{
		FHitResult Hit;
		SafeMoveUpdatedComponent(Delta, GetOwner()->GetActorRotation(), false, Hit);
		return;
	}
	
	FVector NewPos = GetActorLocation() + Delta;
	float NewPosProjection = GetActorToCurrentLadderProjection(NewPos);

	if(NewPosProjection < MinLadderBotomOffset)
	{
		DetachFromLadder(EDetachFromLadderMethod::ReachingTheBottom);
		return;
	}
	else if(NewPosProjection > (CurrentLadder->GetLadderHeight() - MaxLadderTopOffset))
	{
		DetachFromLadder(EDetachFromLadderMethod::ReachingTheTop);
		return;
	}

	FHitResult Hit;
	SafeMoveUpdatedComponent(Delta, GetOwner()->GetActorRotation(), true, Hit);
}

void UGCBaseCharacterMovementComponent::PhysOnZipline(float DeltaTime, int32 Iterations)
{
	Velocity = CurrentZipline->GetCabelDirection() * ZiplineSpeed;
	FVector Delta = Velocity * DeltaTime;

	FVector NewPos = GetActorLocation() + Delta;
	float NewPosProjection = GetActorToCurrentZiplineProjection(NewPos);

	if(NewPosProjection > ((CurrentZipline->GetWidth() * 0.5f) - ZiplineDeattachOffset) || NewPosProjection < -((CurrentZipline->GetWidth() * 0.5f)) + ZiplineDeattachOffset)
	{
	 	DetachFromZipline();
	 	return;
	}
	
	FHitResult Hit;
	SafeMoveUpdatedComponent(Delta, GetOwner()->GetActorRotation(), true, Hit);
}

void UGCBaseCharacterMovementComponent::PhysWallRun(float DeltaTime, int32 Iterations)
{
	if(CurrentHitWall.ImpactNormal.Z > GetWalkableFloorZ() || CurrentHitWall.ImpactNormal.Z < -0.005f)
	{
		StopWallRun();
	}
	
	if(!AreRequiersKeysDown(CurrentWallRunsSide))
	{
		StopWallRun();
	}
	
	UpdateWallRunning();

	Velocity = WallRunDirection * WallRunSpeed;
	CharacterOwner->SetActorRotation(WallRunDirection.ToOrientationRotator(), ETeleportType::TeleportPhysics);
	FVector Delta = Velocity * DeltaTime;
	FHitResult Hit;
	SafeMoveUpdatedComponent(Delta, GetOwner()->GetActorRotation(), true, Hit);
	
}

void UGCBaseCharacterMovementComponent::PhysSlide(float DeltaTime, int32 Iterations)
{
	Velocity = BaseCharacterOwner->GetActorForwardVector() * SlideSpeed;

	FVector StandingLocation = UpdatedComponent->GetComponentLocation();
	StandingLocation.Z -= 2.0f;
	float DefaultHalfHeight = CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	float DefaultCapsuleRadius = CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleRadius();
	const ECollisionChannel CollisionChannel = UpdatedComponent->GetCollisionObjectType();
	const FCollisionShape StandingCapsuleShape = FCollisionShape::MakeCapsule(DefaultCapsuleRadius, DefaultHalfHeight);
	FCollisionQueryParams CapsuleParams;
	CapsuleParams.AddIgnoredActor(CharacterOwner);
	FCollisionResponseParams ResponseParams;
	InitCollisionParams(CapsuleParams, ResponseParams);
	bool bIsEnough = false;
	bIsEnough = GetWorld()->OverlapBlockingTestByChannel(StandingLocation, FQuat::Identity, CollisionChannel, StandingCapsuleShape, CapsuleParams, ResponseParams);
	DrawDebugCapsule(GetWorld(), StandingLocation, DefaultHalfHeight, DefaultCapsuleRadius, FQuat::Identity, FColor::Red, false, 5.0f);
	if(!bIsEnough)
	{
		Velocity += GetGravityZ() * FVector::UpVector;
	}
	
	FVector Delta = Velocity * DeltaTime;
	Velocity = Delta / DeltaTime;
	FHitResult Hit;
	SafeMoveUpdatedComponent(Delta, GetOwner()->GetActorRotation(), true, Hit);
}

float UGCBaseCharacterMovementComponent::GetActorToCurrentLadderProjection(const FVector& Location) const
{
	checkf(IsValid(CurrentLadder), TEXT("UGCBaseCharacterMovementComponent::GetCharacterToCurrentLadderProjection() can not be invoked when current ladder is null"));
	FVector LadderUpVector = CurrentLadder->GetActorUpVector();
	FVector LadderToCharacterDistance = Location - CurrentLadder->GetActorLocation();
	return FVector::DotProduct(LadderUpVector, LadderToCharacterDistance);
}

float UGCBaseCharacterMovementComponent::GetActorToCurrentZiplineProjection(const FVector& Location) const
{
	checkf(IsValid(CurrentZipline), TEXT("UGCBaseCharacterMovementComponent::GetActorToCurrentZiplineProjection can not be invoked when current zipline is null"));
	FVector ZiplineForwardVector = CurrentZipline->GetCabel()->GetForwardVector();
	FVector ZiplineToCharacterDistance = Location - CurrentZipline->GetCabel()->GetComponentLocation();
	return FVector::DotProduct(ZiplineForwardVector, ZiplineToCharacterDistance);
}

bool UGCBaseCharacterMovementComponent::AreRequiersKeysDown(EWallRunSide Side)
{
	if(ForwardAxis == 0.0f)
	{
		return false;
	}
	return true;
}

void UGCBaseCharacterMovementComponent::UpdateWallRunning()
{
	FVector StartPosition = GetActorLocation();
	FVector LineTraceDirection = CurrentWallRunsSide == EWallRunSide::Right ? CharacterOwner->GetActorRightVector() : -CharacterOwner->GetActorRightVector(); 
	FVector EndPosition =  StartPosition + 100.0f * LineTraceDirection;
	FHitResult TraceHit;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(CharacterOwner);
	if(GetWorld()->LineTraceSingleByChannel(TraceHit, StartPosition, EndPosition, ECC_WallRunnable, QueryParams, FCollisionResponseParams::DefaultResponseParam))
	{
		if(TraceHit.Distance > 45.0f)
		{
			GetWallRunSide(TraceHit);
			GetWallRunDirection(TraceHit);
			CurrentHitWall = TraceHit;
		}
		else
		{
			GetWallRunDirection(CurrentHitWall);
		}
	}
	else
	{
		StopWallRun();
	}
}

void UGCBaseCharacterMovementComponent::GetWallRunSide(const FHitResult& Hit)
{
	if(FVector::DotProduct(Hit.ImpactNormal, CharacterOwner->GetActorRightVector()) > 0)
	{
		CurrentWallRunsSide = EWallRunSide::Left;
	}
	else
	{
		CurrentWallRunsSide = EWallRunSide::Right;
	}
}

void UGCBaseCharacterMovementComponent::GetWallRunDirection(const FHitResult& Hit)
{
	if(FVector::DotProduct(Hit.ImpactNormal, CharacterOwner->GetActorRightVector()) > 0)
	{
		WallRunDirection = FVector::CrossProduct(Hit.ImpactNormal, FVector::UpVector).GetSafeNormal();
	}
	else
	{
		WallRunDirection = FVector::CrossProduct(FVector::UpVector, Hit.ImpactNormal).GetSafeNormal();
	}
}

bool UGCBaseCharacterMovementComponent::IsNotEnoughSpaceToStandUp()
{
	FVector StandingLocation = UpdatedComponent->GetComponentLocation();
	ACharacter* DefaultCharacter = CharacterOwner->GetClass()->GetDefaultObject<ACharacter>();
	float DefaultHalfHeight = DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	float DefaultCapsuleRadius = DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleRadius();
	float Offset = 30.0f;
	FVector LocationWithOffset = StandingLocation + Offset * FVector::UpVector;
	const ECollisionChannel CollisionChannel = UpdatedComponent->GetCollisionObjectType();
	const FCollisionShape StandingCapsuleShape = FCollisionShape::MakeCapsule(DefaultCapsuleRadius, DefaultHalfHeight);
	FCollisionQueryParams CapsuleParams;
	CapsuleParams.AddIgnoredActor(CharacterOwner);
	FCollisionResponseParams ResponseParams;
	InitCollisionParams(CapsuleParams, ResponseParams);
	bool bIsEnough = false;
	DrawDebugCapsule(GetWorld(), LocationWithOffset, DefaultHalfHeight, DefaultCapsuleRadius, FQuat::Identity, FColor::Green, false, 5.0f);
	bIsEnough = GetWorld()->OverlapBlockingTestByChannel(LocationWithOffset + Offset * FVector::UpVector, FQuat::Identity, CollisionChannel, StandingCapsuleShape, CapsuleParams, ResponseParams);
	return bIsEnough;
}

bool UGCBaseCharacterMovementComponent::CanProneInCurrentState()
{
	return IsMovingOnGround() && UpdatedComponent && !UpdatedComponent->IsSimulatingPhysics();
}

void UGCBaseCharacterMovementComponent::OnPlayerCapsuleHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	CurrentHitWall = Hit;
	if(IsFalling() && !IsWallRuning() && Hit.Actor != CurrentWall)
	{
		CurrentWall = CurrentHitWall.Actor;
		GetWallRunSide(CurrentHitWall);
		FVector StartPosition = GetActorLocation();
		FVector LineTraceDirection = CurrentWallRunsSide == EWallRunSide::Right ? CharacterOwner->GetActorRightVector() : -CharacterOwner->GetActorRightVector(); 
		FVector EndPosition =  StartPosition + 100.0f * LineTraceDirection;
		FHitResult TraceHit;
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(CharacterOwner);

		if(GetWorld()->LineTraceSingleByChannel(TraceHit, StartPosition, EndPosition, ECC_WallRunnable, QueryParams, FCollisionResponseParams::DefaultResponseParam))
		{
			StartWallRun();
		}
	}
}

void FSavedMove_CGC::Clear()
{
	FSavedMove_Character::Clear();
	SavedIsSprinting = 0;
	SavedIsMantling = 0;
	SavedIsSliding = 0;
}

uint8 FSavedMove_CGC::GetCompressedFlags() const
{
	uint8 Result = FSavedMove_Character::GetCompressedFlags();

	if(SavedIsSprinting)
	{
		Result |= FLAG_IsSprinting;
	}
	if(SavedIsMantling)
	{
		Result &= ~FLAG_JumpPressed;
		Result |= FLAG_IsMantling;
	}
	if(SavedIsSliding)
	{
		//Result &= ~FLAG_WantsToCrouch;
		Result |= FLAG_IsSliding;
	}

	return Result;
}

bool FSavedMove_CGC::CanCombineWith(const FSavedMovePtr& NewMovePtr, ACharacter* InCharacter, float MaxDelta) const
{
	const FSavedMove_CGC* NewMove = StaticCast<const FSavedMove_CGC*>(NewMovePtr.Get());

	if(SavedIsSprinting != NewMove->SavedIsSprinting || SavedIsMantling != NewMove->SavedIsMantling || SavedIsSliding != NewMove->SavedIsSliding)
	{
		return false;
	}

	return FSavedMove_Character::CanCombineWith(NewMovePtr, InCharacter, MaxDelta);
}

void FSavedMove_CGC::SetMoveFor(ACharacter* Character, float InDeltaTime, FVector const& NewAccel,
	FNetworkPredictionData_Client_Character& ClientData)
{
	FSavedMove_Character::SetMoveFor(Character, InDeltaTime, NewAccel, ClientData);

	ACGCBaseCharacter* BaseCharacter = StaticCast<ACGCBaseCharacter*>(Character);
	UGCBaseCharacterMovementComponent* MovementComponent = BaseCharacter->GetBaseCharacterMovementComponent();

	SavedIsSprinting = MovementComponent->bIsSprinting;
	SavedIsMantling = BaseCharacter->bIsMantling;
	SavedIsSliding = BaseCharacter->bIsSliding;
}

void FSavedMove_CGC::PrepMoveFor(ACharacter* C)
{
	FSavedMove_Character::PrepMoveFor(C);

	UGCBaseCharacterMovementComponent* MovementComponent = StaticCast<UGCBaseCharacterMovementComponent*>(C->GetMovementComponent());

	MovementComponent->bIsSprinting = SavedIsSprinting;
}

FNetworkPredictionData_Client_CharacterCGC::FNetworkPredictionData_Client_CharacterCGC(
	const UCharacterMovementComponent& ClientMovementComponent)
		: Super(ClientMovementComponent)
{
	
}

FSavedMovePtr FNetworkPredictionData_Client_CharacterCGC::AllocateNewMove()
{
	return FSavedMovePtr(new FSavedMove_CGC());
}
