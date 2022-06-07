// Fill out your copyright notice in the Description page of Project Settings.

#include "ThrowableItem.h"
#include "Characters/CGCBaseCharacter.h"

AThrowableItem::AThrowableItem()
{
	SetReplicates(true);
}

void AThrowableItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	FDoRepLifetimeParams RepParams;
	RepParams.Condition = COND_SimulatedOnly;
	RepParams.RepNotifyCondition = REPNOTIFY_Always;
	DOREPLIFETIME_WITH_PARAMS(AThrowableItem, LastThrowInfo, RepParams);
	DOREPLIFETIME(AThrowableItem, ThrowProjectilePool);
	DOREPLIFETIME(AThrowableItem, CurrentThrowProjectilePoolIndex);
}

void AThrowableItem::BeginPlay()
{
	Super::BeginPlay();

	
}

void AThrowableItem::SetOwner(AActor* NewOwner)
{
	Super::SetOwner(NewOwner);
	if(IsValid(NewOwner))
	{
		if(GetOwner()->GetLocalRole() < ROLE_Authority)
		{
			return;
		}

		if(!IsValid(ProjectileClass))
		{
			return;
		}

		ThrowProjectilePool.Reserve(ThrowProjectilePoolSize);
		for(int32 i = 0; i < ThrowProjectilePoolSize; ++i)
		{
			AExplosiveProjectile* ThrowProjectile = GetWorld()->SpawnActor<AExplosiveProjectile>(ProjectileClass, ThrowProjectileServerPosition, FRotator::ZeroRotator);
			ThrowProjectile->SetOwner(GetOwner());
			ThrowProjectile->SetProjectileActive(false);
			ThrowProjectilePool.Add(ThrowProjectile);
		}
	}
}

void AThrowableItem::Throw()
{
	ACGCBaseCharacter* CharacterOwner = GetCharacterOwner();
	if(!IsValid(CharacterOwner))
	{
		return;
	}

	APlayerController* PlayerController = CharacterOwner->GetController<APlayerController>();
	if(!IsValid(PlayerController))
	{
		return;
	}

	FVector PlayerViewPoint;
	FRotator PlayerViewRotation;
	
	PlayerController->GetPlayerViewPoint(PlayerViewPoint, PlayerViewRotation);
	FTransform PlayerViewTransform(PlayerViewRotation, PlayerViewPoint);

	FVector PlayerViewDirection = PlayerViewRotation.RotateVector(FVector::ForwardVector);
	FVector PlayerViewUpVector = PlayerViewRotation.RotateVector(FVector::UpVector);

	FVector LaunchDirection = (PlayerViewDirection + FMath::Tan(FMath::DegreesToRadians(ThrowAngle)) * PlayerViewUpVector).GetSafeNormal();
	
	FVector ThrowableSocketLocation = CharacterOwner->GetMesh()->GetSocketLocation(SocketThrowable);
	FVector SocketInViewSpace = PlayerViewTransform.InverseTransformPosition(ThrowableSocketLocation);
	
	FVector SpawnLocation = PlayerViewPoint + PlayerViewDirection * SocketInViewSpace.X;

	FThrowInfo ThrowsInfo = FThrowInfo(SpawnLocation, LaunchDirection);
	
	if(GetOwner()->GetLocalRole() == ROLE_AutonomousProxy)
	{
		Server_Throw(ThrowsInfo);
	}
	ThrowInternal(ThrowsInfo);
	UE_LOG(LogTemp, Warning, TEXT("Throw ThrowsInfo: Location: %s, Direction %s"), *ThrowsInfo.Location.ToString(), *ThrowsInfo.Direction.ToString());
}

void AThrowableItem::ThrowInternal(const FThrowInfo& ThrowsInfo)
{
	if(GetOwner()->HasAuthority())
	{
		LastThrowInfo = ThrowsInfo;
	}

	FVector ThrowStart = ThrowsInfo.GetLocation();
	FVector ThrowDirection = ThrowsInfo.GetDirection();

	LaunchProjectile(ThrowStart, ThrowDirection);
}

EAmmoType AThrowableItem::GetAmmoType()
{
	return AmmoType;
}

void AThrowableItem::LaunchProjectile(const FVector& StartLocation, const FVector& Direction)
{
	AExplosiveProjectile* ThrowProjectile = ThrowProjectilePool[CurrentThrowProjectilePoolIndex];
	ThrowProjectile->SetActorLocation(StartLocation);
	ThrowProjectile->SetActorRotation(Direction.ToOrientationRotator());
	ThrowProjectile->SetProjectileActive(true);
	ThrowProjectile->OnExplosionDelegate.AddDynamic(this, &AThrowableItem::OnExplosion);
	ThrowProjectile->LaunchProjectile(Direction.GetSafeNormal());
	++CurrentThrowProjectilePoolIndex;
	if(CurrentThrowProjectilePoolIndex == ThrowProjectilePool.Num())
	{
		CurrentThrowProjectilePoolIndex = 0;
	}
}

void AThrowableItem::OnExplosion(AExplosiveProjectile* ExplosiveProjectile)
{
	ExplosiveProjectile->SetProjectileActive(false);
	ExplosiveProjectile->SetActorLocation(ThrowProjectileServerPosition);
	ExplosiveProjectile->SetActorRotation(FRotator::ZeroRotator);
	ExplosiveProjectile->OnExplosionDelegate.RemoveAll(this);
}

void AThrowableItem::OnRep_LastThrowInfo()
{
	ThrowInternal(LastThrowInfo);
	UE_LOG(LogTemp, Warning, TEXT("OnRep LastThrowsInfo: Location: %s, Direction %s"), *LastThrowInfo.Location.ToString(), *LastThrowInfo.Direction.ToString());
}

void AThrowableItem::Server_Throw_Implementation(const FThrowInfo& ThrowsInfo)
{
	ThrowInternal(ThrowsInfo);
	UE_LOG(LogTemp, Warning, TEXT("Server_Throw ThrowsInfo: Location: %s, Direction %s"), *ThrowsInfo.Location.ToString(), *ThrowsInfo.Direction.ToString());
}
