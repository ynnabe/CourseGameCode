// Fill out your copyright notice in the Description page of Project Settings.


#include "SpiderbotPawn.h"

#include "Kismet/KismetSystemLibrary.h"

ASpiderbotPawn::ASpiderbotPawn()
{
	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Skeletal Mesh Component"));
	SkeletalMeshComponent->SetupAttachment(RootComponent);

	IKScale = GetActorScale().Z;
	IKTraceDistance = CollisionSphereRadius * IKScale;
}

void ASpiderbotPawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	RightFrontFootOffset = FMath::FInterpTo(RightFrontFootOffset, GetIKOffsetForASocket(RightFrontFootSocket), DeltaSeconds, IKInterpSpeed);
	RightRearFootOffset = FMath::FInterpTo(RightRearFootOffset, GetIKOffsetForASocket(RightRearFootSocket), DeltaSeconds, IKInterpSpeed);
	LeftFrontFootOffset = FMath::FInterpTo(LeftFrontFootOffset, GetIKOffsetForASocket(LeftFrontFootSocket), DeltaSeconds, IKInterpSpeed);
	LeftRearFootOffset = FMath::FInterpTo(LeftRearFootOffset, GetIKOffsetForASocket(LeftRearFootSocket), DeltaSeconds, IKInterpSpeed);
}

float ASpiderbotPawn::GetIKOffsetForASocket(const FName& SocketName)
{
	float Result = 0.0f;
	
	FVector SocketLocation = SkeletalMeshComponent->GetSocketLocation(SocketName);
	FVector TraceStart(SocketLocation.X, SocketLocation.Y, GetActorLocation().Z);
	FVector TraceEnd = TraceStart - IKTraceDistance * FVector::UpVector;

	FHitResult HitResult;
	ETraceTypeQuery TraceType = UEngineTypes::ConvertToTraceType(ECC_Visibility);
	if(UKismetSystemLibrary::LineTraceSingle(GetWorld(), TraceStart, TraceEnd, TraceType, true, TArray<AActor*>(), EDrawDebugTrace::ForOneFrame, HitResult, true))
	{
		Result = (TraceEnd.Z - HitResult.Location.Z) / IKScale;
	}
	else if(UKismetSystemLibrary::LineTraceSingle(GetWorld(), TraceEnd, TraceEnd - IKTraceExtendedDistance * FVector::UpVector, TraceType, true, TArray<AActor*>(), EDrawDebugTrace::ForOneFrame, HitResult, true))
	{
		Result = (TraceEnd.Z - HitResult.Location.Z) / IKScale;
	}
	return Result;
}
