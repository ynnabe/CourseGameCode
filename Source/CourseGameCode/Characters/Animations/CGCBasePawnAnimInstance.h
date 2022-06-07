// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CGCBasePawnAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class COURSEGAMECODE_API UCGCBasePawnAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:

	virtual void NativeBeginPlay() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
protected:

	UPROPERTY(BlueprintReadOnly, Transient, Category = "Base Pawn animation initial")
	float InputForward;

	UPROPERTY(BlueprintReadOnly, Transient, Category = "Base Pawn animation initial")
	float InputRight;

	UPROPERTY(BlueprintReadOnly, Transient, Category = "Base Pawn animation initial")
	bool bIsInAir;

private:
	TWeakObjectPtr<class ACourseGameCodePawn> CachedBasePawn;
};
