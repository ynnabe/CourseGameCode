// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CGCCollisionTypes.h"
#include "Animation/AnimInstance.h"
#include "CourseGameCode/Components/MovementComponents/GCBaseCharacterMovementComponent.h"
#include "CGCBaseCharacterAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class COURSEGAMECODE_API UCGCBaseCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:

	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UAnimMontage* SlideMontage;
	

protected:
	UFUNCTION()
	void ChangeSlideState(bool State);

	UFUNCTION()
	void PlayHardLanding();

	UFUNCTION()
	void PlayDeathForwardMontage();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animations")
	float Speed = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "Character animations")
	bool bIsFalling = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "Character animations")
	bool bIsCrouching = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "Character animations")
	bool bIsSprinting = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character animations")
	bool bIsOutOfStamina = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character animations")
	bool bIsProning = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character animations")
	bool bIsSwimming = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character animations")
	bool bIsOnLadder = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character animations")
	bool bIsOnZipline = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character animations")
	bool bIsWallRunning = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character animations")
	EWallRunSide CurrentWallRunSide = EWallRunSide::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character animations")
	float LadderSpeedRatio = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character animations")
	bool bIsStrafing = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character animations", meta=(UIMin = -180.0f, UIMax = 180))
	float Direction = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character animations")
	FRotator AimRotation = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, Transient, BlueprintReadOnly, Category= "Character | IK System")
	FVector LeftFootEffectorLocation = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, Transient, BlueprintReadOnly, Category="Character | IK System")
	FVector RightFootEffectorLocation = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, Transient, BlueprintReadOnly, Category="Character | IK System")
	FVector PelvisOffset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, Transient, BlueprintReadOnly, Category="Character animations")
	float PronePelvisOffset = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UAnimMontage* HardLandingMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UAnimMontage* OnDeathForward;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon")
	EEquipableItemType CurrentEquippedItem;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Sockets")
	FTransform ForeGripSocketTransform;

	UPROPERTY(EditAnywhere, Transient, BlueprintReadOnly, Category="Character animations")
	bool bIsAiming = false;

private:
	TWeakObjectPtr<class ACGCBaseCharacter> CachedBaseCharacter;
	TWeakObjectPtr<class UCharacterAttributeComponent> CachedCharacterAttributesComponent;
};
