// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CGCBasePawnAnimInstance.h"
#include "SpiderbotPawnAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class COURSEGAMECODE_API USpiderbotPawnAnimInstance : public UCGCBasePawnAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeBeginPlay() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	UPROPERTY(VisibleAnywhere, Transient, BlueprintReadOnly, Category="Spider Bot | IK Settings")
	FVector RightFrontEffectorLocation = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere, Transient, BlueprintReadOnly, Category="Spider Bot | IK Settings")
	FVector RightRearEffectorLocation = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere, Transient, BlueprintReadOnly, Category="Spider Bot | IK Settings")
	FVector LeftFrontEffectorLocation = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere, Transient, BlueprintReadOnly, Category="Spider Bot | IK Settings")
	FVector LeftRearEffectorLocation = FVector::ZeroVector;


private:
	TWeakObjectPtr<class ASpiderbotPawn> CachedSpiderbotPawnOwner;
};
