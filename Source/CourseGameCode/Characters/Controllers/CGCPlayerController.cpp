// Fill out your copyright notice in the Description page of Project Settings.


#include "CGCPlayerController.h"
#include "../CGCBaseCharacter.h"
#include "CourseGameCode/Components/MovementComponents/GCBaseCharacterMovementComponent.h"
#include "GameFramework/PlayerInput.h"
#include "UI/CharacterAttributesWidget.h"

void ACGCPlayerController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);
	CachedBaseCharacter = Cast<ACGCBaseCharacter>(InPawn);
	if(IsValid(InPawn))
	{
		CachedBasedMovementComponent = Cast<UGCBaseCharacterMovementComponent>(InPawn->GetMovementComponent());	
	}
	if(CachedBaseCharacter.IsValid() && IsLocalController())
	{
		CreateAndInitializeWidgets();
		CachedBaseCharacter->OnInteractableObjectFoundDelegate.BindUObject(this, &ACGCPlayerController::OnInteractableObjectFind);
	}
}

bool ACGCPlayerController::GetIgnoreCameraPitch() const
{
	return bIgnoreCameraPitch;
}

void ACGCPlayerController::SetIgnoreCameraPitch(bool bIgnoreCameraPitch_In)
{
	bIgnoreCameraPitch = bIgnoreCameraPitch_In;
}

void ACGCPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	InputComponent->BindAxis("MoveForward", this, &ACGCPlayerController::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &ACGCPlayerController::MoveRight);
	InputComponent->BindAxis("TurnUp", this, &ACGCPlayerController::TurnUp);
	InputComponent->BindAxis("TurnRight", this, &ACGCPlayerController::TurnRight);
	InputComponent->BindAxis("SwimForward", this, &ACGCPlayerController::SwimForward);
	InputComponent->BindAxis("SwimRight", this, &ACGCPlayerController::SwimRight);
	InputComponent->BindAxis("SwimUp", this, &ACGCPlayerController::SwimUp);
	InputComponent->BindAxis("ClimbLadderUp", this, &ACGCPlayerController::ClimbLadderUp);
	InputComponent->BindAction("InteractWithLadder", EInputEvent::IE_Pressed, this, &ACGCPlayerController::InteractWithLadder);
	InputComponent->BindAction("InteractWithZipline", EInputEvent::IE_Pressed, this, &ACGCPlayerController::InteractWithZipline);
	InputComponent->BindAction("Mantle", EInputEvent::IE_Pressed, this, &ACGCPlayerController::Mantle);
	InputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &ACGCPlayerController::Jump);
	InputComponent->BindAction("Prone", EInputEvent::IE_Pressed, this, &ACGCPlayerController::ChangeProneState);
	InputComponent->BindAction("Crouch", EInputEvent::IE_Pressed, this, &ACGCPlayerController::ChangeCrouchState);
	InputComponent->BindAction("Slide", EInputEvent::IE_Pressed, this, &ACGCPlayerController::Slide);
	InputComponent->BindAction("Sprint", EInputEvent::IE_Pressed, this, &ACGCPlayerController::StartSprint);
	InputComponent->BindAction("Sprint", EInputEvent::IE_Released, this, &ACGCPlayerController::StopSprint);
	InputComponent->BindAction("Fire", EInputEvent::IE_Pressed, this, &ACGCPlayerController::PlayerStartFire);
	InputComponent->BindAction("Fire", EInputEvent::IE_Released, this, &ACGCPlayerController::PlayerStopFire);
	InputComponent->BindAction("Aim", EInputEvent::IE_Pressed, this, &ACGCPlayerController::StartAiming);
	InputComponent->BindAction("Aim", EInputEvent::IE_Released, this, &ACGCPlayerController::StopAiming);
	InputComponent->BindAction("Reload", EInputEvent::IE_Pressed, this, &ACGCPlayerController::Reload);
	InputComponent->BindAction("NextItem", EInputEvent::IE_Pressed, this, &ACGCPlayerController::NextItem);
	InputComponent->BindAction("PreviousItem", EInputEvent::IE_Pressed, this, &ACGCPlayerController::PreviousItem);
	InputComponent->BindAction("EquipPrimaryItem", EInputEvent::IE_Pressed, this, &ACGCPlayerController::EquipPrimaryItem);
	InputComponent->BindAction("PrimaryMeleeAttack", EInputEvent::IE_Pressed, this, &ACGCPlayerController::AttackPrimaryMelee);
	InputComponent->BindAction("SecondaryMeleeAttack", EInputEvent::IE_Pressed, this, &ACGCPlayerController::AttackSecondaryMelee);
	InputComponent->BindAction("ChangeAmmoType", EInputEvent::IE_Pressed, this, &ACGCPlayerController::ChangeFireMode);
	InputComponent->BindAction("ChangeFireMode", EInputEvent::IE_Pressed, this, &ACGCPlayerController::ChangeShotType);
	InputComponent->BindAction(ActionInteract, EInputEvent::IE_Pressed, this, &ACGCPlayerController::Interact);
	InputComponent->BindAction("OpenInventory", IE_Pressed, this, &ACGCPlayerController::UseInventory);
	InputComponent->BindAction("ConfirmSelectedWeapon", IE_Pressed, this, &ACGCPlayerController::ConfirmWeaponWheelSelection);
	
	FInputActionBinding& ToogleMenuBinding = InputComponent->BindAction("ToogleMainMenu", IE_Pressed, this, &ACGCPlayerController::ToogleMainMenu);
	ToogleMenuBinding.bExecuteWhenPaused = true;
}

