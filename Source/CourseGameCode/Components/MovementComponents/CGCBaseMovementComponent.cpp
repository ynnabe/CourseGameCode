// Fill out your copyright notice in the Description page of Project Settings.


#include "CGCBaseMovementComponent.h"

void UCGCBaseMovementComponent::TickComponent(float DeltaTime, ELevelTick Tick,
	FActorComponentTickFunction* ThisTickFunction)
{
	if(ShouldSkipUpdate(DeltaTime))
	{
		return;
	}

	Super::TickComponent(DeltaTime, Tick, ThisTickFunction);

	const FVector PendingInput = GetPendingInputVector().GetClampedToMaxSize(1.0f);
	Velocity = PendingInput * MaxSpeed;
	ConsumeInputVector();

	if(bEnableGravity)
	{
		FHitResult HitResult;
		FVector StartPoint = UpdatedComponent->GetComponentLocation();
		float TraceDepth = 1.0f;
		float SphereRadius = 50.0f;
		FVector EndPoint = StartPoint - TraceDepth * FVector::UpVector;
		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(GetOwner());

		bool bWasFalling = bFalling;
		FCollisionShape Sphere = FCollisionShape::MakeSphere(SphereRadius);
		bFalling = !GetWorld()->SweepSingleByChannel(HitResult, StartPoint, EndPoint, FQuat::Identity, ECC_Visibility, Sphere, CollisionParams);
		if(bFalling)
		{
			VerticalVelocity += GetGravityZ() * FVector::UpVector * DeltaTime;
		}
		else if(bWasFalling && VerticalVelocity.Z < 0.0f)
		{
			VerticalVelocity = FVector::ZeroVector;
		}
	}

	Velocity += VerticalVelocity;
	const FVector Delta = Velocity * DeltaTime;
	if(!Delta.IsNearlyZero(1e-6f))
	{
		const FQuat Rot = UpdatedComponent->GetComponentQuat();
		FHitResult Hit(1.f);
		SafeMoveUpdatedComponent(Delta, Rot, true, Hit);

		if(Hit.IsValidBlockingHit())
		{
			HandleImpact(Hit, DeltaTime, Delta);
			SlideAlongSurface(Delta, 1.f - Hit.Time, Hit.Normal, Hit, true);
		}
	}
	UpdateComponentVelocity();
}

void UCGCBaseMovementComponent::JumpStart()
{
	VerticalVelocity = InitialJumpVelocity * FVector::UpVector;
}

bool UCGCBaseMovementComponent::IsFalling() const
{
	return bFalling;
}
