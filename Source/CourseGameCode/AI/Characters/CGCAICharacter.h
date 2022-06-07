// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Characters/CGCBaseCharacter.h"
#include "CGCAICharacter.generated.h"

class UAIPatrolingComponent;
UCLASS(Blueprintable)
class COURSEGAMECODE_API ACGCAICharacter : public ACGCBaseCharacter
{
	GENERATED_BODY()

public:
	ACGCAICharacter(const FObjectInitializer& ObjectInitializer);

	UAIPatrolingComponent* GetPatrolingComponent() const;

	UBehaviorTree* GetBehaviorTree() const;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UAIPatrolingComponent* PatrolingComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Components")
	UBehaviorTree* BehaviorTree;
};
