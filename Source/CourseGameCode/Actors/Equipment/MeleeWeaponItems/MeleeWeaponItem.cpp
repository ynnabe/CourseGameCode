// Fill out your copyright notice in the Description page of Project Settings.


#include "MeleeWeaponItem.h"

#include "Characters/CGCBaseCharacter.h"
#include "Components/Weapon/MeleeHitRegistrator/MeleeHitRegistrator.h"

AMeleeWeaponItem::AMeleeWeaponItem()
{
	EquipSocketName = SocketCharacterWeapon;
}

void AMeleeWeaponItem::StartAttack(EMeleeAttackTypes AttackType)
{
	ACGCBaseCharacter* CharacterOwner = GetCharacterOwner();

	if(!IsValid(CharacterOwner))
	{
		return;
	}

	HitActors.Empty();

	CurrentAttack = Attacks.Find(AttackType);
	if(CurrentAttack && IsValid(CurrentAttack->AttackMontage))
	{
		UAnimInstance* CharacterAnimInstance = CharacterOwner->GetMesh()->GetAnimInstance();
		if(IsValid(CharacterAnimInstance))
		{
			float MontageDuration = CharacterAnimInstance->Montage_Play(CurrentAttack->AttackMontage, 1.0f, EMontagePlayReturnType::Duration);
			GetWorld()->GetTimerManager().SetTimer(AttackTimer, this, &AMeleeWeaponItem::OnAttackElapsed, MontageDuration, false);
		}
		else
		{
			OnAttackElapsed();
		}
	}
}

void AMeleeWeaponItem::SetIsHitregistrationEnabled(bool bIsEnable)
{
	HitActors.Empty();
	for(UMeleeHitRegistrator* HitRegistrator : HitRegistrators)
	{
		HitRegistrator->SetIsHitRegistrationEnable(bIsEnable);
	}
}

void AMeleeWeaponItem::BeginPlay()
{
	Super::BeginPlay();
	GetComponents<UMeleeHitRegistrator>(HitRegistrators);
	for(UMeleeHitRegistrator* HitRegistrator : HitRegistrators)
	{
		HitRegistrator->OnMeleeHitRegistredDelegate.AddDynamic(this, &AMeleeWeaponItem::ProcessHit);
	}
}

void AMeleeWeaponItem::OnAttackElapsed()
{
	CurrentAttack = nullptr;
	SetIsHitregistrationEnabled(false);
}

void AMeleeWeaponItem::ProcessHit(const FHitResult& HitResult, const FVector& Direction)
{
	if(!CurrentAttack)
	{
		return;
	}
	AActor* HitActor = HitResult.GetActor();
	if(!IsValid(HitActor))
	{
		return;
	}
	if(HitActors.Contains(HitActor))
	{
		return;
	}

	FPointDamageEvent PointDamageEvent;
	PointDamageEvent.HitInfo = HitResult;
	PointDamageEvent.ShotDirection = Direction;
	PointDamageEvent.DamageTypeClass = CurrentAttack->DamageTypeClass;
	ACGCBaseCharacter* CharacterOwner = GetCharacterOwner();
	AController* Controller = IsValid(CharacterOwner) ? CharacterOwner->GetController() : nullptr;
	HitActor->TakeDamage(CurrentAttack->DamageAmount, PointDamageEvent, Controller, CharacterOwner);

	HitActors.Add(HitActor);
}
