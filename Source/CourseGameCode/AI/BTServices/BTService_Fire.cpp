// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_Fire.h"

#include "AIController.h"
#include "Actors/Equipment/RangeWeaponItems/RangeWeaponItem.h"
#include "AI/Characters/CGCAICharacter.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTService_Fire::UBTService_Fire()
{
	NodeName = "Fire";
}

void UBTService_Fire::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* AIController = OwnerComp.GetAIOwner();
	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();

	if(!IsValid(AIController) || !IsValid(BlackboardComponent))
	{
		return;
	}
	ACGCBaseCharacter* Character = Cast<ACGCBaseCharacter>(AIController->GetPawn());
	if(!IsValid(Character))
	{
		return;
	}

	const UCharacterEquipmentComponent* EquipmentComponent = Character->GetCharacterEquipmentComponent();
	ARangeWeaponItem* CurrentRangeWeapon = EquipmentComponent->GetCurrentRangeWeapon();

	if(!IsValid(CurrentRangeWeapon))
	{
		return;
	}

	AActor* CurrentTarget = Cast<AActor>(BlackboardComponent->GetValueAsObject(TargetKey.SelectedKeyName));
	if(!IsValid(CurrentTarget))
	{
		Character->StopFire();
		return;
	}

	float DistSq = FVector::DistSquared(CurrentTarget->GetActorLocation(), Character->GetActorLocation());
	if(DistSq > FMath::Square(MaxFireDistnace))
	{
		Character->StopFire();
		return;
	}

	if(!CurrentRangeWeapon->IsReloading() || !CurrentRangeWeapon->IsFiring())
	{
		Character->StartFire();
	}
}
