// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponBarrelComponent.h"

#include "CGCCollisionTypes.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Subsystem/DebugSubsystem.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Actors/Projectiles/CGCProjectile.h"
#include "Components/DecalComponent.h"
#include "Perception/AISense_Damage.h"


bool UWeaponBarrelComponent::HitScan(FVector ShotStart, FVector ShotDirection, FVector& ShotEnd, FHitResult ShotResult)
{
	bool bHasHit = GetWorld()->LineTraceSingleByChannel(ShotResult, ShotStart, ShotEnd, ECC_Bullet);
	if(bHasHit)
	{
		ShotEnd = ShotResult.ImpactPoint;
		SetDamageByDiagram(ShotStart, ShotEnd);
		ProcessHit(ShotResult, ShotDirection);
	}
	
	return bHasHit;
}

void UWeaponBarrelComponent::LaunchProjectile(const FVector& LaunchStart, const FVector& LaunchDirection)
{
	ACGCProjectile* Projectile = ProjectilePool[CurrentProjectileIndex];
	Projectile->SetActorLocation(LaunchStart);
	Projectile->SetActorRotation(LaunchDirection.ToOrientationRotator());
	Projectile->SetProjectileActive(true);
	Projectile->OnProjectileHitDelegate.AddDynamic(this, &UWeaponBarrelComponent::UWeaponBarrelComponent::ProcessProjectileHit);
	Projectile->LaunchProjectile(LaunchDirection.GetSafeNormal());
	++CurrentProjectileIndex;
	if(CurrentProjectileIndex == ProjectilePool.Num())
	{
		CurrentProjectileIndex = 0;
	}
}

void UWeaponBarrelComponent::ProcessHit(const FHitResult& Hit, const FVector& Direction)
{
	AActor* HitActor = Hit.GetActor();
	if(GetOwner()->HasAuthority() && IsValid(HitActor))
	{
		FPointDamageEvent PointDamageEvent;
		PointDamageEvent.HitInfo = Hit;
		PointDamageEvent.ShotDirection = Direction;
		PointDamageEvent.DamageTypeClass = CurrentShootInfo.DamageTypeClass;
		AController* Controller = GetOwningController();
		HitActor->TakeDamage(CurrentShootInfo.Damage, PointDamageEvent, Controller, GetOwner());
		APawn* Pawn = GetOwningPawn();
		UAISense_Damage::ReportDamageEvent(GetWorld(), HitActor, Pawn, CurrentShootInfo.Damage, Pawn->GetActorLocation(), Hit.ImpactPoint);
	}

	UDecalComponent* DecalComponent = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), CurrentShootInfo.DefaultDecalInfo.DecalMaterial, CurrentShootInfo.DefaultDecalInfo.DecalSize, Hit.ImpactPoint, Hit.ImpactNormal.ToOrientationRotator());
	if(IsValid(DecalComponent))
	{
		DecalComponent->SetFadeScreenSize(0.0001f);
		DecalComponent->SetFadeOut(CurrentShootInfo.DefaultDecalInfo.DecalLifeTime, CurrentShootInfo.DefaultDecalInfo.DecalFadeOutTime);
	}
}

void UWeaponBarrelComponent::ProcessProjectileHit(ACGCProjectile* Projectile, const FHitResult& Hit,
	const FVector& Direction)
{
	Projectile->SetProjectileActive(false);
	Projectile->SetActorLocation(ProjectileLocation);
	Projectile->SetActorRotation(FRotator::ZeroRotator);
	Projectile->OnProjectileHitDelegate.RemoveAll(this);
	ProcessHit(Hit, Direction);
}

void UWeaponBarrelComponent::SetDamageByDiagram(FVector ShotStart, FVector ShotEnd)
{
	if(IsValid(CurrentShootInfo.FallofDiagram))
	{
		float ShotDistance = FVector::Distance(ShotStart, ShotEnd);
		CurrentShootInfo.Damage = CurrentShootInfo.Damage * CurrentShootInfo.FallofDiagram->GetFloatValue(ShotDistance);
	}
}

