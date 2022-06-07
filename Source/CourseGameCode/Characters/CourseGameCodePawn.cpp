// Fill out your copyright notice in the Description page of Project Settings.


#include "CourseGameCodePawn.h"

#include "Camera/CameraComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/SphereComponent.h"
#include "CourseGameCode/Components/MovementComponents/CGCBaseMovementComponent.h"
#include "Engine/CollisionProfile.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/MovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"


ACourseGameCodePawn::ACourseGameCodePawn()
{
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Collision component"));
	CollisionComponent->SetSphereRadius(CollisionSphereRadius);
	CollisionComponent->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
	RootComponent = CollisionComponent;
	
	MovementComponent = CreateDefaultSubobject<UPawnMovementComponent, UCGCBaseMovementComponent>(TEXT("Movement Component"));
	MovementComponent->SetUpdatedComponent(CollisionComponent);

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComponent->bUsePawnControlRotation = 1;
	SpringArmComponent->SetupAttachment(RootComponent);

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(SpringArmComponent);
#if WITH_EDITORONLY_DATA
	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	ArrowComponent->SetupAttachment(RootComponent);
#endif
}



void ACourseGameCodePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("TurnUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("TurnRight", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("MoveForward", this, &ACourseGameCodePawn::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ACourseGameCodePawn::MoveRight);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACourseGameCodePawn::Jump);
}

void ACourseGameCodePawn::MoveForward(float Value)
{
	InputForward = Value;
	if(Value != 0.0f)
	{
		AddMovementInput(CurrentViewActor->GetActorForwardVector(), Value);
	}
}

void ACourseGameCodePawn::MoveRight(float Value)
{
	InputRight = Value;
	if(Value != 0.0f)
	{
		AddMovementInput(CurrentViewActor->GetActorRightVector(), Value);
	}
}

void ACourseGameCodePawn::Jump()
{
	checkf(MovementComponent->IsA<UCGCBaseMovementComponent>(), TEXT("ACourseGameCodePawn::Jump() can work only with UCGCBaseMovementComponent"));
	UCGCBaseMovementComponent* BaseMovement = StaticCast<UCGCBaseMovementComponent*>(MovementComponent);
	BaseMovement->JumpStart();
}

void ACourseGameCodePawn::BeginPlay()
{
	Super::BeginPlay();

	APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	CurrentViewActor = CameraManager->GetViewTarget();
	CameraManager->OnBlendComplete().AddUFunction(this, FName("OnBlendComplete"));
}

void ACourseGameCodePawn::OnBlendComplete()
{
	CurrentViewActor = GetController()->GetViewTarget();
}


