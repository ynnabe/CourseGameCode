// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widgets/MainMenu/NetworkWidget.h"
#include "JoinSessionWidget.generated.h"

class UCGCGameInstance;
UENUM(BlueprintType, Blueprintable)
enum class ESearchingSessionState : uint8
{
	None,
	Searching,
	SessionFound
};


UCLASS()
class COURSEGAMECODE_API UJoinSessionWidget : public UNetworkWidget
{
	GENERATED_BODY()

protected:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Network session")
	ESearchingSessionState CurrentSearchingState;

	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable)
	void FindOnlineSession();

	UFUNCTION(BlueprintCallable)
	void JoinOnlineSession();

	UFUNCTION(BlueprintNativeEvent)
	void OnMatchFound(bool bIsSuccesful);

	virtual void CloseWidget() override;

private:
	TWeakObjectPtr<UCGCGameInstance> CGCGameInstance;
	
};
