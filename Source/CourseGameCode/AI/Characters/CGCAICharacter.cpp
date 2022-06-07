// Fill out your copyright notice in the Description page of Project Settings.


#include "CGCAICharacter.h"

#include "Components/CharacterComponents/AIPatrolingComponent.h"

ACGCAICharacter::ACGCAICharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PatrolingComponent = CreateDefaultSubobject<UAIPatrolingComponent>(TEXT("PatrollingComponent"));
}

UAIPatrolingComponent* ACGCAICharacter::GetPatrolingComponent() const
{
	return PatrolingComponent;
}

UBehaviorTree* ACGCAICharacter::GetBehaviorTree() const
{
	return BehaviorTree;
}
