// Fill out your copyright notice in the Description page of Project Settings.


#include "AIPatrolingComponent.h"

#include "Actors/Navigation/PatrolPath.h"
#include "Algo/MinElement.h"


FVector UAIPatrolingComponent::SelectClosestWayPoint()
{
	FVector OwnerLocation = GetOwner()->GetActorLocation();
	const TArray<FVector> WayPoints = PatrolInfo.PatrolPath->GetWayPoints();
	FTransform PathTransform = PatrolInfo.PatrolPath->GetActorTransform();

	FVector ClosestWayPoint;
	float MinSquaredDistance = FLT_MAX;
	
	for(int i = 0; i < WayPoints.Num(); i++)
	{
		FVector WayPointWorld = PathTransform.TransformPosition(WayPoints[i]);
		float CurrentDistance = (OwnerLocation - WayPointWorld).SizeSquared();
		if(CurrentDistance < MinSquaredDistance)
		{
			MinSquaredDistance = CurrentDistance;
			ClosestWayPoint = WayPointWorld;
			CurrentWayPointIndex = i;
		}
	}
	return ClosestWayPoint;
}

FVector UAIPatrolingComponent::SelectNextWayPoint()
{
	switch (PatrolInfo.PatrolType)
	{
	case EPatrolType::Circle:
		{
			++CurrentWayPointIndex;
			if(CurrentWayPointIndex == PatrolInfo.PatrolPath->GetWayPoints().Num())
			{
				CurrentWayPointIndex = 0;
			}
		}
	case EPatrolType::PingPong:
		{
			if(CurrentWayPointIndex == PatrolInfo.PatrolPath->GetWayPoints().Num() - 1)
			{
				PatrolInfo.bIsGoingBack = true;
			}
			else if(CurrentWayPointIndex == 0)
			{
				PatrolInfo.bIsGoingBack = false;
			}
			if(PatrolInfo.bIsGoingBack)
			{
				CurrentWayPointIndex--;
			}
			else
			{
				CurrentWayPointIndex++;
			}
		}
	}
	
	const TArray<FVector> WayPoints = PatrolInfo.PatrolPath->GetWayPoints();
	FTransform PathTransform = PatrolInfo.PatrolPath->GetActorTransform();
	FVector WayPoint = PathTransform.TransformPosition(WayPoints[CurrentWayPointIndex]);
	return WayPoint;
	
}

bool UAIPatrolingComponent::CanPatrol() const
{
	return IsValid(PatrolInfo.PatrolPath) && PatrolInfo.PatrolPath->GetWayPoints().Num() > 0;
}
