// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractiveActor.h"

#include "Components/CapsuleComponent.h"
#include "CourseGameCode/Characters/CGCBaseCharacter.h"


// Called when the game starts or when spawned
void AInteractiveActor::BeginPlay()
{
	Super::BeginPlay();

	if(IsValid(InteractionVolume))
	{
		InteractionVolume->OnComponentBeginOverlap.AddDynamic(this, &AInteractiveActor::OnInteractionVolumeStartOverlap);
		InteractionVolume->OnComponentEndOverlap.AddDynamic(this, &AInteractiveActor::OnInteractionVolumeEndOverlap);
	}
	
}

bool AInteractiveActor::IsOverlappingCharacterCapsule(AActor* OtherActor, UPrimitiveComponent* OtherComp) const
{
	ACGCBaseCharacter* BaseCharacter = Cast<ACGCBaseCharacter>(OtherActor);
	if(!IsValid(BaseCharacter))
	{
		return false;
	}

	if(Cast<UCapsuleComponent>(OtherComp) != BaseCharacter->GetCapsuleComponent())
	{
		return false;
	}
	return true;
}

void AInteractiveActor::OnInteractionVolumeStartOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(!IsOverlappingCharacterCapsule(OtherActor, OtherComp))
	{
		return;
	}
	
	ACGCBaseCharacter* BaseCharacter = StaticCast<ACGCBaseCharacter*>(OtherActor);
	BaseCharacter->RegisterInteractiveActor(this);
}

void AInteractiveActor::OnInteractionVolumeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                      UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(!IsOverlappingCharacterCapsule(OtherActor, OtherComp))
	{
		return;
	}

	ACGCBaseCharacter* BaseCharacter = Cast<ACGCBaseCharacter>(OtherActor);
	BaseCharacter->UnregisterInteractiveActor(this);
}


