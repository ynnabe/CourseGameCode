// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CGCCollisionTypes.h"
#include "Blueprint/UserWidget.h"
#include "ReticleWidget.generated.h"

class AEquipableItem;
UCLASS()
class COURSEGAMECODE_API UReticleWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UFUNCTION(BlueprintNativeEvent)
	void OnAimStateChanged(bool bIsAiming);

	UFUNCTION(BlueprintNativeEvent)
	void OnEquipedItemChanged(const AEquipableItem* EquipableItem);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Reticle")
	EReticleType ReticleType = EReticleType::None;

private:
	TWeakObjectPtr<const AEquipableItem> CurrentEquipedItem;

	void SetupCurrentReticle();
};
