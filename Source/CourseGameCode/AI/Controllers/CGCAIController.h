// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "CGCAIController.generated.h"

class UAISense;
UCLASS()
class COURSEGAMECODE_API ACGCAIController : public AAIController
{
	GENERATED_BODY()

public:
	ACGCAIController();


protected:

	AActor* FindClosestSensedActor(TSubclassOf<UAISense> SenseClass);
};
