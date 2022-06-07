// Fill out your copyright notice in the Description page of Project Settings.


#include "Turret.h"

#include "AI/Controllers/AITurretController.h"
#include "Components/Weapon/WeaponBarrelComponent.h"

// Sets default values
ATurret::ATurret()
{
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* TurretRoot = CreateDefaultSubobject<USceneComponent>(TEXT("TurretRootComponent"));
	SetRootComponent(TurretRoot);

	TurretBaseComponent = CreateDefaultSubobject<USceneComponent>(TEXT("TurretBaseComponent"));
	TurretBaseComponent->SetupAttachment(TurretRoot);

	TurretBarrelComponent = CreateDefaultSubobject<USceneComponent>(TEXT("TurretBarrelComponent"));
	TurretBarrelComponent->SetupAttachment(TurretBaseComponent);

	WeaponBarrelComponent = CreateDefaultSubobject<UWeaponBarrelComponent>(TEXT("WeaponBarrelComponent"));
	WeaponBarrelComponent->SetupAttachment(TurretBarrelComponent);
	SetReplicates(true);
}

void ATurret::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ATurret, CurrentTarget);
}

void ATurret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	switch (CurrentTurretState)
	{
	case ETurretState::Searching:
		{
			SearchingMovement(DeltaTime);
			break;
		}
	case ETurretState::Attack:
		{
			AttackMovement(DeltaTime);
			break;
		}
	}
}

void ATurret::MakeShot()
{
	FVector ShotLocation = WeaponBarrelComponent->GetComponentLocation();
	FVector ShotDirection = WeaponBarrelComponent->GetComponentRotation().RotateVector(FVector::ForwardVector);
	float SpreadAngle = FMath::DegreesToRadians(BulledSpreadAngle);
	WeaponBarrelComponent->Shot(ShotLocation, ShotDirection, SpreadAngle);
}

FVector ATurret::GetPawnViewLocation() const
{
	return WeaponBarrelComponent->GetComponentLocation();
}

FRotator ATurret::GetViewRotation() const
{
	return WeaponBarrelComponent->GetComponentRotation();
}

void ATurret::OnCurrentTargetSet()
{
	ETurretState NewState = IsValid(CurrentTarget) ? ETurretState::Attack : ETurretState::Searching;
	SetCurrentTurretState(NewState);
}

void ATurret::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	AAITurretController* TurretController = StaticCast<AAITurretController*>(NewController);
	if(IsValid(TurretController))
	{
		FGenericTeamId GenericTeamId((uint8)Team);
		TurretController->SetGenericTeamId(GenericTeamId);
	}
}

void ATurret::OnRep_CurrentTarget()
{
	OnCurrentTargetSet();
}

void ATurret::SearchingMovement(float DeltaTime)
{
	FRotator TurretBaseRotation = TurretBaseComponent->GetRelativeRotation();
	TurretBaseRotation.Yaw += DeltaTime * BaseSearchingRotationRate;
	TurretBaseComponent->SetRelativeRotation(TurretBaseRotation);

	FRotator TurretBarrelRotation = TurretBarrelComponent->GetRelativeRotation();
	TurretBarrelRotation.Pitch = FMath::FInterpTo(TurretBarrelRotation.Pitch, 0.0f, DeltaTime, BarrelPitchRotationRate);
	TurretBarrelComponent->SetRelativeRotation(TurretBarrelRotation);
}

void ATurret::AttackMovement(float DeltaTime)
{
	FVector BaseLookAtDirection = (CurrentTarget->GetActorLocation() - TurretBaseComponent->GetComponentLocation()).GetSafeNormal2D();
	FQuat LookAtQuat = BaseLookAtDirection.ToOrientationQuat();
	FQuat TargetQuat = FMath::QInterpTo(TurretBaseComponent->GetComponentQuat(), LookAtQuat, DeltaTime, BarrelAttackingInterpSpeed);
	TurretBaseComponent->SetWorldRotation(TargetQuat);

	FVector BarrelLookAtDirection = (CurrentTarget->GetActorLocation() - TurretBarrelComponent->GetComponentLocation()).GetSafeNormal();
	float LookAtPinchAngle = BarrelLookAtDirection.ToOrientationRotator().Pitch;
	FRotator BarrelLocalRotation = TurretBarrelComponent->GetRelativeRotation();
	BarrelLocalRotation.Pitch = FMath::FInterpTo(BarrelLocalRotation.Pitch, LookAtPinchAngle, DeltaTime, BarrelPitchRotationRate);
	TurretBarrelComponent->SetRelativeRotation(BarrelLocalRotation);
}

void ATurret::SetCurrentTurretState(ETurretState NewTurretState)
{
	bool bIsStateChanged = NewTurretState != CurrentTurretState;
	CurrentTurretState = NewTurretState;
	if(!bIsStateChanged)
	{
		return;
	}

	switch(CurrentTurretState)
	{
	case ETurretState::Searching:
		{
			GetWorld()->GetTimerManager().ClearTimer(ShotTimer);
			break;
		}
	case ETurretState::Attack:
		{
			GetWorld()->GetTimerManager().SetTimer(ShotTimer, this, &ATurret::MakeShot , GetFireInterval(), true, FireDelay);
			break;
		}
	}
}

float ATurret::GetFireInterval() const
{
	return 60 / RateOfFire;
}



