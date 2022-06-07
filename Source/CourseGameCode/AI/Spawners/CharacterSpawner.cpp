// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterSpawner.h"

#include "AI/Characters/CGCAICharacter.h"


ACharacterSpawner::ACharacterSpawner()
{
	USceneComponent* SceneRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SpawnRoot"));
	SetRootComponent(SceneRootComponent);

}

void ACharacterSpawner::SpawnAI()
{
	if(!bCanSpawn || !IsValid(CharacterClass))
	{
		return;
	}

	ACGCAICharacter* AICharacter = GetWorld()->SpawnActor<ACGCAICharacter>(CharacterClass, GetTransform());
	if(!IsValid(AICharacter->Controller))
	{
		AICharacter->SpawnDefaultController();
	}

	if(bDoOnce)
	{
		bCanSpawn = false;
		UnSubscribeOnTrigger();
	}
}

void ACharacterSpawner::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if(PropertyChangedEvent.Property->GetName() == GET_MEMBER_NAME_STRING_CHECKED(ACharacterSpawner, SpawnTriggerActor))
	{
		SpawnTrigger = SpawnTriggerActor;
		if(SpawnTrigger.GetInterface())
		{
			if(!SpawnTrigger->HasOnInteractionCallback())
			{
				SpawnTriggerActor = nullptr;
				SpawnTrigger = nullptr;
			}
		}
		else
		{
			SpawnTriggerActor = nullptr;
			SpawnTrigger = nullptr;
		}
	}
}


void ACharacterSpawner::BeginPlay()
{
	Super::BeginPlay();

	if(SpawnTrigger.GetInterface())
	{
		TriggerHandle = SpawnTrigger->AddOnInteractionUFunction(this, FName("SpawnAI"));
	}

	if(bSpawnOnStart)
	{
		SpawnAI();
	}
}

void ACharacterSpawner::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnSubscribeOnTrigger();
	Super::EndPlay(EndPlayReason);
}

void ACharacterSpawner::UnSubscribeOnTrigger()
{
	if(TriggerHandle.IsValid() || SpawnTrigger.GetInterface())
	{
		SpawnTrigger->RemoveOnInteractionDelegate(TriggerHandle);
	}
}

