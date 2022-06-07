// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CGCBaseCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "PlayerCharacter.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class COURSEGAMECODE_API APlayerCharacter : public ACGCBaseCharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	
	virtual void MoveForward(float Value) override;
	virtual void MoveRight(float Value) override;
	virtual void TurnUp(float Value) override;
	virtual void TurnRight(float Value) override;

	virtual void SwimForward(float Value) override;
	virtual void SwimRight(float Value) override;
	virtual void SwimUp(float Value) override;

	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

	virtual bool CanJumpInternal_Implementation() const override;
	virtual void OnJumped_Implementation() override;

	virtual void OnSprintStart_Implementation() override;
	virtual void OnSprintEnd_Implementation() override;

	virtual void OnStartProne(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual void OnEndProne(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Character | Camera")
	UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Character | SpringArm")
	USpringArmComponent* SpringArm;

	FTimeline SpringArmLengthTimeline;

	FTimeline AimFOVTimeline;

	void SprintArmLengthTimelineUpdate(const float Alpha);

	void AimFOVTimelineUpdate(const float Alpha);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Timeline settings")
	UCurveFloat* SprintArmLengthCurveFloat;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Timeline settings")
	UCurveFloat* AimFOVCurveFloat;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Spring arm settings")
	float DefaultSpringArmLength;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Spring arm settings")
	float MaxSpringArmLength = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Spring arm settings")
	float SpringArmProneTargetOffset = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Spring arm settings")
	float SpringArmCrouchTargetOffset = 5.0f;

	virtual void OnStartAimingInternal() override;

	virtual void OnStopAimingInternal() override;
	
};
