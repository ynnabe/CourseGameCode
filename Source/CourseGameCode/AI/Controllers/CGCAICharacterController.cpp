// Fill out your copyright notice in the Description page of Project Settings.


#include "CGCAICharacterController.h"
#include "AI/Characters/CGCAICharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/CharacterComponents/AIPatrolingComponent.h"
#include "Perception/AISense_Sight.h"

void ACGCAICharacterController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);
	if(IsValid(InPawn))
	{
		checkf(InPawn->IsA<ACGCAICharacter>(), TEXT("ACGCAICharacterController::SetPawn can posses only CGCAICharacter"));
		CachedAICharacter = StaticCast<ACGCAICharacter*>(InPawn);
		RunBehaviorTree(CachedAICharacter->GetBehaviorTree());
		SetupPatrolling();
	}
	else
	{
		CachedAICharacter = nullptr;
	}
}

void ACGCAICharacterController::ActorsPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
	Super::ActorsPerceptionUpdated(UpdatedActors);
	if(!CachedAICharacter.IsValid())
	{
		return;
	}

	TryMoveToNextTarget();
}

void ACGCAICharacterController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);
	if(!Result.IsSuccess())
	{
		return;
	}
	TryMoveToNextTarget();
}

void ACGCAICharacterController::SetupPatrolling()
{
	UAIPatrolingComponent* PatrolingComponent = CachedAICharacter->GetPatrolingComponent();
	if(PatrolingComponent->CanPatrol())
	{
		FVector ClosestWayPoint = PatrolingComponent->SelectClosestWayPoint();
		if(IsValid(Blackboard))
		{
			Blackboard->SetValueAsVector(BB_NextLocation, ClosestWayPoint);
			Blackboard->SetValueAsObject(BB_CurrentTarget, nullptr);
		}
		bIsPatrol = true;
	}
}

void ACGCAICharacterController::TryMoveToNextTarget()
{
	UAIPatrolingComponent* PatrolingComponent = CachedAICharacter->GetPatrolingComponent();
	AActor* ClosestActor = FindClosestSensedActor(UAISense_Sight::StaticClass());

	if(IsValid(ClosestActor))
	{
		if(IsValid(Blackboard))
		{
			Blackboard->SetValueAsObject(BB_CurrentTarget, ClosestActor);
			SetFocus(ClosestActor, EAIFocusPriority::Gameplay);
		}
		bIsPatrol = false;
	}
	else if(PatrolingComponent->CanPatrol())
	{
		FVector NextWayPoint = bIsPatrol ? PatrolingComponent->SelectNextWayPoint() : PatrolingComponent->SelectClosestWayPoint();
		if(IsValid(Blackboard))
		{
			Blackboard->SetValueAsVector(BB_NextLocation, NextWayPoint);
			Blackboard->SetValueAsObject(BB_CurrentTarget, nullptr);
			ClearFocus(EAIFocusPriority::Gameplay);
		}
		bIsPatrol = true;
	}
}

bool ACGCAICharacterController::IsTargetReached(FVector TargetLocation)
{
	return (TargetLocation - CachedAICharacter->GetActorLocation()).SizeSquared() <= FMath::Square(TargetReachedRadius); 
}
