// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterAttributeComponent.h"

#include "DrawDebugHelpers.h"
#include "Components/CapsuleComponent.h"
#include "Components/SlateWrapperTypes.h"
#include "CourseGameCode/CGCCollisionTypes.h"
#include "CourseGameCode/Characters/CGCBaseCharacter.h"
#include "CourseGameCode/Components/MovementComponents/GCBaseCharacterMovementComponent.h"
#include "CourseGameCode/Subsystem/DebugSubsystem.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PhysicsVolume.h"
#include "Kismet/GameplayStatics.h"


UCharacterAttributeComponent::UCharacterAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
}

void UCharacterAttributeComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UCharacterAttributeComponent, Health);
}


float UCharacterAttributeComponent::GetHealthProcent() const
{
	return Health / MaxHealth;
}

void UCharacterAttributeComponent::AddHealth(float HealthToAdd)
{
	Health = FMath::Clamp(Health + HealthToAdd, 0.0f, MaxHealth);
	OnHealthChanged.Broadcast(Health / MaxHealth);
	OnHealthChangedDelegate.Broadcast(Health / MaxHealth);
}

void UCharacterAttributeComponent::RestoreFullStamina()
{
	Stamina = MaxStamina;
	OnStaminaChanged.Broadcast(Stamina / MaxStamina);
}

void UCharacterAttributeComponent::BeginPlay()
{
	Super::BeginPlay();

	Health = MaxHealth;
	Stamina = MaxStamina;
	Oxygen = MaxOxygen;
	
	UpdateStaminaWidget();
	UpdateOxygenWidget();

	checkf(GetOwner()->IsA<ACGCBaseCharacter>(), TEXT("UCharacterAttributeComponent::BeginPlay() can only work with ACGCBaseCharacter"));
	CachedBaseCharacterOwner = StaticCast<ACGCBaseCharacter*>(GetOwner());

	if(GetOwner()->HasAuthority())
	{
		CachedBaseCharacterOwner->OnTakeAnyDamage.AddDynamic(this, &UCharacterAttributeComponent::OnTakeAnyDamage);
	}

}

void UCharacterAttributeComponent::UpdateStaminaValue(float DeltaTime)
{
	if(CachedBaseCharacterOwner->GetBaseCharacterMovementComponent()->GetIsSprinting())
	{
		Stamina -= StaminaConsumptionVelocity * DeltaTime;
		Stamina = FMath::Clamp(Stamina, 0.0f, MaxStamina);
		UpdateStaminaWidget();
		if(Stamina != MaxStamina)
		{
			if(OnStaminaVisibilityChanged.IsBound())
			{
				OnStaminaVisibilityChanged.Broadcast(ESlateVisibility::Visible);
			}
		}
		if(Stamina == 0.0f)
		{
			if(OnStaminaIsOut.IsBound())
			{
				OnStaminaIsOut.Broadcast(true);
			}
		}
	}
	else
	{
		Stamina += StaminaRestoreVelocity * DeltaTime;
		Stamina = FMath::Clamp(Stamina, 0.0f, MaxStamina);
		UpdateStaminaWidget();
		if(Stamina == MaxStamina)
		{
			if(OnStaminaIsOut.IsBound())
			{
				OnStaminaIsOut.Broadcast(false);
			}
			if(OnStaminaVisibilityChanged.IsBound())
			{
				OnStaminaVisibilityChanged.Broadcast(ESlateVisibility::Hidden);
			}
		}
	}

}

#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
void UCharacterAttributeComponent::UpdateOxygenValue(float DeltaTime)
{
	if(CachedBaseCharacterOwner->IsSwimingUnderWater())
	{
		Oxygen -= OxygenConsumptionVelocity * DeltaTime;
		Oxygen = FMath::Clamp(Oxygen, 0.0f, MaxOxygen);
		UpdateOxygenWidget();
		if(Oxygen != MaxOxygen)
		{
			if(OnOxygenVisibilityChanged.IsBound())
			{
				OnOxygenVisibilityChanged.Broadcast(ESlateVisibility::Visible);
			}
		}
		if(Oxygen == 0.0f)
		{
			if(!GetWorld()->GetTimerManager().IsTimerActive(TimerHandle_OxygenTimer))
			{
				GetWorld()->GetTimerManager().SetTimer(TimerHandle_OxygenTimer, this, &UCharacterAttributeComponent::TakeOxygenDamage, OxygenDamageInterval, true);
			}
		}
	}
	else
	{
		Oxygen += OxygenRestoreVelocity * DeltaTime;
		Oxygen = FMath::Clamp(Oxygen, 0.0f, MaxOxygen);
		UpdateOxygenWidget();
		if(GetWorld()->GetTimerManager().IsTimerActive(TimerHandle_OxygenTimer))
		{
			GetWorld()->GetTimerManager().ClearTimer(TimerHandle_OxygenTimer);
		}
		if(Oxygen == MaxOxygen)
		{
			if(OnOxygenVisibilityChanged.IsBound())
			{
				OnOxygenVisibilityChanged.Broadcast(ESlateVisibility::Hidden);
			}
		}
	}
}

