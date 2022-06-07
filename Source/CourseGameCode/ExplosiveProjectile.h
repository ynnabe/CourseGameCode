// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Projectiles/CGCProjectile.h"
#include "ExplosiveProjectile.generated.h"

class AExplosiveProjectile;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnExplosionDelegate, AExplosiveProjectile*, ExplosiveProjectile);

class UExplosionComponent;
UCLASS()
class COURSEGAMECODE_API AExplosiveProjectile : public ACGCProjectile
{
	GENERATED_BODY()

public:

	AExplosiveProjectile();

	FOnExplosionDelegate OnExplosionDelegate;

	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UExplosionComponent* ExplosionComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Explosion")
	bool bIsHaveTimer = false;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Explosion", meta=(EditCondition = "bIsHaveTimer == true"))
	float ExplosionTime = 2.0f;
	
	virtual void OnProjectileLaunched() override;

private:
	void OnExplosionTimerElapsed();

	virtual void OnCollisionHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;

	AController* GetController();
	
	FTimerHandle ExplosionTimer;
	
};
