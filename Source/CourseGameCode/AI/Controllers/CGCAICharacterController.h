// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/Controllers/CGCAIController.h"
#include "CGCAICharacterController.generated.h"

class ACGCAICharacter;
UCLASS()
class COURSEGAMECODE_API ACGCAICharacterController : public ACGCAIController
{
	GENERATED_BODY()

public:
	virtual void SetPawn(APawn* InPawn) override;

	virtual void ActorsPerceptionUpdated(const TArray<AActor*>& UpdatedActors) override;

	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;
	void SetupPatrolling();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Movement")
	float TargetReachedRadius = 100.0f;

private:
	void TryMoveToNextTarget();
	TWeakObjectPtr<ACGCAICharacter> CachedAICharacter;

	bool IsTargetReached(FVector TargetLocation);
	bool bIsPatrol = false;
};
