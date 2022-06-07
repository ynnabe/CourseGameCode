// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CourseGameCode/Characters/CGCBaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GCBaseCharacterMovementComponent.generated.h"

struct FMantlingMovementParameters
{
	FVector InitialLocation = FVector::ZeroVector;
	FRotator InitialRotation = FRotator::ZeroRotator;

	FVector TargetLocation = FVector::ZeroVector;
	FRotator TargetRotation = FRotator::ZeroRotator;
	FVector InitialAnimationLocation = FVector::ZeroVector;

	float Duration = 1.0f;
	float StartTime = 0.0f;

	UCurveVector* MantlingCurve;

	UPrimitiveComponent* PrimitiveComponent;
	FVector PrimitiveComponentInitialLocation;
};

UENUM(BlueprintType)
enum class ECustomMovementMode : uint8
{
	CMOVE_None = 0 UMETA(DisplayName = "None"),
	CMOVE_Mantling UMETA(DisplayName = "Mantling"),
	CMOVE_OnLadder UMETA(DisplayName = "OnLadder"),
	CMOVE_OnZipline UMETA(DisplayName = "OnZipline"),
	CMOVE_WallRun UMETA(DisplayName = "WallRun"),
	CMOVE_Slide UMETA(DisplayName = "Slide"),
	CMOVE_Max UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EWallRunSide : uint8
{
	None,
	Left,
	Right
};

UENUM(BlueprintType)
enum class EDetachFromLadderMethod : uint8
{
	Fall = 0,
	ReachingTheTop,
	ReachingTheBottom,
	JumpOff
};

/**
 * 
 */
UCLASS()
class COURSEGAMECODE_API UGCBaseCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

	friend class FSavedMove_CGC;

public:

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;

	virtual void UpdateFromCompressedFlags(uint8 Flags) override;
	
	bool GetIsSprinting() {return bIsSprinting;}
	bool IsOutOfStamina() const {return bIsOutOfStamina;}
	void SetIsOutOfStamina(bool bIsOutOfStamina_in);

	virtual float GetMaxSpeed() const override;
	virtual void BeginPlay() override;

	void StartSprint();
	void StopSprint();

	void StartMantle(const FMantlingMovementParameters& MantlingParameters);
	void EndMantle();
	bool IsMantling() const;

	void StartWallRun();
	void StopWallRun();
	bool IsWallRuning() const;

	void AttachToLadder(const class ALadder* Ladder);
	void DetachFromLadder(EDetachFromLadderMethod DetachFromLadderMethod = EDetachFromLadderMethod::Fall);
	bool IsOnLadder() const;
	const class ALadder* GetCurrentLadder();
	virtual void PhysicsRotation(float DeltaTime) override;
	float GetLadderSpeedRatio() const;
	
	void AttachToZipline(const class AZipline* Zipline);
	void DetachFromZipline();
	bool IsOnZipline() const;
	const class AZipline* GetCurrentZipline();

	float ForwardAxis = 0.0f;
	float RightAxis = 0.0f;

	EWallRunSide GetSide();
	FVector GetWallRunDirection();
	float GetJumpOffFromWall();
	void StartSlide();
	void StopSlide();
	bool IsSliding();
	
	void Prone();
	void UnProne();
	bool IsProning() const {return BaseCharacterOwner && BaseCharacterOwner->bIsProned;}
	bool bIsWantsToProne = false;
	bool bProneMaintainsBaseLocation = false;
	virtual void UpdateCharacterStateBeforeMovement(float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character movement: prone", meta =(ClampMin = 0.0f, UIMin = 0.0f))
	float ProneCapsuleRadius = 40.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character movement: prone", meta=(ClampMin = 0.0f, UIMin = 0.0f))
	float ProneCapsuleHalfHeight = 40.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="CharacterMovement: slide", meta=(ClampMin = 0.0f, UIMin = 0.0f))
	float SlideCapsuleHalfHeight = 40.0f;
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Character | Prone")
	bool bIsFullHeight = false;
	
protected:

	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;

	virtual void PhysCustom(float deltaTime, int32 Iterations) override;

	void PhysMantle(float DeltaTime, int32 Iterations);

	void PhysOnLadder(float DeltaTime, int32 Iterations);

	void PhysOnZipline(float DeltaTime, int32 Iterations);

	void PhysWallRun(float DeltaTime, int32 Iterations);

	void PhysSlide(float DeltaTime, int32 Iterations);

	float GetActorToCurrentLadderProjection(const FVector& Location) const;

	float GetActorToCurrentZiplineProjection(const FVector& Location) const;

	bool AreRequiersKeysDown(EWallRunSide Side);
	bool IsNotEnoughSpaceToStandUp();

