// Fill out your copyright notice in the Description page of Project Settings.


#include "NetworkWidget.h"

FText UNetworkWidget::GetNetworkType() const
{
	return bIsLan ? FText::FromString(TEXT("LAN")) : FText::FromString(TEXT("Internet"));
}

void UNetworkWidget::ToogleNetworkType()
{
	bIsLan = !bIsLan;
}

void UNetworkWidget::CloseWidget()
{
	if(OnNetworkWidgetCloseDelegate.IsBound())
	{
		OnNetworkWidgetCloseDelegate.Broadcast();
	}
	SetVisibility(ESlateVisibility::Hidden);
}