void UCharacterAttributeComponent::TakeOxygenDamage()
{
	CachedBaseCharacterOwner->TakeDamage(OxygenDamage, FDamageEvent(), CachedBaseCharacterOwner->GetController(), CachedBaseCharacterOwner->GetCharacterMovement()->GetPhysicsVolume());
}

void UCharacterAttributeComponent::OnRep_Health()
{
	OnRepHealthChanged();
}

void UCharacterAttributeComponent::OnRepHealthChanged()
{
	UpdateHealthWidget();
	if(OnHealthChangedDelegate.IsBound())
	{
		OnHealthChangedDelegate.Broadcast(GetHealthProcent());
	}
	if(Health == 0.0f)
	{
		if(OnDeathEvent.IsBound())
		{
			OnDeathEvent.Broadcast();
		}
	}
}

void UCharacterAttributeComponent::UpdateHealthWidget()
{
	if(OnHealthChanged.IsBound())
	{
		OnHealthChanged.Broadcast(Health / MaxHealth);
	}
}

void UCharacterAttributeComponent::UpdateStaminaWidget()
{
	if(OnStaminaChanged.IsBound())
	{
		OnStaminaChanged.Broadcast(Stamina / MaxStamina);
	}
}

void UCharacterAttributeComponent::UpdateOxygenWidget()
{
	if(OnOxygenChanged.IsBound())
	{
		OnOxygenChanged.Broadcast(Oxygen / MaxOxygen);
	}
}

void UCharacterAttributeComponent::DebugDrawAttributes()
{
	UDebugSubsystem* DebugSubsystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UDebugSubsystem>();
	if(!DebugSubsystem->IsCategoryEnabled(DebugCategoryCharacterAttributes))
	{
		return;
	}
	FVector HealthTextLocation = CachedBaseCharacterOwner->GetActorLocation() + (CachedBaseCharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + 25.0f) * FVector::UpVector;
	DrawDebugString(GetWorld(), HealthTextLocation, FString::Printf(TEXT("Health: %.2f"), Health), nullptr, FColor::Green, 0.0f, true);
	FVector StaminaTextLocation = CachedBaseCharacterOwner->GetActorLocation() + (CachedBaseCharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + 15.0f) * FVector::UpVector;
	DrawDebugString(GetWorld(), StaminaTextLocation, FString::Printf(TEXT("Stamina: %.2f"), Stamina), nullptr, FColor::Blue, 0.0f, true);
	FVector OxygenTextLocation = CachedBaseCharacterOwner->GetActorLocation() +(CachedBaseCharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + 5.0f) * FVector::UpVector;
	DrawDebugString(GetWorld(), OxygenTextLocation, FString::Printf(TEXT("Oxygen: %.2f"), Oxygen), nullptr, FColor::Yellow, 0.0f, true);
}
#endif

void UCharacterAttributeComponent::OnTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
                                                   AController* InstigatedBy, AActor* DamageCauser)
{
	if(!IsAlive())
	{
		return;
	}
		
	Health = FMath::Clamp(Health - Damage, 0.0f, MaxHealth);
	UE_LOG(LogDamage, Warning, TEXT("UCharacterAttributeComponent::OnTakeAnyDamage %s received %.2f amount of damage from %s"), *CachedBaseCharacterOwner->GetName(), Damage, *DamageCauser->GetName());
	
	OnRepHealthChanged();
}


void UCharacterAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	DebugDrawAttributes();
#endif

	UpdateStaminaValue(DeltaTime);
	UpdateOxygenValue(DeltaTime);
	
}