void ACGCPlayerController::MoveForward(float Value)
{
	if(CachedBaseCharacter.IsValid())
	{
		CachedBasedMovementComponent->ForwardAxis = Value;
		CachedBaseCharacter->MoveForward(Value);
	}
}

void ACGCPlayerController::MoveRight(float Value)
{
	if(CachedBaseCharacter.IsValid())
	{
		CachedBasedMovementComponent->RightAxis = Value;
		CachedBaseCharacter->MoveRight(Value);
	}
}

void ACGCPlayerController::TurnUp(float Value)
{
	if(CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->TurnUp(Value);
	}
}

void ACGCPlayerController::TurnRight(float Value)
{
	if(CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->TurnRight(Value);
	}
}

void ACGCPlayerController::ChangeCrouchState()
{
	if(CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->ChangeCrouchState();
	}
}

void ACGCPlayerController::ToogleMainMenu()
{
	if(!IsValid(MainMenuWidget) || !IsValid(PlayerHUDWidget))
	{
		return;
	}
	if(MainMenuWidget->IsVisible())
	{
		MainMenuWidget->RemoveFromParent();
		PlayerHUDWidget->AddToViewport();
		SetInputMode(FInputModeGameOnly {});
		SetPause(false);
		bShowMouseCursor = false;
	}
	else
	{
		MainMenuWidget->AddToViewport();
		PlayerHUDWidget->RemoveFromParent();
		SetInputMode(FInputModeGameAndUI {});
		SetPause(true);
		bShowMouseCursor = true;
	}
}

void ACGCPlayerController::OnInteractableObjectFind(FName ActionName)
{
	if(!IsValid(PlayerHUDWidget))
	{
		return;
	}

	TArray<FInputActionKeyMapping> ActionKeys = PlayerInput->GetKeysForAction(ActionName);
	const bool HasAnyKeys = ActionKeys.Num() != 0;
	if(HasAnyKeys)
	{
		FName ActionKey = ActionKeys[0].Key.GetFName();
		PlayerHUDWidget->SetHighlightInteractableActionText(ActionKey);
	}
	PlayerHUDWidget->SetHighlightInteractableVisibility(HasAnyKeys);
}

void ACGCPlayerController::Mantle()
{
	if(CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->Mantle(false);
	}
}

void ACGCPlayerController::Jump()
{
	if(CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->Jump();
	}
}

void ACGCPlayerController::Slide()
{
	if(CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->Slide();
	}
}

void ACGCPlayerController::PlayerStartFire()
{
	if(CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->StartFire();
	}
}

void ACGCPlayerController::PlayerStopFire()
{
	if(CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->StopFire();
	}
}

void ACGCPlayerController::Interact()
{
	if(CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->Interact();
	}
}

void ACGCPlayerController::StartAiming()
{
	if(CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->StartAiming();
	}
}

void ACGCPlayerController::StopAiming()
{
	if(CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->StopAiming();
	}
}

