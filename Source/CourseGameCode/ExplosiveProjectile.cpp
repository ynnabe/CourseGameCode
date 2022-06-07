// Fill out your copyright notice in the Description page of Project Settings.


#include "ExplosiveProjectile.h"

#include "Components/SceneComponents/ExplosionComponent.h"

AExplosiveProjectile::AExplosiveProjectile()
{
	ExplosionComponent = CreateDefaultSubobject<UExplosionComponent>(TEXT("ExplosionComponent"));
	ExplosionComponent->SetupAttachment(GetRootComponent());
}

void AExplosiveProjectile::OnProjectileLaunched()
{
	Super::OnProjectileLaunched();
	if(bIsHaveTimer)
	{
		GetWorld()->GetTimerManager().SetTimer(ExplosionTimer, this, &AExplosiveProjectile::OnExplosionTimerElapsed, ExplosionTime, false);
	}
}

void AExplosiveProjectile::OnExplosionTimerElapsed()
{
	ExplosionComponent->Explode(GetController());
	if(OnExplosionDelegate.IsBound())
	{
		OnExplosionDelegate.Broadcast(this);
	}
}

void AExplosiveProjectile::OnCollisionHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if(!bIsHaveTimer)
	{
		ExplosionComponent->Explode(GetController());
	}
}

AController* AExplosiveProjectile::GetController()
{
	APawn* PawnOwner = Cast<APawn>(GetOwner());
	return IsValid(PawnOwner) ? PawnOwner->GetController() : nullptr;
	
}
