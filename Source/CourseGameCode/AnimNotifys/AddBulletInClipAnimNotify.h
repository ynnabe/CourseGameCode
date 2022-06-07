// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AddBulletInClipAnimNotify.generated.h"

/**
 * 
 */
UCLASS()
class COURSEGAMECODE_API UAddBulletInClipAnimNotify : public UAnimNotify
{
	GENERATED_BODY()

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ammo", meta=(ClampMin = 1, UIMin = 1))
	int32 BulletsAmount = 1;
};
