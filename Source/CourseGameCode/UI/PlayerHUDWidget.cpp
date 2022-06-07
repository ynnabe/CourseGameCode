// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerHUDWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Characters/CGCBaseCharacter.h"
#include "Widgets/New folder/HighlightInteractable.h"


UReticleWidget* UPlayerHUDWidget::GetReticleWidget()
{
	return WidgetTree->FindWidget<UReticleWidget>(ReticleWidgetName);
}

UAmmoWidget* UPlayerHUDWidget::GetAmmoWidget()
{
	return WidgetTree->FindWidget<UAmmoWidget>(AmmoWidgetName);
}

UCharacterAttributesWidget* UPlayerHUDWidget::GetCharacterAttributesWidget()
{
	return WidgetTree->FindWidget<UCharacterAttributesWidget>(CharacterAttributesWidgetName);
}

void UPlayerHUDWidget::SetHighlightInteractableVisibility(bool bIsVisible)
{
	if(!IsValid(InteractableKey))
	{
		return;
	}
	
	if(bIsVisible)
	{
		InteractableKey->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		InteractableKey->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UPlayerHUDWidget::SetHighlightInteractableActionText(FName KeyName)
{
	if(IsValid(InteractableKey))
	{
		InteractableKey->SetActionText(KeyName);
	}
}

float UPlayerHUDWidget::GetHealthProcent() const
{
	float Result;
	APawn* Pawn = GetOwningPlayerPawn();
	ACGCBaseCharacter* CachedBaseCharacter = Cast<ACGCBaseCharacter>(Pawn);
	if(IsValid(CachedBaseCharacter))
	{
		Result = CachedBaseCharacter->GetCharacterAttributesComponent()->GetHealthProcent();
	}
	else
	{
		Result = 100.0f;
	}
	return Result;
}
