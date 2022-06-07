// Fill out your copyright notice in the Description page of Project Settings.


#include "RangeWeaponItem.h"

#include "CGCCollisionTypes.h"
#include "Characters/CGCBaseCharacter.h"
#include "Components/Weapon/WeaponBarrelComponent.h"


void ARangeWeaponItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ARangeWeaponItem, Ammo);
}

ARangeWeaponItem::ARangeWeaponItem()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("WeaponRoot"));

	WeaponMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMeshComponent->SetupAttachment(RootComponent);

	WeaponBarrelComponent = CreateDefaultSubobject<UWeaponBarrelComponent>(TEXT("WeaponBarrelComponent"));
	WeaponBarrelComponent->SetupAttachment(WeaponMeshComponent, SocketWeaponMuzzle);

	ReticleType = EReticleType::Ironsight;

	EquipSocketName = SocketCharacterWeapon;
}

void ARangeWeaponItem::StartFire()
{
	if(GetWorld()->GetTimerManager().IsTimerActive(ShotTimer))
	{
		return;
	}
	
	bIsFiring = true;
	MakeShot();
}

void ARangeWeaponItem::StopFire()
{
	bIsFiring = false;
}

bool ARangeWeaponItem::IsFiring() const
{
	return bIsFiring;
}

void ARangeWeaponItem::StartAiming()
{
	bIsAiming = true;
}

void ARangeWeaponItem::StopAiming()
{
	bIsAiming = false;
}

void ARangeWeaponItem::OnStartReload()
{
	ACGCBaseCharacter* CharacterOwner = GetCharacterOwner();
	if(!IsValid(CharacterOwner))
	{
		return;
	}

	bIsReloading = true;
	if(IsValid(CharacterReloadMontage))
	{
		float DurationMontage = CharacterOwner->PlayAnimMontage(CharacterReloadMontage);
		PlayAnimMontage(WeaponReloadMontage);
		if(ReloadType == EReloadType::Clip)
		{
			GetWorld()->GetTimerManager().SetTimer(ReloadTimer, [this]() { EndReload(true); }, DurationMontage, false);
		}
	}
	else
	{
		EndReload(true);
	}
}

void ARangeWeaponItem::StartReload()
{
	Server_StartReload();
}

void ARangeWeaponItem::EndReload(bool bIsSuccess)
{
	if(!bIsReloading)
	{
		return;
	}

	ACGCBaseCharacter* CharacterOwner = GetCharacterOwner();
	
	if(!bIsSuccess)
	{
		if(IsValid(CharacterOwner))
		{
			CharacterOwner->StopAnimMontage(CharacterReloadMontage);
		}
		StopAnimMontage(WeaponReloadMontage);
	}

	if(ReloadType == EReloadType::Bullets)
	{
		UAnimInstance* CharacterAnimInstance = IsValid(CharacterOwner) ? CharacterOwner->GetMesh()->GetAnimInstance() : nullptr;
		if(IsValid(CharacterAnimInstance))
		{
			CharacterAnimInstance->Montage_JumpToSection(MontageSectionEndReload, CharacterReloadMontage);
		}

		UAnimInstance* WeaponAnimInstance = WeaponMeshComponent->GetAnimInstance();
		if(IsValid(WeaponAnimInstance))
		{
			WeaponAnimInstance->Montage_JumpToSection(MontageSectionEndReload, WeaponReloadMontage);
		}
	}
	
	GetWorld()->GetTimerManager().ClearTimer(ReloadTimer);
	bIsReloading = false;
	if(bIsSuccess && OnReloadComplete.IsBound())
	{
		OnReloadComplete.Broadcast();
	}
}

bool ARangeWeaponItem::IsReloading() const
{
	return bIsReloading;
}

FTransform ARangeWeaponItem::GetForeGripTransform() const
{
	return WeaponMeshComponent->GetSocketTransform(SocketForeGrip);
}

float ARangeWeaponItem::GetAimFov() const
{
	return AimFov;
}

float ARangeWeaponItem::GetAimMaxSpeed() const
{
	return AimMaxSpeed;
}

int32 ARangeWeaponItem::GetAmmo() const
{
	return Ammo;
}

int32 ARangeWeaponItem::GetMaxAmmo() const
{
	return CurrentMaxAmmo;
}

void ARangeWeaponItem::SetAmmo(int32 NewAmmo)
{
	Ammo = NewAmmo;
	if(OnAmmoChanged.IsBound())
	{
		OnAmmoChanged.Broadcast(Ammo);
	}
}

bool ARangeWeaponItem::CanShoot() const
{
	return Ammo > 0;
}

EAmmoType ARangeWeaponItem::GetAmmoType()
{
	return CurrentAmmoType;
}

UWeaponBarrelComponent* ARangeWeaponItem::GetWeaponBarrelComponent() const
{
	return WeaponBarrelComponent;
}

void ARangeWeaponItem::InitializeAmmo()
{
	AmmoTypes.GenerateKeyArray(UsedAmmoTypes);
	CurrentAmmoType = UsedAmmoTypes[0];
	CurrentMaxAmmo = AmmoTypes[UsedAmmoTypes[0]];
	CurrentAmmoIndex = 0;
	CachedAmmoInClip.AddZeroed(UsedAmmoTypes.Num());
	for(int TempAmmoIndex = 0; TempAmmoIndex < UsedAmmoTypes.Num(); TempAmmoIndex++)
	{
		CachedAmmoInClip[TempAmmoIndex] = AmmoTypes[UsedAmmoTypes[TempAmmoIndex]];
	}
	CachedAmmoInClip[CurrentAmmoIndex] = CurrentMaxAmmo;
	SetAmmo(CurrentMaxAmmo);
}

