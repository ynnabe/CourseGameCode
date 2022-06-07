// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NetworkWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnNetworkWidgetClose);


UCLASS()
class COURSEGAMECODE_API UNetworkWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintAssignable)
	FOnNetworkWidgetClose OnNetworkWidgetCloseDelegate;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Network session")
	bool bIsLan;

	UFUNCTION(BlueprintPure)
	FText GetNetworkType() const;

	UFUNCTION(BlueprintCallable)
	void ToogleNetworkType();

	UFUNCTION(BlueprintCallable)
	virtual void CloseWidget();
	
	
};
