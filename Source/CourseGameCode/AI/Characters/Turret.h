// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CGCCollisionTypes.h"
#include "GameFramework/Pawn.h"
#include "Turret.generated.h"

UENUM()
enum class ETurretState : uint8
{
	Searching,
	Attack
};

class UWeaponBarrelComponent;
UCLASS()
class COURSEGAMECODE_API ATurret : public APawn
{
	GENERATED_BODY()

public:
	
	ATurret();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void Tick(float DeltaTime) override;

	void MakeShot();

	virtual FVector GetPawnViewLocation() const override;

	virtual FRotator GetViewRotation() const override;

	void OnCurrentTargetSet();

	virtual void PossessedBy(AController* NewController) override;

	UPROPERTY(ReplicatedUsing=OnRep_CurrentTarget)
	AActor* CurrentTarget = nullptr;

	UFUNCTION()
	void OnRep_CurrentTarget();
	
protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	USceneComponent* TurretBaseComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	USceneComponent* TurretBarrelComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UWeaponBarrelComponent* WeaponBarrelComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Parameters", meta=(ClampMin = 0.0f, UIMin = 0.0f))
	float BaseSearchingRotationRate = 60.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Parameters", meta=(ClampMin = 0.0f, UIMin = 0.0f))
	float BarrelPitchRotationRate = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Parameters", meta=(ClampMin = 0.0f, UIMin = 0.0f))
	float BarrelAttackingInterpSpeed = 5.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Parameters", meta=(ClampMin = 0.0f, UIMin = 0.0f))
	float MaxBarrelPitchAngle = 90.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly,Category="Parameters", meta=(ClampMin = 0.0f, UIMin = 0.0f))
	float MinBarrelPitchAngle = -30.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly,Category="Parameters", meta=(ClampMin = 1.0f, UIMin = 1.0f))
	float RateOfFire = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly,Category="Parameters", meta=(ClampMin = 0.0f, UIMin = 0.0f))
	float BulledSpreadAngle = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly,Category="Parameters", meta=(ClampMin = 0.0f, UIMin = 0.0f))
	float FireDelay = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Parameters | Team")
	ETeams Team = ETeams::Enemy;


private:
	void SearchingMovement(float DeltaTime);
	void AttackMovement(float DeltaTime);

	void SetCurrentTurretState(ETurretState NewTurretState);
	ETurretState CurrentTurretState = ETurretState::Searching;

	

	float GetFireInterval() const;

	FTimerHandle ShotTimer;
};
