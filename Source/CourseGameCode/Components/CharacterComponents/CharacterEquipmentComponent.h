// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Equipment/MeleeWeaponItems/MeleeWeaponItem.h"
#include "Components/ActorComponent.h"
#include "Actors/Projectiles/CGCProjectile.h"
#include "Actors/Equipment/ThrowableItems/ThrowableItem.h"
#include "Components/Weapon/WeaponBarrelComponent.h"
#include "Net/UnrealNetwork.h"
#include "CharacterEquipmentComponent.generated.h"

typedef TArray<class AEquipableItem*, TInlineAllocator<(uint32)EEquipmentSlots::MAX>> TItemsArray;
typedef TArray<int32, TInlineAllocator<(uint32)EAmmoType::MAX>> TAmmunitionArray;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnCurrentWeaponAmmoChanged, int32);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnGrenadesCountChanged, int32);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnEquipedItemChangedDelegate, const AEquipableItem*);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnAmmoCountChanged, EAmmoType, int32);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnFireModeChanged, EWeaponFireMode);

class ARangeWeaponItem;
class AThrowableItem;
class UEquipmentViewrWidget;
class UWeaponWheelWidget;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class COURSEGAMECODE_API UCharacterEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCharacterEquipmentComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	EEquipableItemType GetCurrentEquippedItemType() const;

	ARangeWeaponItem* GetCurrentRangeWeapon() const;

	bool GetIsEquipping() const;

	FOnCurrentWeaponAmmoChanged OnCurrentWeaponAmmoChangedEvent;
	FOnGrenadesCountChanged OnGrenadesCountChangedDelegate;
	FOnEquipedItemChangedDelegate OnEquipedItemChangedDelegate;
	FOnAmmoCountChanged OnAmmoCountChanged;
	FOnFireModeChanged OnFireModeChangedDelegate;

	void ReloadCurrentWeapon();
	void ReloadAmmoInCurrentWeapon(int32 AmmoCount = 0, bool CheckIsFull = false);
	void UnEquipCurrentItem();
	void AttachCurrentItemToEquipSocket();

	void LaunchCurrentThrowableItem();
	void AttachWeaponToSocket(EEquipmentSlots EquipmentSlot, bool bIsThrowingGrenade);

	void EquipItemInSlot(EEquipmentSlots EquipmentSlot, bool bIsThrowing);

	void EquipNextItem();

	void EquipPreviousItem();

	bool CanThrow() const;

	void ChangeFireMode();
	void ChangeShotType();

	void OnChangeFireMode();

	AMeleeWeaponItem* GetCurrentMeleeWeapon() const;

	bool AddEquipmentItemInSlot(const TSubclassOf<AEquipableItem> EquipableItemClass, int32 SlotIndex);
	void RemoveEquipmentItemFromSlot(int32 SlotIndex);

	void OpenViewEquipment(APlayerController* PlayerController);
	void CloseViewEquipment();
	bool IsViewVisible() const;

	const TArray<AEquipableItem*> GetItems() const;

	void OpenWheelWidget(APlayerController* PlayerController);
	bool IsSelectingWeapon();
	void ConfirmWeaponSelection();


protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Loadout")
	TMap<EAmmoType, int32> MaxAmunitionAmount;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Loadout | Equipment")
	TMap<EEquipmentSlots, TSubclassOf<class AEquipableItem>> ItemsLoadout;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Loadout | Equipment")
	TSet<EEquipmentSlots> IgnorSlotsWhileSwitching;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Loadout | Equipment")
	EEquipmentSlots AutoEquipSlot = EEquipmentSlots::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="View")
	TSubclassOf<UEquipmentViewrWidget> ViewWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="View")
	TSubclassOf<UWeaponWheelWidget> WeaponWheelViewWidgetClass;

	void CreateEquipmentWidgets(APlayerController* PlayerController);

private:

	UFUNCTION(Server, Reliable)
	void Server_EquipItemInSlot(EEquipmentSlots Slot, bool bIsThrowing);

	UPROPERTY(Replicated)
	TArray<int32> AmmunitionArray;

	UPROPERTY(Replicated)
	TArray<AEquipableItem*> ItemsArray;
	
	int32 GetAvailableAmmunitionForCurrentWeapon();

	UFUNCTION()
	void OnWeaponReloadComplete();

	void AutoEquip();
	
	void CreateLoadout();

	void SetAmmo(EAmmoType AmmoType, int32 NewTotalAmmo = 0);

	void OnCurrentWeaponAmmoTypeChanged(EAmmoType NewAmmoType);

	void EquipAnimationFinished();

	uint32 NextItemsArraySlotIndex(uint32 CurrentSlotIndex);

	uint32 PreviousItemsArraySlotIndex(uint32 CurrentSlotIndex);

	bool bIsEquiping = false;

	FTimerHandle EquipTimer;

	UFUNCTION()
	void OnCurrentWeaponAmmoChanged(int32 NewAmmo);

	FDelegateHandle OnCurrentWeaponAmmoChangedHandle;
	FDelegateHandle OnCurrentWeaponReloadedHandle;

	EEquipmentSlots PreviuosEquipSlot;

	UPROPERTY(ReplicatedUsing=OnRep_CurrentEquipSlot)
	EEquipmentSlots CurrentEquipSlot;

	UFUNCTION()
	void OnRep_CurrentEquipSlot(EEquipmentSlots CurrentEquipSlot_Old);
	
	AEquipableItem* CurrentEquippedItem;
	ARangeWeaponItem* CurrentEquippedWeapon;
	AThrowableItem* CurrentThrowableItem;
	AMeleeWeaponItem* CurrentMeleeItem;
	
	TWeakObjectPtr<class ACGCBaseCharacter> CachedBaseCharacter;

	TWeakObjectPtr<UEquipmentViewrWidget> ViewWidget;
	TWeakObjectPtr<UWeaponWheelWidget> WeaponWheelWidget;

		
};
