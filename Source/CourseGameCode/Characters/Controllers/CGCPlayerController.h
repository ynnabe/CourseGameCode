// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CourseGameCode/Characters/CGCBaseCharacter.h"
#include "GameFramework/PlayerController.h"
#include "UI/PlayerHUDWidget.h"
#include "CGCPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class COURSEGAMECODE_API ACGCPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void SetPawn(APawn* InPawn) override;

	bool GetIgnoreCameraPitch() const;

	void SetIgnoreCameraPitch(bool bIgnoreCameraPitch_In);

protected:
	virtual void SetupInputComponent() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Widgets")
	TSubclassOf<UPlayerHUDWidget> PlayerHUDWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Widgets")
	TSubclassOf<UUserWidget> MainMenuWidgetClass;

private:

	void MoveForward(float Value);
	void MoveRight(float Value);
	void TurnUp(float Value);
	void TurnRight(float Value);
	void ChangeCrouchState();

	void ToogleMainMenu();
	UUserWidget* MainMenuWidget = nullptr;

	void OnInteractableObjectFind(FName ActionName);
	
	void Mantle();
	
	void Jump();
	void Slide();
	
	void PlayerStartFire();
	void PlayerStopFire();

	void Interact();

	void StartAiming();
	void StopAiming();

	void Reload();

	void NextItem();
	void PreviousItem();

	void EquipPrimaryItem();

	void AttackPrimaryMelee();
	void AttackSecondaryMelee();

	void ChangeFireMode();
	void ChangeShotType();

	void StartSprint();
	void StopSprint();

	void ChangeProneState();

	void SwimForward(float Value);
	void SwimRight(float Value);
	void SwimUp(float Value);

	void ClimbLadderUp(float Value);
	void InteractWithLadder();
	void InteractWithZipline();

	void UseInventory();

	void ConfirmWeaponWheelSelection();
	
	TSoftObjectPtr<ACGCBaseCharacter> CachedBaseCharacter;
	TSoftObjectPtr<UGCBaseCharacterMovementComponent> CachedBasedMovementComponent;
	
	void CreateAndInitializeWidgets();
	UPlayerHUDWidget* PlayerHUDWidget = nullptr;
	bool bIgnoreCameraPitch = false;
	
};
