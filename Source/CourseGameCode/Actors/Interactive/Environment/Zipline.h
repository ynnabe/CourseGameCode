// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/CapsuleComponent.h"
#include "CourseGameCode/Actors/Interactive/InteractiveActor.h"
#include "Zipline.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class COURSEGAMECODE_API AZipline : public AInteractiveActor
{
	GENERATED_BODY()

public:

	AZipline();
	virtual void OnConstruction(const FTransform& Transform) override;

	UStaticMeshComponent* GetCabel() const;

	FVector GetCabelDirection() const;

	float GetWidth() const;
protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Zipline parameters")
	UStaticMeshComponent* FirstPillar;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Zipline parameters")
	UStaticMeshComponent* SecondPillar;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Zipline parameters")
	UStaticMeshComponent* Cabel;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Zipline parameters")
	float FirstPillarHeight = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Zipline parameters")
	float SecondPillarHeight = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Zipline parameters")
	float FirstPillarZPosition = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Zipline parameters")
	float SecondPillarZPosition = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Zipline parameters")
	float PillarWidth = 300.0f;

	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category="Zipline parameters")
	float InteractiveVolumeCapsuleRadius = 50.0f;
	
	float CabelLength = 0.0f;

	FVector CabelDirection = FVector::ZeroVector;
	
	FVector FirstPillarTop = FVector::ZeroVector;
	
	FVector SecondPillarTop = FVector::ZeroVector;

	UCapsuleComponent* GetCapsuleInteractionVolume();
	
};