void UWeaponBarrelComponent::ShotInternal(const TArray<FShotInfo>& ShotsInfo)
{
	if(GetOwner()->HasAuthority())
	{
		LastShotsInfo = ShotsInfo;
	}
	
	FVector MuzzleLocation = GetComponentLocation();
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), CurrentShootInfo.MuzzleFlashFX, MuzzleLocation, GetComponentRotation());
	
	for(const FShotInfo& ShotInfo : ShotsInfo)
	{
		FVector ShotStart = ShotInfo.GetLocation();
		FVector ShotDirection = ShotInfo.GetDirection();
		FVector ShotEnd = ShotStart + CurrentShootInfo.FiringRange * ShotDirection;
		FHitResult ShotResult;

#if ENABLE_DRAW_DEBUG
		UDebugSubsystem* DebugSubsystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UDebugSubsystem>();
		bool bIsDebugEnabled = DebugSubsystem->IsCategoryEnabled(DebugCategoryRangeWeapon);
#else
		bool bIsDebugEnabled = false;
#endif

		if(IsValid(CurrentShootInfo.TraceFX))
		{
			UNiagaraComponent* TraceFXComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), CurrentShootInfo.TraceFX, MuzzleLocation, GetComponentRotation());
			if(IsValid(TraceFXComponent))
			{
				TraceFXComponent->SetVectorParameter(FXParamTraceEnd, ShotEnd);
			}
		}

		switch(CurrentShootInfo.HitRegistration)
		{
		case EHitRegistrationType::HitScan:
			{
				bool bHasHit = HitScan(ShotStart, ShotDirection, ShotEnd, ShotResult);
				if(bIsDebugEnabled && bHasHit)
				{
					DrawDebugSphere(GetWorld(), ShotEnd, 10.0f, 24, FColor::Red, false, 1.0f);
				}
				break;
			}
		case EHitRegistrationType::Projectile:
			{
				LaunchProjectile(ShotStart, ShotDirection);
			}
		}
		
		
		if(bIsDebugEnabled)
		{
			DrawDebugLine(GetWorld(), MuzzleLocation, ShotEnd, FColor::Red, false, 1.0f, 0, 3.0f);
		}
	}
}

UWeaponBarrelComponent::UWeaponBarrelComponent()
{
	SetIsReplicatedByDefault(true);
}

void UWeaponBarrelComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	FDoRepLifetimeParams RepLifetimeParams;
	RepLifetimeParams.Condition = COND_SimulatedOnly;
	RepLifetimeParams.RepNotifyCondition = REPNOTIFY_Always;
	DOREPLIFETIME_WITH_PARAMS(UWeaponBarrelComponent, LastShotsInfo, RepLifetimeParams);
	DOREPLIFETIME(UWeaponBarrelComponent, ProjectilePool);
	DOREPLIFETIME(UWeaponBarrelComponent, CurrentProjectileIndex);
}

void UWeaponBarrelComponent::BeginPlay()
{
	Super::BeginPlay();
	CurrentShootInfo = ShootParameters[0];
	CurrentShootInfoIndex = 0;

	if(GetOwnerRole() < ROLE_Authority)
	{
		return;
	}
	if(!IsValid(CurrentShootInfo.ProjectileClass))
	{
		return;
	}

	ProjectilePool.Reserve(CurrentShootInfo.ProjectilePoolSize);

	for(int32 i = 0; i < CurrentShootInfo.ProjectilePoolSize; ++i)
	{
		ACGCProjectile* Projectile = GetWorld()->SpawnActor<ACGCProjectile>(CurrentShootInfo.ProjectileClass, ProjectileLocation, FRotator::ZeroRotator);
		Projectile->SetOwner(GetOwningPawn());
		Projectile->SetProjectileActive(false);
		ProjectilePool.Add(Projectile);
	}
}

