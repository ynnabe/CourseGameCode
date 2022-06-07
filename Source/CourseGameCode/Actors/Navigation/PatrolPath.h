// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PatrolPath.generated.h"

UCLASS()
class COURSEGAMECODE_API APatrolPath : public AActor
{
	GENERATED_BODY()
	
public:	

	const TArray<FVector>& GetWayPoints() const;

protected:
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="Path", meta=(MakeEditWidget))
	TArray<FVector> WayPoints;

};
