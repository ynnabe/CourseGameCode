// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterAttributesWidget.h"
#include "Characters/CGCBaseCharacter.h"
#include "Characters/Controllers/CGCPlayerController.h"

void UCharacterAttributesWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void UCharacterAttributesWidget::SubscribeDelegates(ACGCBaseCharacter* Character)
{
	if(!IsValid(Character))
	{
		return;
	}

	if(Character->IsLocallyControlled())
	{
		UCharacterAttributeComponent* CharacterAttributeComponent = Character->GetCharacterAttributeComponent_Mutable();
		CharacterAttributeComponent->OnHealthChanged.AddUObject(this, &UCharacterAttributesWidget::UpdateHealth);
		CharacterAttributeComponent->OnStaminaChanged.AddUObject(this, &UCharacterAttributesWidget::UpdateStamina);
		CharacterAttributeComponent->OnOxygenChanged.AddUObject(this, &UCharacterAttributesWidget::UpdateOxygen);
		CharacterAttributeComponent->OnStaminaVisibilityChanged.AddUObject(this, &UCharacterAttributesWidget::UpdateVisibilityStamina);
		CharacterAttributeComponent->OnOxygenVisibilityChanged.AddUObject(this, &UCharacterAttributesWidget::UpdateVisibilityOxygen);
	}

}

void UCharacterAttributesWidget::UpdateHealth(float NewHealth)
{
	Health = NewHealth;
}

void UCharacterAttributesWidget::UpdateStamina(float NewStamina)
{
	Stamina = NewStamina;
}

void UCharacterAttributesWidget::UpdateOxygen(float NewOxygen)
{
	Oxygen = NewOxygen;
}

void UCharacterAttributesWidget::UpdateVisibilityStamina(ESlateVisibility NewVisibility)
{
	StaminaVisibility = NewVisibility;
}

void UCharacterAttributesWidget::UpdateVisibilityOxygen(ESlateVisibility NewVisibility)
{
	OxygenVisibility = NewVisibility;
}
