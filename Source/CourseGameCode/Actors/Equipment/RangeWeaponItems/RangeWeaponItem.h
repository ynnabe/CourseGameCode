// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Equipment/EquipableItem.h"
#include "Components/Weapon/WeaponBarrelComponent.h"
#include "RangeWeaponItem.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnReloadComplete);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnAmmoChanged, int32);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnAmmoTypeChanged, EAmmoType);

UENUM()
enum class EReloadType : uint8
{
	Clip,
	Bullets
};


class UAnimMontage;
UCLASS(Blueprintable)
class COURSEGAMECODE_API ARangeWeaponItem : public AEquipableItem
{
	GENERATED_BODY()

public:

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	ARangeWeaponItem();

	void StartFire();

	void StopFire();

	bool IsFiring() const;

	void StartAiming();

	void StopAiming();

	void OnStartReload();

	void StartReload();

	void EndReload(bool bIsSuccess);

	bool IsReloading() const;

	FTransform GetForeGripTransform() const;

	float GetAimFov() const;

	float GetAimMaxSpeed() const;

	int32 GetAmmo() const;

	int32 GetMaxAmmo() const;

	void SetAmmo(int32 NewAmmo);

	bool CanShoot() const;

	EAmmoType GetAmmoType();

	UWeaponBarrelComponent* GetWeaponBarrelComponent() const;
	void InitializeAmmo();

	FOnAmmoChanged OnAmmoChanged;
	FOnReloadComplete OnReloadComplete;
	FOnAmmoTypeChanged OnAmmoTypeChangedDelegate;

	float GetAimTurnModifier() const;

	float GetAimLookUpModifier() const;

	virtual EReticleType GetReticleType() const override;

	void ChangeCurrentAmmoType();

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	class USkeletalMeshComponent* WeaponMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	class UWeaponBarrelComponent* WeaponBarrelComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Animations | Weapon")
	UAnimMontage* WeaponFireMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Animations | Weapon")
	UAnimMontage* WeaponReloadMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Animations | Weapon")
	EReloadType ReloadType = EReloadType::Clip;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Animations | Character")
	UAnimMontage* CharacterFireMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Animations | Character")
	UAnimMontage* CharacterReloadMontage;

	// Rate of fire in round per minute
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon | Parameters", meta = (ClampMin = 1.0f, UIMin = 1.0f))
	float RateOfFire = 600.0f;

	// Bullet spread half angle in degrees
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon | Parameters | Aiming", meta = (ClampMin = 0.0f, UIMin = 0.0f, ClampMax = 10.0f, UIMax = 10.0f))
	float SpreadAngle = 2.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon | Parameters | Aiming", meta = (ClampMin = 0.0f, UIMin = 0.0f, ClampMax = 2.0f, UIMax = 2.0f))
	float AimSpreadAngle = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon | Parameters | Aiming", meta = (ClampMin = 0.0f, UIMin = 0.0f, ClampMax = 600.0f, UIMax = 600.0f))
	float AimMaxSpeed = 200.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon | Parameters | Aiming", meta = (ClampMin = 0.0f, UIMin = 0.0f, ClampMax = 120.0f, UIMax = 120.0f))
	float AimFov = 60.0f;

	// Ammo types which this weapon uses. First value is ammotype, second value is max ammo in clip
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon | Parameters | Ammo")
	TMap<EAmmoType, int32> AmmoTypes;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon | Parameters | Ammo")
	bool bAutoReload = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon | Parameters | Aiming", meta = (ClampMin = 0.0f, UIMin = 0.0f, ClampMax = 1.0f, UIMax = 1.0f))
	float AimTurnModifier = 0.5;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon | Parameters | Aiming", meta = (ClampMin = 0.0f, UIMin = 0.0f, ClampMax = 1.0f, UIMax = 1.0f))
	float AimLookUpModifier = 0.5;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon | Parameters | Aiming")
	EReticleType AimReticleType = EReticleType::Ironsight;

private:
	void MakeShot();
	float GetShotTimerInterval() const;
	float PlayAnimMontage(UAnimMontage* AnimMontage);
	bool bIsAiming = false;

	UFUNCTION(Server, Reliable)
	void Server_StartReload();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_StartReload();

	UPROPERTY(Replicated)
	int32 Ammo = 0;
	
	TArray<EAmmoType> UsedAmmoTypes;
	EAmmoType CurrentAmmoType;
	int32 CurrentAmmoIndex;
	int32 CurrentMaxAmmo;
	TArray<int32> CachedAmmoInClip;
	bool bIsReloading = false;
	bool bIsFiring = false;

	void OnShotTimerElapsed();

	float GetCurrentSpreadAngle() const;

	void StopAnimMontage(UAnimMontage* AnimMontage, float BlendOutTime = 0);

	FTimerHandle ShotTimer;
	FTimerHandle ReloadTimer;
};