	void UpdateWallRunning();
	
	void GetWallRunSide(const FHitResult& Hit);
	void GetWallRunDirection(const FHitResult& Hit);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character movement: Swimming")
	float SwimmingCapsuleRadius = 60.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character movement: Swimming")
	float SwimmingCapsuleHalfHeight = 60.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character movement: sprint", meta=(ClampMin = 0.0f, UIMin = 0.0f))
	float SprintSpeed = 1200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character movement: stamina", meta=(ClampMin = 0.0f, UIMin = 0.0f))
	float OutOfStaminaSpeed = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character movement: prone", meta=(ClampMin = 0.0f, UIMin = 0.0f))
	float ProneSpeed = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character movement: ladder", meta=(ClampMin = 0.0f, UIMin = 0.0f));
	float ClimbingOnLadderSpeed = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character movement: ladder", meta=(ClampMin = 0.0f, UIMin = 0.0f));
	float ClimbingOnLadderBrakingDecelaration = 2048.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character movement: ladder", meta=(ClampMin = 0.0f, UIMin = 0.0f));
	float LadderToCharacterOffset = 60.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character movement: ladder", meta=(ClampMin = 0.0f, UIMin = 0.0f));
	float MaxLadderTopOffset = 90.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character movement: ladder", meta=(ClampMin = 0.0f, UIMin = 0.0f));
	float MinLadderBotomOffset = 90.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character movement: ladder", meta=(ClampMin = 0.0f, UIMin = 0.0f));
	float JumpOffFromLadderSpeed = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="CharacterMovement: zipline", meta=(ClampMin = 0.0f, UIMin = 0.0f))
	float ZiplineToCharacterOffset = 110.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="CharacterMovement: zipline", meta=(ClampMin = 0.0f, UIMin = 0.0f))
	float ZiplineSpeed = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="CharacterMovement: zipline", meta=(ClampMin = 0.0f, UIMin = 0.0f))
	float ZiplineDeattachOffset = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="CharacterMovement: WallRun", meta=(ClampMin = 0.0f, UIMin = 0.0f))
	float WallRunSpeed = 300.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="CharacterMovement: slide", meta=(ClampMin = 0.0f, UIMin = 0.0f))
	float SlideSpeed = 800.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="CharacterMovement: WallRun", meta=(ClampMin = 0.0f, UIMin = 0.0f))
	float WallRunTime = 5.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="CharacterMovement: slide", meta=(ClampMin = 0.0f, UIMin = 0.0f))
	float SlideTime = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="CharacterMovement: WallRun", meta=(ClampMin = 0.0f, UIMin = 0.0f))
	float JumpOffFromWall = 300.0f;
	
	EWallRunSide CurrentWallRunsSide = EWallRunSide::None;
	
	FVector WallRunDirection = FVector::ZeroVector;
	
	FHitResult CurrentHitWall;

	TSoftObjectPtr<ACGCBaseCharacter> BaseCharacterOwner;

private:
	bool CanProneInCurrentState();
	
	UFUNCTION()
	void OnPlayerCapsuleHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
	bool bIsSprinting = false;
	bool bIsOutOfStamina = false;
	
	bool bIsSliding = false;

	FMantlingMovementParameters CurrentMantlingParameters;
	FTimerHandle MantlingTimer;
	FTimerHandle SlidingTimer;

	const ALadder* CurrentLadder = nullptr;
	const AZipline* CurrentZipline = nullptr;
	TWeakObjectPtr<AActor> CurrentWall = nullptr;

	FTimerHandle WallRunTimer;

	FRotator ForceTargetRotation = FRotator::ZeroRotator;
	bool bForceRotation = false;
};

class FSavedMove_CGC : public FSavedMove_Character
{
	typedef FSavedMove_Character Super;

public:
	virtual void Clear() override;

	virtual uint8 GetCompressedFlags() const override;

	virtual bool CanCombineWith(const FSavedMovePtr& NewMovePtr, ACharacter* InCharacter, float MaxDelta) const override;

	virtual void SetMoveFor(ACharacter* Character, float InDeltaTime, FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData) override;

	virtual void PrepMoveFor(ACharacter* C) override;

private:
	uint8 SavedIsSprinting : 1;
	uint8 SavedIsMantling : 1;
	uint8 SavedIsSliding : 1;
	
};

class FNetworkPredictionData_Client_CharacterCGC : public FNetworkPredictionData_Client_Character
{
	typedef FNetworkPredictionData_Client_Character Super;

public:
	FNetworkPredictionData_Client_CharacterCGC(const UCharacterMovementComponent& ClientMovementComponent);

	virtual FSavedMovePtr AllocateNewMove() override;
};
