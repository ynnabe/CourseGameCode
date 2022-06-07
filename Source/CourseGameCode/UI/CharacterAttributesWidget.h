// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Characters/CGCBaseCharacter.h"
#include "CharacterAttributesWidget.generated.h"

/**
 * 
 */

UCLASS()
class COURSEGAMECODE_API UCharacterAttributesWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeOnInitialized() override;

	void SubscribeDelegates(ACGCBaseCharacter* Character);

protected:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Attributes")
	float Health;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Attributes")
	float Stamina;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Attributes")
	float Oxygen;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Attributes")
	ESlateVisibility StaminaVisibility = ESlateVisibility::Hidden;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Attributes")
	ESlateVisibility OxygenVisibility = ESlateVisibility::Hidden;


private:

	UFUNCTION()
	void UpdateHealth(float NewHealth);

	UFUNCTION()
	void UpdateStamina(float NewStamina);

	UFUNCTION()
	void UpdateOxygen(float NewOxygen);

	UFUNCTION()
	void UpdateVisibilityStamina(ESlateVisibility NewVisibility);

	UFUNCTION()
	void UpdateVisibilityOxygen(ESlateVisibility NewVisibility);
};
