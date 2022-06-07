// Fill out your copyright notice in the Description page of Project Settings.


#include "JoinSessionWidget.h"

#include "CGCGameInstance.h"
#include "Kismet/GameplayStatics.h"

void UJoinSessionWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(GetWorld());
	check(GameInstance->IsA<UCGCGameInstance>());
	CGCGameInstance = StaticCast<UCGCGameInstance*>(GetGameInstance());
}

void UJoinSessionWidget::FindOnlineSession()
{
	CGCGameInstance->OnMatchFoundDelegate.AddDynamic(this, &UJoinSessionWidget::OnMatchFound);
	CGCGameInstance->FindMatch(bIsLan);
	CurrentSearchingState = ESearchingSessionState::Searching;
}

void UJoinSessionWidget::JoinOnlineSession()
{
	CGCGameInstance->JoinMatch();
}

void UJoinSessionWidget::CloseWidget()
{
	CGCGameInstance->OnMatchFoundDelegate.RemoveAll(this);
	Super::CloseWidget();
}

void UJoinSessionWidget::OnMatchFound_Implementation(bool bIsSuccesful)
{
	CurrentSearchingState = bIsSuccesful ? ESearchingSessionState::SessionFound : ESearchingSessionState::None;
}
