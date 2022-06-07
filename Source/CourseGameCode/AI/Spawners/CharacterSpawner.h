// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Interactive/Interface/Interactive.h"
#include "GameFramework/Actor.h"
#include "CharacterSpawner.generated.h"

class ACGCAICharacter;
UCLASS()
class COURSEGAMECODE_API ACharacterSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	
	ACharacterSpawner();

	UFUNCTION()
	void SpawnAI();

protected:
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="AI Spawner")
	TSubclassOf<ACGCAICharacter> CharacterClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ai Spawner")
	bool bSpawnOnStart = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="AI Spawner")
	bool bDoOnce = false;

	// An actor implementing IInteractable interface
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="AI Spawner")
	AActor* SpawnTriggerActor;

private:
	bool bCanSpawn = true;

	void UnSubscribeOnTrigger();

	UPROPERTY()
	TScriptInterface<IInteractable> SpawnTrigger;

	FDelegateHandle TriggerHandle;

};