void ACGCPlayerController::Reload()
{
	if(CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->Reload();
	}
}

void ACGCPlayerController::NextItem()
{
	if(CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->NextItem();
	}
}

void ACGCPlayerController::PreviousItem()
{
	if(CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->PreviousItem();
	}
}

void ACGCPlayerController::EquipPrimaryItem()
{
	if(CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->EquipPrimaryItem();
	}
}

void ACGCPlayerController::AttackPrimaryMelee()
{
	if(CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->AttackPrimaryMelee();
	}
}

void ACGCPlayerController::AttackSecondaryMelee()
{
	if(CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->AttackSecondaryMelee();
	}
}

void ACGCPlayerController::ChangeFireMode()
{
	if(CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->ChangeFireMode();
	}
}

void ACGCPlayerController::ChangeShotType()
{
	if(CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->ChangeShotType();
		CachedBaseCharacter->ChangeAmmoType();
	}
}

void ACGCPlayerController::StartSprint()
{
	if(CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->StartSprint();
	}
}

void ACGCPlayerController::StopSprint()
{
	if(CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->StopSprint();
	}
}

void ACGCPlayerController::ChangeProneState()
{
	if(CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->ChangeStateProne();
	}
}

void ACGCPlayerController::SwimForward(float Value)
{
	if(CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->SwimForward(Value);
	}
}

void ACGCPlayerController::SwimRight(float Value)
{
	if(CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->SwimRight(Value);
	}
}

void ACGCPlayerController::SwimUp(float Value)
{
	if(CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->SwimUp(Value);
	}
}

void ACGCPlayerController::ClimbLadderUp(float Value)
{
	if(CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->ClimbLadderUp(Value);
	}
}

void ACGCPlayerController::InteractWithLadder()
{
	if(CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->InteractWithLadder();
	}
}

void ACGCPlayerController::InteractWithZipline()
{
	if(CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->InteractWithZipline();
	}
}

void ACGCPlayerController::UseInventory()
{
	if(CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->UseInventory();
	}
}

void ACGCPlayerController::ConfirmWeaponWheelSelection()
{
	if(CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->ConfirmWeaponSelection();
	}
}

void ACGCPlayerController::CreateAndInitializeWidgets()
{
	if(!IsValid(PlayerHUDWidget))
	{
		PlayerHUDWidget = CreateWidget<UPlayerHUDWidget>(GetWorld(), PlayerHUDWidgetClass);
		if(IsValid(PlayerHUDWidget))
		{
			PlayerHUDWidget->AddToViewport();
		}
	}

	if(!IsValid(MainMenuWidget))
	{
		MainMenuWidget = CreateWidget<UUserWidget>(GetWorld(), MainMenuWidgetClass);
	}

	if(CachedBaseCharacter.IsValid() && IsValid((PlayerHUDWidget)))
	{
		UAmmoWidget* AmmoWidget = PlayerHUDWidget->GetAmmoWidget();
		if(IsValid(AmmoWidget))
		{
			AmmoWidget->SubscribeDelegates(CachedBaseCharacter.Get());
		}
	}

	if(CachedBaseCharacter.IsValid() && IsValid((PlayerHUDWidget)))
	{
		UCharacterAttributesWidget* AttributesWidget = PlayerHUDWidget->GetCharacterAttributesWidget();
		if(IsValid(AttributesWidget))
		{
			AttributesWidget->SubscribeDelegates(CachedBaseCharacter.Get());
		}
	}
	
	if(CachedBaseCharacter.IsValid() && IsValid(PlayerHUDWidget))
	{
		UReticleWidget* ReticleWidget = PlayerHUDWidget->GetReticleWidget();
		if(IsValid(ReticleWidget))
		{
			CachedBaseCharacter->AimStateChanged.AddUFunction(ReticleWidget, FName("OnAimStateChanged"));
			UCharacterEquipmentComponent* CharacterEquipmentComponent = CachedBaseCharacter->GetCharacterEquipmentComponent_Mutable();
			CharacterEquipmentComponent->OnEquipedItemChangedDelegate.AddUFunction(ReticleWidget, FName("OnEquipedItemChanged"));
		}
	}

	SetInputMode(FInputModeGameOnly {});
	bShowMouseCursor = false;
}
