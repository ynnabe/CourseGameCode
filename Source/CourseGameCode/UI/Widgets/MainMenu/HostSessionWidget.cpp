// Fill out your copyright notice in the Description page of Project Settings.


#include "HostSessionWidget.h"

#include "CGCGameInstance.h"
#include "Kismet/GameplayStatics.h"

void UHostSessionWidget::CreateSession()
{
	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(GetWorld());
	check(GameInstance->IsA<UCGCGameInstance>());
	UCGCGameInstance* CGCGameInstance = StaticCast<UCGCGameInstance*>(GetGameInstance());

	CGCGameInstance->LaunchLobby(4, ServerName, bIsLan);
}
