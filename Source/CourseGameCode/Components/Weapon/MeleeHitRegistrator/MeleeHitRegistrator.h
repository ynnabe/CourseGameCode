// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "MeleeHitRegistrator.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMeleeHitRegistred, const FHitResult&, HitResult, const FVector&, HitDirection);

UCLASS(meta = (BlueprintSpawnableComponent))
class COURSEGAMECODE_API UMeleeHitRegistrator : public USphereComponent
{
	GENERATED_BODY()

public:
	UMeleeHitRegistrator();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void ProcessHitRegistration();

	void SetIsHitRegistrationEnable(bool bIsEnable);

	FOnMeleeHitRegistred OnMeleeHitRegistredDelegate;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Hit registration")
	bool bIsHitRegistrationEnabled = false;

private:
	FVector PreviuosComponentLocation = FVector::ZeroVector;
};
