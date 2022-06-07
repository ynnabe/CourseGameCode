// Fill out your copyright notice in the Description page of Project Settings.


#include "PatrolPath.h"


const TArray<FVector>& APatrolPath::GetWayPoints() const
{
	return WayPoints;
}
