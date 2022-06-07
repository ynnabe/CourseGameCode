// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AmmoWidget.h"
#include "CharacterAttributesWidget.h"
#include "ReticleWidget.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHUDWidget.generated.h"

class UHighlightInteractable;
UCLASS()
class COURSEGAMECODE_API UPlayerHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UReticleWidget* GetReticleWidget();

	UAmmoWidget* GetAmmoWidget();

	UCharacterAttributesWidget* GetCharacterAttributesWidget();

	void SetHighlightInteractableVisibility(bool bIsVisible);

	void SetHighlightInteractableActionText(FName KeyName);

protected:
	UFUNCTION(BlueprintCallable)
	float GetHealthProcent() const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Widget names")
	FName ReticleWidgetName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Widget names")
	FName AmmoWidgetName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Widget names")
	FName CharacterAttributesWidgetName;

	UPROPERTY(meta=(BindWidget))
	UHighlightInteractable* InteractableKey;
};
