// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CGCCollisionTypes.h"
#include "GameFramework/Actor.h"
#include "EquipableItem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEquipmentStateChanged, bool, bIsEquipped);

class UAnimMontage;
class ACGCBaseCharacter;
UCLASS(Abstract, NotBlueprintable)
class COURSEGAMECODE_API AEquipableItem : public AActor
{
	GENERATED_BODY()

public:

	AEquipableItem();
	
	virtual void SetOwner(AActor* NewOwner) override;
	
	EEquipableItemType GetItemType() const;

	FName GetUnEquipSocketName() const;

	FName GetEquipSocketName() const;

	UAnimMontage* GetCharacterEquipAnimMontage() const;

	virtual EReticleType GetReticleType() const;
	
	ACGCBaseCharacter* GetCharacterOwner() const;

	FName GetDataTableId() const;

	bool IsSlotCompatable(EEquipmentSlots Slots);
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="EquipableItem")
	EEquipableItemType ItemType = EEquipableItemType::None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="EquipableItem")
	FName UnEquipSocketName = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="EquipableItem | Animation")
	UAnimMontage* CharacterEquipAnimMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="EquipableItem")
	FName EquipSocketName = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Reticle")
	EReticleType ReticleType = EReticleType::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="EquipableItem")
	FName DataTableID = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="EquipableItem")
	TArray<EEquipmentSlots> CompetableEquipmentSlots;

private:
	TWeakObjectPtr<ACGCBaseCharacter> CachedBaseCharacter;
};
