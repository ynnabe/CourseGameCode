// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CGCAttributeProgressBar.generated.h"

class UProgressBar;
UCLASS()
class COURSEGAMECODE_API UCGCAttributeProgressBar : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetProgressPercantage(float Percentage);

protected:
	UPROPERTY(meta=(BindWidget))
	UProgressBar* Healthbar;
	
};
