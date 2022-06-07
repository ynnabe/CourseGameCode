// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "CGCProjectile.generated.h"

class ACGCProjectile;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnProjectileHitDelegate, ACGCProjectile*, Projectile, const FHitResult&, HitResult, const FVector&, Direction);

UCLASS()
class COURSEGAMECODE_API ACGCProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	ACGCProjectile();

	UFUNCTION(BlueprintCallable)
	void LaunchProjectile(FVector Direction);

	UPROPERTY(BlueprintAssignable)
	FOnProjectileHitDelegate OnProjectileHitDelegate;

	UFUNCTION(BlueprintNativeEvent)
	void SetProjectileActive(bool bIsProjectileActive);

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Components")
	class USphereComponent* CollisionComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Components")
	class UProjectileMovementComponent* ProjectileMovementComponent;

	virtual void BeginPlay() override;
	
	virtual void OnProjectileLaunched();

private:

	UFUNCTION()
	virtual void OnCollisionHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

};
