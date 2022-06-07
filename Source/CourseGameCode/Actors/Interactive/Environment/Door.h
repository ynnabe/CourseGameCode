// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Interactive/Interface/Interactive.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Actor.h"
#include "Door.generated.h"

UCLASS()
class COURSEGAMECODE_API ADoor : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	
	ADoor();
	
	virtual void Tick(float DeltaTime) override;

	virtual void Interact(ACGCBaseCharacter* Character) override;
	virtual FName GetActionEventName() const override;
	virtual bool HasOnInteractionCallback() const override;
	virtual FDelegateHandle AddOnInteractionUFunction(UObject* Object, const FName& FunctionName) override;
	virtual void RemoveOnInteractionDelegate(FDelegateHandle DelegateHandle) override;
	

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Interactive door")
	UStaticMeshComponent* DoorMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Interactive door")
	USceneComponent* DoorPivot;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Interactive door")
	float AngleClose = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Interactive door")
	float AngleOpen = 90.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Interactive door | Timeline settings")
	UCurveFloat* CurveDoorAnimation;
	
	virtual void BeginPlay() override;

	FOnInteraction OnInteractionEvent;
	
private:
	void InteractWithDoor();
	
	UFUNCTION()
	void UpdateDoorAnimation(float Alpha);

	UFUNCTION()
	void OnDoorAnimationFinished();
	
	FTimeline DoorOpenAnimTimeline;
	bool bIsOpened = false;
};