void ARangeWeaponItem::BeginPlay()
{
	Super::BeginPlay();
	InitializeAmmo();
}

float ARangeWeaponItem::GetAimTurnModifier() const
{
	return AimTurnModifier;
}

float ARangeWeaponItem::GetAimLookUpModifier() const
{
	return AimLookUpModifier;
}

EReticleType ARangeWeaponItem::GetReticleType() const
{
	return bIsAiming ? AimReticleType : Super::GetReticleType();
}

void ARangeWeaponItem::ChangeCurrentAmmoType()
{
	if(CurrentAmmoIndex == UsedAmmoTypes.Num() - 1)
	{
		CachedAmmoInClip[CurrentAmmoIndex] = Ammo;
		CurrentAmmoType = UsedAmmoTypes[0];
		CurrentMaxAmmo = AmmoTypes[CurrentAmmoType];
		CurrentAmmoIndex = 0;
		Ammo = CachedAmmoInClip[CurrentAmmoIndex];
		OnAmmoChanged.Broadcast(Ammo);
		OnAmmoTypeChangedDelegate.Broadcast(CurrentAmmoType);
	}
	else
	{
		CachedAmmoInClip[CurrentAmmoIndex] = Ammo;
		CurrentAmmoType = UsedAmmoTypes[CurrentAmmoIndex + 1];
		CurrentMaxAmmo = AmmoTypes[CurrentAmmoType];
		CurrentAmmoIndex++;
		Ammo = CachedAmmoInClip[CurrentAmmoIndex];
		OnAmmoChanged.Broadcast(Ammo);
		OnAmmoTypeChangedDelegate.Broadcast(CurrentAmmoType);
	}
}

void ARangeWeaponItem::MakeShot()
{
	ACGCBaseCharacter* CharacterOwner = GetCharacterOwner();
	if(!IsValid(CharacterOwner))
	{
		return;
	}

	if(!CanShoot())
	{
		StopFire();
		if(Ammo == 0 && bAutoReload)
		{
			CharacterOwner->Reload();
		}
		return;
	}

	EndReload(false);

	CharacterOwner->PlayAnimMontage(CharacterFireMontage);
	PlayAnimMontage(WeaponFireMontage);

	FVector ShotLocation;
	FRotator ShotRotation;

	if(CharacterOwner->IsPlayerControlled())
	{
		APlayerController* PlayerController = CharacterOwner->GetController<APlayerController>();
		PlayerController->GetPlayerViewPoint(ShotLocation, ShotRotation);
	}
	else
	{
		ShotLocation = WeaponBarrelComponent->GetComponentLocation();
		ShotRotation = CharacterOwner->GetBaseAimRotation();
	}
	

	FVector ShotDirection = ShotRotation.RotateVector(FVector::ForwardVector);

	SetAmmo(Ammo - 1);
	WeaponBarrelComponent->Shot(ShotLocation, ShotDirection, GetCurrentSpreadAngle());
	
	GetWorld()->GetTimerManager().SetTimer(ShotTimer, this, &ARangeWeaponItem::OnShotTimerElapsed, GetShotTimerInterval(), false);
}

float ARangeWeaponItem::GetShotTimerInterval() const
{
	return 60.0f / RateOfFire;
}

float ARangeWeaponItem::PlayAnimMontage(UAnimMontage* AnimMontage)
{
	UAnimInstance* WeaponAnimInstance = WeaponMeshComponent->GetAnimInstance();
	float Result = 0.0f;
	if(IsValid(WeaponAnimInstance))
	{
		Result =  WeaponAnimInstance->Montage_Play(AnimMontage);
	}
	return Result;
}

void ARangeWeaponItem::Multicast_StartReload_Implementation()
{
	OnStartReload();
}

void ARangeWeaponItem::Server_StartReload_Implementation()
{
	Multicast_StartReload();
}

void ARangeWeaponItem::OnShotTimerElapsed()
{
	if(!bIsFiring)
	{
		return;
	}

	EWeaponFireMode CurrentFireMode = WeaponBarrelComponent->GetFireMode();

	switch(CurrentFireMode)
	{
		case EWeaponFireMode::Single:
		{
			StopFire();
				break;
		}
		case EWeaponFireMode::FullAuto:
			{
				MakeShot();
				break;
		}
	}
}

float ARangeWeaponItem::GetCurrentSpreadAngle() const
{
	float AngleInDegress = bIsAiming ? AimSpreadAngle : SpreadAngle;
	return FMath::DegreesToRadians(AngleInDegress);
}

void ARangeWeaponItem::StopAnimMontage(UAnimMontage* AnimMontage, float BlendOutTime)
{
	UAnimInstance* WeaponAnimInstance = WeaponMeshComponent->GetAnimInstance();
	if(IsValid(WeaponAnimInstance))
	{
		WeaponAnimInstance->Montage_Stop(BlendOutTime, WeaponReloadMontage);
	}
}
