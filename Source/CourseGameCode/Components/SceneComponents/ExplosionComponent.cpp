// Fill out your copyright notice in the Description page of Project Settings.


#include "ExplosionComponent.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"


void UExplosionComponent::Explode(AController* Controller)
{
	TArray<AActor*> IgnoredActors;
	IgnoredActors.Add(GetOwner());
	UGameplayStatics::ApplyRadialDamageWithFalloff(
		GetWorld(), MaxDamage, MinDamage, GetComponentLocation(),
		InnerRadius, OuterRadius, DamageFallOf, DamageType, IgnoredActors, GetOwner(),
		Controller, ECC_Visibility);

	
	if(IsValid(ExplosionVFX))
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionVFX, GetComponentLocation());
	}
	if(OnExplosion.IsBound())
	{
		OnExplosion.Broadcast();
	}
}
