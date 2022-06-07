// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Equipment/EquipableItem.h"
#include "MeleeWeaponItem.generated.h"

class UMeleeHitRegistrator;
USTRUCT(BlueprintType)
struct FMeleeAttackDescription
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Melee Attack")
	TSubclassOf<UDamageType> DamageTypeClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Melee Attack", meta=(ClampMin = 0.0f, UIMin = 0.0f))
	float DamageAmount = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Melee Attack")
	UAnimMontage* AttackMontage;
};

UCLASS(Blueprintable)
class COURSEGAMECODE_API AMeleeWeaponItem : public AEquipableItem
{
	GENERATED_BODY()

public:
	AMeleeWeaponItem();

	void StartAttack(EMeleeAttackTypes AttackType);

	void SetIsHitregistrationEnabled(bool bIsEnable);
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Melee Attack")
	TMap<EMeleeAttackTypes, FMeleeAttackDescription> Attacks;

	virtual void BeginPlay() override;

private:
	TArray<UMeleeHitRegistrator*> HitRegistrators;
	TSet<AActor*> HitActors;
	FMeleeAttackDescription* CurrentAttack;
	FTimerHandle AttackTimer;
	
	void OnAttackElapsed();

	UFUNCTION()
	void ProcessHit(const FHitResult& HitResult, const FVector& Direction);
	
};
