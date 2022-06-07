// Fill out your copyright notice in the Description page of Project Settings.


#include "AITurretController.h"

#include "AI/Characters/Turret.h"
#include "Perception/AIPerceptionComponent.h"
#include "Algo/MinElement.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "Media/Public/IMediaCache.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Damage.h"
#include "Perception/AISense_Sight.h"

void AAITurretController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);
	if(IsValid(InPawn))
	{
		checkf(InPawn->IsA<ATurret>(), TEXT("AAITurretController::SetPawn can be work only with ATurret"));
		CachedTurret = StaticCast<ATurret*>(InPawn);
	}
	else
	{
		CachedTurret = nullptr;
	}
}

void AAITurretController::ActorsPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
	Super::ActorsPerceptionUpdated(UpdatedActors);
	if(!CachedTurret.IsValid())
	{
		return;
	}
	TArray<AActor*> DamageActors;
	
	AActor* ClosestActor = FindClosestSensedActor(UAISense_Sight::StaticClass());

	PerceptionComponent->GetCurrentlyPerceivedActors(UAISense_Damage::StaticClass(), DamageActors);
	
	auto ClosestDamageActor = Algo::MinElementBy(DamageActors, [&](const auto DamageActor){
	return (DamageActor->GetActorLocation() - CachedTurret->GetActorLocation()).SizeSquared();
	});

	if(IsValid(ClosestActor))
	{
		CachedTurret->CurrentTarget = ClosestActor;
		CachedTurret->OnCurrentTargetSet();
	}

	if(ClosestDamageActor)
	{
		CachedTurret->CurrentTarget = *ClosestDamageActor;
		CachedTurret->OnCurrentTargetSet();
	}

	
}
