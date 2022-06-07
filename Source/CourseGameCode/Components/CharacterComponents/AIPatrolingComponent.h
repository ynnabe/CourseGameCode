// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AIPatrolingComponent.generated.h"

class APatrolPath;

UENUM(BlueprintType)
enum class EPatrolType : uint8
{
	Circle,
	PingPong
};


USTRUCT(BlueprintType)
struct FPathInfo
{
	GENERATED_BODY()

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="Path")
	EPatrolType PatrolType = EPatrolType::Circle;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category="Path")
	APatrolPath* PatrolPath;

	bool bIsGoingBack = false;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class COURSEGAMECODE_API UAIPatrolingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	FVector SelectClosestWayPoint();
	FVector SelectNextWayPoint();
	bool CanPatrol() const;

protected:

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="PathInfo")
	FPathInfo PatrolInfo;

private:
	int32 CurrentWayPointIndex = -1;
};
