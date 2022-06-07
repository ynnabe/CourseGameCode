// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CGCBaseCharacterAnimInstance.h"
#include "FPPCharacterAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class COURSEGAMECODE_API UFPPCharacterAnimInstance : public UCGCBaseCharacterAnimInstance
{
	GENERATED_BODY()

public:

	virtual void NativeBeginPlay() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "Character | Animations | First Person")
	float PlayerCameraPitchAngle = 0.0f;

	TWeakObjectPtr<class AFPPlayerCharacter> CachedFPPlayerCharacterOwner;

private:

	float CalculateCameraPitchAngle() const;
};
