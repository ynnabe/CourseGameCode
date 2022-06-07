// Fill out your copyright notice in the Description page of Project Settings.


#include "CGCAIController.h"

#include "Perception/AIPerceptionComponent.h"

ACGCAIController::ACGCAIController()
{
	PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));
}

AActor* ACGCAIController::FindClosestSensedActor(TSubclassOf<UAISense> SenseClass)
{
	if(!IsValid(GetPawn()))
	{
		return nullptr;
	}
	
	TArray<AActor*> SenseActors;
	PerceptionComponent->GetCurrentlyPerceivedActors(SenseClass, SenseActors);

	AActor* ClosestActor = nullptr;
	float MinSquaredDistance = FLT_MAX;
	FVector TurretLocation = GetPawn()->GetActorLocation();

	for(AActor* SeenActor : SenseActors)
	{
		float CurrentSquaredDistance = (TurretLocation - SeenActor->GetActorLocation()).SizeSquared();
		if(CurrentSquaredDistance < MinSquaredDistance)
		{
			MinSquaredDistance = CurrentSquaredDistance;
			ClosestActor = SeenActor;
		}
	}

	return ClosestActor;
}
