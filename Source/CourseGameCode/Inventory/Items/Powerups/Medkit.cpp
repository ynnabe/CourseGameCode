// Fill out your copyright notice in the Description page of Project Settings.


#include "Medkit.h"

bool UMedkit::Consume(ACGCBaseCharacter* ConsumeTarget)
{
	UCharacterAttributeComponent* CharacterAttributeComponent = ConsumeTarget->GetCharacterAttributeComponent_Mutable();
	CharacterAttributeComponent->AddHealth(HealthRestore);
	this->ConditionalBeginDestroy();
	return true;
}
