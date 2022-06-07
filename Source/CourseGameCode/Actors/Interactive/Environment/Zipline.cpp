// Fill out your copyright notice in the Description page of Project Settings.


#include "Zipline.h"

#include "Components/CapsuleComponent.h"
#include "CourseGameCode/CGCCollisionTypes.h"

AZipline::AZipline()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	
	FirstPillar = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FirstPillar"));
	FirstPillar->SetupAttachment(RootComponent);

	SecondPillar = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SecondPillar"));
	SecondPillar->SetupAttachment(RootComponent);

	Cabel = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cabel"));
	Cabel->SetupAttachment(RootComponent);

	InteractionVolume = CreateDefaultSubobject<UCapsuleComponent>(TEXT("InteractionVolume"));
	InteractionVolume->SetupAttachment(RootComponent);
	InteractionVolume->SetCollisionProfileName(CollisionProfilePawnInteractionVolume);
	InteractionVolume->SetGenerateOverlapEvents(true);
}

void AZipline::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	FirstPillar->SetRelativeLocation(FVector(-PillarWidth * 0.5f, 0.0f, FirstPillarZPosition + (FirstPillarHeight * 0.5f)));
	SecondPillar->SetRelativeLocation(FVector(PillarWidth * 0.5f, 0.0f, SecondPillarZPosition + (SecondPillarHeight * 0.5f)));
	
	FirstPillarTop = FVector(FirstPillar->GetRelativeLocation() + (FirstPillarHeight * 0.5f * FVector::UpVector));
	SecondPillarTop = FVector(SecondPillar->GetRelativeLocation() + (SecondPillarHeight * 0.5f * FVector::UpVector));

	float InitialCabelLength = Cabel->GetStaticMesh()->GetBoundingBox().GetExtent().X * 2.0f;
	float DeltaLength = (SecondPillarTop - FirstPillarTop).Size();
	CabelLength = DeltaLength / InitialCabelLength;
	
	CabelDirection = FirstPillarTop - SecondPillarTop;
	CabelDirection.Normalize();
	
	FRotator CabelRotator = FRotator(CabelDirection.ToOrientationRotator());
	FVector CabelLocation = (FirstPillarTop + SecondPillarTop) * 0.5f;
	
	Cabel->SetRelativeLocation(CabelLocation);
	Cabel->SetRelativeRotation(CabelRotator);
	//CabelDirection = Cabel->GetForwardVector();

	GetCapsuleInteractionVolume()->SetCapsuleHalfHeight(DeltaLength * 0.5f);
	GetCapsuleInteractionVolume()->SetCapsuleRadius(InteractiveVolumeCapsuleRadius);
	GetCapsuleInteractionVolume()->SetRelativeLocation(CabelLocation);
	FRotator InteractiveVolumeRotator = FRotator(CabelRotator.Pitch - 90.0f, 180.0f, 0.0f);
	GetCapsuleInteractionVolume()->SetRelativeRotation(InteractiveVolumeRotator);
	
	
	UStaticMesh* FirstPillarMesh = FirstPillar->GetStaticMesh();
	if(IsValid(FirstPillarMesh))
	{
		float MeshHeight = FirstPillarMesh->GetBoundingBox().GetSize().Z;
		if(!FMath::IsNearlyZero(MeshHeight))
		{
			FirstPillar->SetRelativeScale3D(FVector(1.0f, 1.0f, FirstPillarHeight / MeshHeight));
		}
	}

	UStaticMesh* SecondPillarMesh = SecondPillar->GetStaticMesh();
	if(IsValid(SecondPillarMesh))
	{
		float MeshHeight = SecondPillarMesh->GetBoundingBox().GetSize().Z;
		if(!FMath::IsNearlyZero(MeshHeight))
		{
			SecondPillar->SetRelativeScale3D(FVector(1.0f, 1.0f, SecondPillarHeight / MeshHeight));
		}
	}

	 UStaticMesh* CabelMesh = Cabel->GetStaticMesh();
	 if(IsValid(CabelMesh))
	 {
	  	float MeshWidth = CabelMesh->GetBoundingBox().GetSize().X;
	  	if(!FMath::IsNearlyZero(MeshWidth))
	  	{
	  		Cabel->SetRelativeScale3D(FVector(CabelLength, 1.0f, 1.0f));
	  	}
	 }
}

UStaticMeshComponent* AZipline::GetCabel() const
{
	return Cabel;
}

FVector AZipline::GetCabelDirection() const
{
	FVector Direction = Cabel->GetForwardVector();
	FRotator DirectionRotator = Direction.ToOrientationRotator();
	if(DirectionRotator.Pitch > 0.0f)
	{
		return -Direction;
	}
	else if(DirectionRotator.Pitch < 0.0f)
	{
		return Direction;
	}
	else
	{
		return FVector::ZeroVector;
	}
}

float AZipline::GetWidth() const
{
	return PillarWidth;
}

UCapsuleComponent* AZipline::GetCapsuleInteractionVolume()
{
	return StaticCast<UCapsuleComponent*>(InteractionVolume);
}
