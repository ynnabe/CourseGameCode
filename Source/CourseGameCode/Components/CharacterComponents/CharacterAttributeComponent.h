// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CharacterAttributeComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnDeathEventSignature);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnStaminaIsOut, bool);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnAttributeChanged, float);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnAttributeVisibilityChanged, ESlateVisibility);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnHealthChanged, float);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class COURSEGAMECODE_API UCharacterAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	
	UCharacterAttributeComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	FOnDeathEventSignature OnDeathEvent;
	FOnStaminaIsOut OnStaminaIsOut;
	FOnHealthChanged OnHealthChangedDelegate;

	FOnAttributeChanged OnHealthChanged;
	FOnAttributeChanged OnStaminaChanged;
	FOnAttributeChanged OnOxygenChanged;

	FOnAttributeVisibilityChanged OnStaminaVisibilityChanged;
	FOnAttributeVisibilityChanged OnOxygenVisibilityChanged;

	bool IsAlive() { return Health > 0.0f;}

	float GetHealthProcent() const;

	void AddHealth(float HealthToAdd);

	void RestoreFullStamina();

protected:
	
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Health", meta=(UIMin = 0.0f))
	float MaxHealth = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Health", meta=(UIMin = 0.0f))
	float MaxStamina = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Stamina", meta=(UIMin = 0.0f))
	float StaminaRestoreVelocity = 10.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Stamina", meta=(UIMin = 0.0f))
	float StaminaConsumptionVelocity = 15.0f;

	void UpdateStaminaValue(float DeltaTime);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Oxygen", meta=(UIMin = 0.0f))
	float MaxOxygen = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Oxygen", meta=(UIMin = 0.0f))
	float OxygenRestoreVelocity = 10.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Oxygen", meta=(UIMin = 0.0f))
	float OxygenConsumptionVelocity = 5.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Oxygen", meta=(UIMin = 0.0f))
	float OxygenDamage = 5.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Oxygen", meta=(UIMin = 0.0f))
	float OxygenDamageInterval = 2.0f;

	void UpdateOxygenValue(float DeltaTime);

	void TakeOxygenDamage();

	
private:

	UPROPERTY(ReplicatedUsing=OnRep_Health)
	float Health = 0.0f;

	UFUNCTION()
	void OnRep_Health();

	void OnRepHealthChanged();
	
	float Stamina = 0.0f;
	float Oxygen = 0.0f;

	void UpdateHealthWidget();
	void UpdateStaminaWidget();
	void UpdateOxygenWidget();

	FTimerHandle TimerHandle_OxygenTimer;

#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	void DebugDrawAttributes();
#endif

	UFUNCTION()
	void OnTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	TWeakObjectPtr<class ACGCBaseCharacter> CachedBaseCharacterOwner;
};
