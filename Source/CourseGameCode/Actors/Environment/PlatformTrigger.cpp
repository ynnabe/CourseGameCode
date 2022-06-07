// Fill out your copyright notice in the Description page of Project Settings.


#include "PlatformTrigger.h"
#include "CGCCollisionTypes.h"
#include "Characters/CGCBaseCharacter.h"
#include "Components/BoxComponent.h"
#include "Net/UnrealNetwork.h"

APlatformTrigger::APlatformTrigger()
{
	bReplicates = true;
	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetCollisionProfileName(CollisionProfilePawnInteractionVolume);
	SetRootComponent(TriggerBox);
}

void APlatformTrigger::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	FDoRepLifetimeParams RepParams;
	RepParams.RepNotifyCondition = REPNOTIFY_Always;
	DOREPLIFETIME(APlatformTrigger, bIsActivated);
}

void APlatformTrigger::BeginPlay()
{
	Super::BeginPlay();
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &APlatformTrigger::OnTriggerOverlapBegin);
	TriggerBox->OnComponentEndOverlap.AddDynamic(this, &APlatformTrigger::OnTriggerOverlapEnd);
}

void APlatformTrigger::SetIsActivated(bool bIsActivated_In)
{
	bIsActivated = bIsActivated_In;
	if (OnTriggerActivated.IsBound())
	{
		OnTriggerActivated.Broadcast(bIsActivated_In);
	}
}

void APlatformTrigger::Multicast_SetIsActivated_Implementation(bool bIsActivated_In)
{
	bIsActivated = bIsActivated_In;
	if (OnTriggerActivated.IsBound())
	{
		OnTriggerActivated.Broadcast(bIsActivated_In);
	}
}

void APlatformTrigger::Server_SetIsActivated_Implementation(bool bIsActivated_In)
{
	SetIsActivated(bIsActivated_In);
}

void APlatformTrigger::OnRep_IsActivated(bool bIsActivated_Old)
{
	SetIsActivated(bIsActivated);
}

void APlatformTrigger::OnTriggerOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ACGCBaseCharacter* OtherPawn = Cast<ACGCBaseCharacter>(OtherActor);
	if (!IsValid(OtherPawn))
	{
		return;
	}

	if(GetLocalRole() == ROLE_Authority)
	{
		OverlappedPawns.AddUnique(OtherPawn);

		if (!bIsActivated && OverlappedPawns.Num() > 0)
		{
			SetIsActivated(true);
			bIsActivated = true;
		}
	}
}

void APlatformTrigger::OnTriggerOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ACGCBaseCharacter* OtherPawn = Cast<ACGCBaseCharacter>(OtherActor);
	if (!IsValid(OtherPawn))
	{
		return;
	}
	
	if(GetLocalRole() == ROLE_Authority)
	{
		OverlappedPawns.RemoveSingleSwap(OtherPawn);

		if (bIsActivated && OverlappedPawns.Num() == 0)
		{
			SetIsActivated(false);
			bIsActivated = false;
		}
	}
}