void UWeaponBarrelComponent::Shot(FVector ShotStart, FVector ShotDirection, float SpreadAngle)
{
	TArray<FShotInfo> ShotsInfo;
	
	for(int i = 0; i < CurrentShootInfo.BulletsPerShot; i++)
	{
		ShotDirection += GetBulletSpreadOffset(FMath::RandRange(0.0f, SpreadAngle), ShotDirection.ToOrientationRotator());
		ShotDirection = ShotDirection.GetSafeNormal();
		ShotsInfo.Emplace(ShotStart, ShotDirection);
	}

	if(GetOwner()->GetLocalRole() == ROLE_AutonomousProxy)
	{
		Server_Shot(ShotsInfo);
	}
	ShotInternal(ShotsInfo);
	UE_LOG(LogTemp, Warning, TEXT("Shot ShotsInfo"));
}

EWeaponFireMode UWeaponBarrelComponent::GetFireMode() const
{
	return CurrentShootInfo.FireMode;
}

void UWeaponBarrelComponent::ChangeFireMode()
{
	int32 IndexFireMode = StaticCast<int32>(CurrentShootInfo.FireMode);
	if(CurrentShootInfo.bIsCanChangeFireMode)
	{
		if(IndexFireMode == StaticEnum<EWeaponFireMode>()->GetMaxEnumValue() - 1)
		{
			IndexFireMode = 0;
			CurrentShootInfo.FireMode = StaticCast<EWeaponFireMode>(IndexFireMode);
		}
		else
		{
			IndexFireMode++;
			CurrentShootInfo.FireMode = StaticCast<EWeaponFireMode>(IndexFireMode);
		}
	}
}

void UWeaponBarrelComponent::ChangeShotType()
{
	if(CurrentShootInfoIndex == ShootParameters.Num() - 1)
	{
		CurrentShootInfo = ShootParameters[0];
		CurrentShootInfoIndex = 0;
	}
	else
	{
		CurrentShootInfoIndex++;
		CurrentShootInfo = ShootParameters[CurrentShootInfoIndex];
	}
}

FShootParameters UWeaponBarrelComponent::GetCurrentShootInfo() const
{
	return CurrentShootInfo;
}

void UWeaponBarrelComponent::Server_Shot_Implementation(const TArray<FShotInfo>& ShotsInfo)
{
	ShotInternal(ShotsInfo);
	UE_LOG(LogTemp, Warning, TEXT("Server ShotsInfo"));
}

void UWeaponBarrelComponent::OnRep_LastShotsInfo()
{
	ShotInternal(LastShotsInfo);
	UE_LOG(LogTemp, Warning, TEXT("OnRep ShotsInfo"));
}

FVector UWeaponBarrelComponent::GetBulletSpreadOffset(float Angle, FRotator ShotRotation) const
{
	float SpreadSize = FMath::Tan(Angle);
	float RotationAngle = FMath::RandRange(0.0f, 2 * PI);

	float SpreadY = FMath::Cos(RotationAngle);
	float SpreadZ = FMath::Sin(RotationAngle);

	FVector Result = (ShotRotation.RotateVector(FVector::UpVector) * SpreadZ + ShotRotation.RotateVector(FVector::RightVector) * SpreadY) * SpreadSize;
	return Result;
}

APawn* UWeaponBarrelComponent::GetOwningPawn() const
{
	APawn* PawnOwner = Cast<APawn>(GetOwner());
	if(!IsValid(PawnOwner))
	{
		PawnOwner = Cast<APawn>(GetOwner()->GetOwner());
	}

	return PawnOwner;
}

AController* UWeaponBarrelComponent::GetOwningController() const
{
	APawn* PawnOwner = GetOwningPawn();
	return IsValid(PawnOwner) ? PawnOwner->GetController() : nullptr;
}
