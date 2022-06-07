// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_SetHitRegistration.generated.h"

/**
 * 
 */
UCLASS()
class COURSEGAMECODE_API UAnimNotify_SetHitRegistration : public UAnimNotify
{
	GENERATED_BODY()

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Hit registration")
	bool bIsHitRegistrationEnabled = false;
	
};
