// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTaskNode_PointAroundTarget.generated.h"

/**
 * 
 */
UCLASS()
class COURSEGAMECODE_API UBTTaskNode_PointAroundTarget : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTaskNode_PointAroundTarget();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="AI")
	float Radius = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="AI")
	FBlackboardKeySelector TargetKey;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="AI")
	FBlackboardKeySelector LocationKey;
	
};
