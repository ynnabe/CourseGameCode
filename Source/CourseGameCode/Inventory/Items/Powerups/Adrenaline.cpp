// Fill out your copyright notice in the Description page of Project Settings.


#include "Adrenaline.h"

bool UAdrenaline::Consume(ACGCBaseCharacter* ConsumeTarget)
{
	UCharacterAttributeComponent* CharacterAttributeComponent = ConsumeTarget->GetCharacterAttributeComponent_Mutable();
	CharacterAttributeComponent->RestoreFullStamina();
	this->ConditionalBeginDestroy();
	return true;
}
