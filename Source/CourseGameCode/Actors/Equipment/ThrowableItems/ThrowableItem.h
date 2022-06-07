// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ExplosiveProjectile.h"
#include "Actors/Equipment/EquipableItem.h"
#include "Actors/Projectiles/CGCProjectile.h"
#include "ThrowableItem.generated.h"

USTRUCT(BlueprintType)
struct FThrowInfo
{
	GENERATED_BODY()

	FThrowInfo() : Location(FVector::ZeroVector), Direction(FVector::ZeroVector) {};
	FThrowInfo(FVector Location, FVector Direction) : Location(Location), Direction(Direction) {};

	UPROPERTY()
	FVector Location;

	UPROPERTY()
	FVector_NetQuantizeNormal Direction;

	FVector GetLocation() const { return Location; }

	FVector GetDirection() const { return Direction; }
};

UCLASS(Blueprintable)
class COURSEGAMECODE_API AThrowableItem : public AEquipableItem
{
	GENERATED_BODY()


public:
	AThrowableItem();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void BeginPlay() override;

	virtual void SetOwner(AActor* NewOwner) override;

	void Throw();

	void ThrowInternal(const FThrowInfo& ThrowsInfo);

	EAmmoType GetAmmoType();


protected:

	void LaunchProjectile(const FVector& StartLocation, const FVector& Direction);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Projectile")
	TSubclassOf<AExplosiveProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Projectile | Net", meta=(ClampMin = 1, UIMin = 1))
	int32 ThrowProjectilePoolSize = 10;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Throwables", meta = (UIMin = -90.0f, UIMax = 90.0f, ClampMin = -90.0f, ClampMax = 90.0f))
	float ThrowAngle = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ammo")
	EAmmoType AmmoType;

private:

	UFUNCTION()
	void OnExplosion(AExplosiveProjectile* ExplosiveProjectile);

	UFUNCTION(Server, Reliable)
	void Server_Throw(const FThrowInfo& ThrowsInfo);

	UPROPERTY(ReplicatedUsing=OnRep_LastThrowInfo)
	FThrowInfo LastThrowInfo;

	UPROPERTY(Replicated)
	TArray<AExplosiveProjectile*> ThrowProjectilePool; 

	UPROPERTY(Replicated)
	int32 CurrentThrowProjectilePoolIndex = 0;

	FVector ThrowProjectileServerPosition = FVector(0.0f, 0.0f, -100.0f);
	
	UFUNCTION()
	void OnRep_LastThrowInfo();
};
