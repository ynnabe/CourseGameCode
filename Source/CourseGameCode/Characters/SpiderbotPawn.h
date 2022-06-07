// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CourseGameCodePawn.h"
#include "SpiderbotPawn.generated.h"

/**
 * 
 */
UCLASS()
class COURSEGAMECODE_API ASpiderbotPawn : public ACourseGameCodePawn
{
	GENERATED_BODY()

public:
	ASpiderbotPawn();

	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE float GetRightFrontFootOffset() const { return RightFrontFootOffset;}

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE float GetRightRearFootOffset() const { return RightRearFootOffset;}

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE float GetLeftFrontFootOffset() const {return LeftFrontFootOffset;}

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE float GetLeftRearFootOffset() const {return LeftRearFootOffset;}

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spider Bot")
	USkeletalMeshComponent* SkeletalMeshComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= "Spider Bot | IK")
	FName RightFrontFootSocket;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spider Bot | IK")
	FName RightRearFootSocket;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= "Spider Bot | IK")
	FName LeftFrontFootSocket;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= "Spider Bot | IK")
	FName LeftRearFootSocket;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "Spider Bot | IK", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float IKTraceExtendedDistance = 60.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "Spider Bot | IK", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float IKInterpSpeed = 20.0f;

private:
	float GetIKOffsetForASocket(const FName& SocketName);
	
	float RightFrontFootOffset = 0.0f;
	float RightRearFootOffset = 0.0f;
	float LeftFrontFootOffset = 0.0f;
	float LeftRearFootOffset = 0.0f;

	float IKTraceDistance = 0.0f;
	float IKScale = 0.0f;
};
