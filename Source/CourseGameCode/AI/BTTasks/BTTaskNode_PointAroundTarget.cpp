// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTaskNode_PointAroundTarget.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTaskNode_PointAroundTarget::UBTTaskNode_PointAroundTarget()
{
	NodeName = "Random Point Around Target";
}

EBTNodeResult::Type UBTTaskNode_PointAroundTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	
	if(!IsValid(AIController) || !IsValid(BlackboardComponent))
	{
		return EBTNodeResult::Failed;
	}

	APawn* Pawn = AIController->GetPawn();
	if(!IsValid(Pawn))
	{
		return EBTNodeResult::Failed;
	}

	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(Pawn);
	if(!IsValid(NavSys))
	{
		return EBTNodeResult::Failed;
	}

	AActor* TargetActor = Cast<AActor>(BlackboardComponent->GetValueAsObject(TargetKey.SelectedKeyName));
	if(!IsValid(TargetActor))
	{
		return EBTNodeResult::Failed;
	}

	FNavLocation NavLocation;
	bool bIsFound = NavSys->GetRandomReachablePointInRadius(TargetActor->GetActorLocation(), Radius, NavLocation);
	if(!bIsFound)
	{
		return EBTNodeResult::Failed;
	}

	BlackboardComponent->SetValueAsVector(LocationKey.SelectedKeyName, NavLocation.Location);
	return EBTNodeResult::Succeeded;
}
