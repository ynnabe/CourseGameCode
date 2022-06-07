// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerCharacter.h"
#include "Controllers/CGCPlayerController.h"
#include "FPPlayerCharacter.generated.h"

/**
 * 
 */
UCLASS()
class COURSEGAMECODE_API AFPPlayerCharacter : public APlayerCharacter
{
	GENERATED_BODY()

	AFPPlayerCharacter(const FObjectInitializer& ObjectInitializer);

	virtual void PossessedBy(AController* NewController) override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

	virtual FRotator GetViewRotation() const override;

	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) override;


protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | First Person")
	class USkeletalMeshComponent* FirstPersonMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | First Person")
	class UCameraComponent* FirstPersonCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | First Person | Camera")
	float LadderCameraMinPitch = 60.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | First Person | Camera")
	float LadderCameraMaxPitch = 60.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | First Person | Camera")
	float LadderCameraMinYaw = 60.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | First Person | Camera")
	float LadderCameraMaxYaw = 60.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Character | First Person | Camera")
	float ZiplineCameraMinPitch = 60.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Character | First Person | Camera")
	float ZiplineCameraMaxPitch = 60.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Character | First Person | Camera")
	float ZiplineCameraMinYaw = 60.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Character | First Person | Camera")
	float ZiplineCameraMaxYaw = 60.0f;

	virtual void OnMantle(const FMantlingSettings& MantlingSettings, float MantlingAnimationStartTime) override;

	bool IsFPMontagePlaying() const;

	virtual void OnStartAimingInternal() override;

	virtual void OnStopAimingInternal() override;

private:
	FTimerHandle FPMontageTimerHandle;

	void OnFPMontageTimerElapsed();

	TWeakObjectPtr<class ACGCPlayerController> PlayerController;
};
