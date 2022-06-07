// Fill out your copyright notice in the Description page of Project Settings.


#include "CGCProjectile.h"

#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
ACGCProjectile::ACGCProjectile()
{
 	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	CollisionComponent->InitSphereRadius(5.0f);
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComponent->SetCollisionResponseToChannels(ECR_Block);
	SetRootComponent(CollisionComponent);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MovementComponent"));
	ProjectileMovementComponent->InitialSpeed = 2000.0f;
	ProjectileMovementComponent->bAutoActivate = false;
	
	SetReplicates(true);
	SetReplicateMovement(true);
}

void ACGCProjectile::LaunchProjectile(FVector Direction)
{
	ProjectileMovementComponent->Velocity = Direction * ProjectileMovementComponent->InitialSpeed;
	CollisionComponent->IgnoreActorWhenMoving(GetOwner(), true);
	OnProjectileLaunched();
}

void ACGCProjectile::SetProjectileActive_Implementation(bool bIsProjectileActive)
{
	ProjectileMovementComponent->SetActive(bIsProjectileActive);
}

void ACGCProjectile::BeginPlay()
{
	Super::BeginPlay();
	CollisionComponent->OnComponentHit.AddDynamic(this, &ACGCProjectile::OnCollisionHit);
}

void ACGCProjectile::OnProjectileLaunched()
{
}

void ACGCProjectile::OnCollisionHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if(OnProjectileHitDelegate.IsBound())
	{
		OnProjectileHitDelegate.Broadcast(this, Hit, ProjectileMovementComponent->Velocity.GetSafeNormal());
	}
}



