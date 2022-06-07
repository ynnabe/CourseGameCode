// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Net/UnrealNetwork.h"
#include "WeaponBarrelComponent.generated.h"

class UNiagaraSystem;
class ACGCProjectile;

UENUM(BlueprintType)
enum class EHitRegistrationType : uint8
{
	HitScan,
	Projectile
};

UENUM()
enum class EWeaponFireMode : uint8
{
	Single,
	FullAuto
};

USTRUCT(BlueprintType)
struct FDecalInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Decal info")
	UMaterialInterface* DecalMaterial;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= "Decal info")
	FVector DecalSize = FVector(2.5f, 5.0f, 5.0f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Decal info")
	float DecalLifeTime = 10.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Decal info")
	float DecalFadeOutTime = 5.0f;
};

USTRUCT(BlueprintType)
struct FShootParameters
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Barrel Attributes")
	bool bIsCanChangeFireMode = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Barrel Attributes", meta=(EditCondition = "bIsCanChangeFireMode == true"))
	EWeaponFireMode FireMode = EWeaponFireMode::Single;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Barrel Attributes")
	float FiringRange = 5000.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Barrel Attributes")
	int32 BulletsPerShot = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Barrel Attributes")
	EHitRegistrationType HitRegistration = EHitRegistrationType::HitScan;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Barrel Attributes | Net", meta=(EditCondition = "GetIsReplicated() == true", UIMin = 1, ClampMin = 1))
	int32 ProjectilePoolSize = 10;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Barrel Attributes", meta=(EditCondition = "HitRegistration == EHitRegistrationType::Projectile"))
	TSubclassOf<ACGCProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Barrel Attributes")
	bool bIsProjectileIsExplosive = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Barrel Attributes | Damage", meta=(EditCondition = "bIsProjectileIsExplosive == false"))
	float Damage = 20.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Barrel Attributes | Damage")
	TSubclassOf<UDamageType> DamageTypeClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Barrel Attributes | VFX")
	UNiagaraSystem* MuzzleFlashFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Barrel Attributes | VFX")
	UNiagaraSystem* TraceFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Barrel Attributes | Damage")
	UCurveFloat* FallofDiagram;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Barrel Attributes | Decal")
	FDecalInfo DefaultDecalInfo;
};

USTRUCT(BlueprintType)
struct FShotInfo
{
	GENERATED_BODY()

	FShotInfo() : Location_Mul_10(FVector_NetQuantize100::ZeroVector), Direction(FVector_NetQuantizeNormal::ZeroVector) {};
	FShotInfo(FVector Location, FVector Direction) : Location_Mul_10(Location * 10.0f), Direction(Direction) {};

	UPROPERTY()
	FVector_NetQuantize100 Location_Mul_10;

	UPROPERTY()
	FVector_NetQuantizeNormal Direction;

	FVector GetLocation() const { return Location_Mul_10 * 0.1f; }

	FVector GetDirection() const { return Direction; }
	
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class COURSEGAMECODE_API UWeaponBarrelComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	void ShotInternal(const TArray<FShotInfo>& ShotsInfo);

	UWeaponBarrelComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual void BeginPlay() override;
	
	void Shot(FVector ShotStart, FVector ShotDirection, float SpreadAngle);

	EWeaponFireMode GetFireMode() const;

	void ChangeFireMode();

	void ChangeShotType();

	FShootParameters GetCurrentShootInfo() const;

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Shoot Info")
	TArray<FShootParameters> ShootParameters;

private:
	UFUNCTION(Server, Reliable)
	void Server_Shot(const TArray<FShotInfo>& ShotsInfo);

	UPROPERTY(ReplicatedUsing=OnRep_LastShotsInfo)
	TArray<FShotInfo> LastShotsInfo;

	UPROPERTY(Replicated)
	TArray<ACGCProjectile*> ProjectilePool;

	UPROPERTY(Replicated)
	int32 CurrentProjectileIndex = 0;

	const FVector ProjectileLocation = FVector(0.0f, 0.0f, -100.0f);

	UFUNCTION()
	void OnRep_LastShotsInfo();
	
	FVector GetBulletSpreadOffset(float Angle, FRotator ShotRotation) const;
	bool HitScan(FVector ShotStart, FVector ShotDirection, OUT FVector& ShotEnd,
	             FHitResult ShotResult);

	void LaunchProjectile(const FVector& LaunchStart, const FVector& LaunchDirection);

	UFUNCTION()
	void ProcessHit(const FHitResult& Hit, const FVector& Direction);

	UFUNCTION()
	void ProcessProjectileHit(ACGCProjectile* Projectile, const FHitResult& Hit, const FVector& Direction);

	void SetDamageByDiagram(FVector ShotStart, FVector ShotEnd);

	APawn* GetOwningPawn() const;
	AController* GetOwningController() const;

	FShootParameters CurrentShootInfo;
	int32 CurrentShootInfoIndex;
};
