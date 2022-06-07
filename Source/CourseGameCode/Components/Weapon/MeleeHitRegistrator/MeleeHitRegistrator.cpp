// Fill out your copyright notice in the Description page of Project Settings.


#include "MeleeHitRegistrator.h"
#include "DrawDebugHelpers.h"
#include "CGCCollisionTypes.h"
#include "Characters/CGCBaseCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Subsystem/DebugSubsystem.h"
#include "Utility/CGCTraceUtility.h"

UMeleeHitRegistrator::UMeleeHitRegistrator()
{
	PrimaryComponentTick.bCanEverTick = true;
	SphereRadius = 5.0f;
	SetCollisionProfileName(CollisionProfileNoCollision);
}

void UMeleeHitRegistrator::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	PreviuosComponentLocation = GetComponentLocation();
	if(bIsHitRegistrationEnabled)
	{
		ProcessHitRegistration();
	}
}

void UMeleeHitRegistrator::ProcessHitRegistration()
{
	FVector CurrentLocation = GetComponentLocation();
	FHitResult HitResult;
	FCollisionQueryParams CollisionQueryParams;
	AMeleeWeaponItem* CurrentMeleeWeaponItem = Cast<AMeleeWeaponItem>(GetOwner());
	const AActor* CachedBaseCharacter = CurrentMeleeWeaponItem->GetCharacterOwner();
	CollisionQueryParams.AddIgnoredActor(GetOwner());
	CollisionQueryParams.AddIgnoredActor(CachedBaseCharacter);

#if ENABLE_DRAW_DEBUG
	UDebugSubsystem* DebugSubsystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UDebugSubsystem>();
	bool bIsDebugEnabled = DebugSubsystem->IsCategoryEnabled(DebugCategoryMeleeWeapon);
#else
	bool bIsDebugEnabled = false;
#endif
	

	bool bHasHit = CGCTraceUtility::SweepSphereSingleByChanel(GetWorld(), HitResult, PreviuosComponentLocation, CurrentLocation, GetScaledSphereRadius(), ECC_Melee, CollisionQueryParams, FCollisionResponseParams::DefaultResponseParam, bIsDebugEnabled, 5.0f);
	if(bHasHit)
	{
		FVector Direction = (CurrentLocation - PreviuosComponentLocation).GetSafeNormal();
		if(OnMeleeHitRegistredDelegate.IsBound())
		{
			OnMeleeHitRegistredDelegate.Broadcast(HitResult, Direction);
		}
	}
}

void UMeleeHitRegistrator::SetIsHitRegistrationEnable(bool bIsEnable)
{
	bIsHitRegistrationEnabled = bIsEnable;
}
