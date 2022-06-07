// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_EndHardLanding.generated.h"

/**
 * 
 */
UCLASS()
class COURSEGAMECODE_API UAnimNotifyState_EndHardLanding : public UAnimNotifyState
{
	GENERATED_BODY()

	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	
};
