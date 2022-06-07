// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterEquipmentComponent.h"
#include "Actors/Equipment/RangeWeaponItems/RangeWeaponItem.h"
#include "Characters/CGCBaseCharacter.h"
#include "UI/Widgets/Inventory/EquipmentViewrWidget.h"
#include "UI/Widgets/Inventory/WeaponWheelWidget.h"


UCharacterEquipmentComponent::UCharacterEquipmentComponent()
{
	SetIsReplicatedByDefault(true);
}

void UCharacterEquipmentComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UCharacterEquipmentComponent, CurrentEquipSlot);
	DOREPLIFETIME(UCharacterEquipmentComponent, AmmunitionArray);
	DOREPLIFETIME(UCharacterEquipmentComponent, ItemsArray);
}

EEquipableItemType UCharacterEquipmentComponent::GetCurrentEquippedItemType() const
{
	EEquipableItemType Result = EEquipableItemType::None;
	if(IsValid(CurrentEquippedItem))
	{
		Result = CurrentEquippedItem->GetItemType();
	}
	return Result;
}

ARangeWeaponItem* UCharacterEquipmentComponent::GetCurrentRangeWeapon() const
{
	return CurrentEquippedWeapon;
}

bool UCharacterEquipmentComponent::GetIsEquipping() const
{
	return bIsEquiping;
}

void UCharacterEquipmentComponent::ReloadCurrentWeapon()
{
	check(IsValid(CurrentEquippedWeapon));
	int32 AvailableAmmunition = GetAvailableAmmunitionForCurrentWeapon();
	if(AvailableAmmunition <= 0)
	{
		return;
	}

	CurrentEquippedWeapon->StartReload();

}

void UCharacterEquipmentComponent::UnEquipCurrentItem()
{
	if(IsValid(CurrentEquippedItem))
	{
		CurrentEquippedItem->AttachToComponent(CachedBaseCharacter->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, CurrentEquippedItem->GetUnEquipSocketName());
	}
	if(IsValid(CurrentEquippedWeapon))
	{
		CurrentEquippedWeapon->StopFire();
		CurrentEquippedWeapon->EndReload(false);
		CurrentEquippedWeapon->OnAmmoChanged.Remove(OnCurrentWeaponAmmoChangedHandle);
		CurrentEquippedWeapon->OnReloadComplete.Remove(OnCurrentWeaponReloadedHandle);
	}

	PreviuosEquipSlot = CurrentEquipSlot;
	CurrentEquipSlot = EEquipmentSlots::None;
}

void UCharacterEquipmentComponent::AttachCurrentItemToEquipSocket()
{
	if(IsValid(CurrentEquippedItem))
	{
		CurrentEquippedItem->AttachToComponent(CachedBaseCharacter->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, CurrentEquippedItem->GetEquipSocketName());
	}
}

void UCharacterEquipmentComponent::LaunchCurrentThrowableItem()
{
	if(IsValid(CurrentThrowableItem))
	{
		CurrentThrowableItem->Throw();
		SetAmmo(CurrentThrowableItem->GetAmmoType(), AmmunitionArray[(uint32)CurrentThrowableItem->GetAmmoType()] - 1);
		bIsEquiping = false;
		EquipItemInSlot(PreviuosEquipSlot, false);
	}
}

void UCharacterEquipmentComponent::AttachWeaponToSocket(EEquipmentSlots EquipmentSlot, bool bIsThrowingGrenade)
{
	UAnimMontage* CharacterEquipAnimMontage = CurrentEquippedItem->GetCharacterEquipAnimMontage();
	if(IsValid(CharacterEquipAnimMontage))
	{
		bIsEquiping = true;
		float EquipMontageDuration = CachedBaseCharacter->PlayAnimMontage(CharacterEquipAnimMontage);
		if(IsValid(CurrentThrowableItem) && bIsThrowingGrenade)
		{
			AttachCurrentItemToEquipSocket();
		}
		else
		{
			GetWorld()->GetTimerManager().SetTimer(EquipTimer, this, &UCharacterEquipmentComponent::EquipAnimationFinished, EquipMontageDuration, false);
		}
	}
	else
	{
		AttachCurrentItemToEquipSocket();
	}
	CurrentEquipSlot = EquipmentSlot;
	if(IsValid(CurrentEquippedWeapon))
	{
		OnCurrentWeaponAmmoChangedHandle = CurrentEquippedWeapon->OnAmmoChanged.AddUFunction(this, FName("OnCurrentWeaponAmmoChanged"));
		OnCurrentWeaponReloadedHandle = CurrentEquippedWeapon->OnReloadComplete.AddUFunction(this, FName("OnWeaponReloadComplete"));
		CurrentEquippedWeapon->OnAmmoTypeChangedDelegate.AddUObject(this, &UCharacterEquipmentComponent::OnCurrentWeaponAmmoTypeChanged);
		OnCurrentWeaponAmmoChanged(CurrentEquippedWeapon->GetAmmo());
		OnChangeFireMode();
	}
}

void UCharacterEquipmentComponent::EquipItemInSlot(EEquipmentSlots EquipmentSlot, bool bIsThrowing)
{
	if(bIsEquiping)
	{
		return;
	}
	
	UnEquipCurrentItem();
	
	CurrentEquippedItem = ItemsArray[(uint32)EquipmentSlot];
	CurrentEquippedWeapon = Cast<ARangeWeaponItem>(CurrentEquippedItem);
	CurrentThrowableItem = Cast<AThrowableItem>(CurrentEquippedItem);
	CurrentMeleeItem = Cast<AMeleeWeaponItem>(CurrentEquippedItem);

	if(IsValid(CurrentThrowableItem))
	{
		if(!CanThrow())
		{
			EquipItemInSlot(PreviuosEquipSlot, false);
			return;
		}
		AttachWeaponToSocket(EquipmentSlot, bIsThrowing);
	}

	if(OnEquipedItemChangedDelegate.IsBound())
	{
		OnEquipedItemChangedDelegate.Broadcast(CurrentEquippedItem);
	}
	
	if(IsValid(CurrentEquippedItem))
	{
		AttachWeaponToSocket(EquipmentSlot, bIsThrowing);
	}

	if(GetOwner()->GetLocalRole() == ROLE_AutonomousProxy)
	{
	 	Server_EquipItemInSlot(CurrentEquipSlot, false);
	}
}

bool UCharacterEquipmentComponent::CanThrow() const
{
	return AmmunitionArray[(uint32)CurrentThrowableItem->GetAmmoType()] > 0;
}

void UCharacterEquipmentComponent::ChangeFireMode()
{
	if(IsValid(CurrentEquippedWeapon))
	{
		UWeaponBarrelComponent* CurrentWeaponBarrelComponent = CurrentEquippedWeapon->GetWeaponBarrelComponent();
		CurrentWeaponBarrelComponent->ChangeFireMode();
		OnChangeFireMode();
	}
}

void UCharacterEquipmentComponent::ChangeShotType()
{
	if(IsValid(CurrentEquippedWeapon))
	{
		UWeaponBarrelComponent* CurrentWeaponBarrelComponent = CurrentEquippedWeapon->GetWeaponBarrelComponent();
		CurrentWeaponBarrelComponent->ChangeShotType();
	}
}

void UCharacterEquipmentComponent::OnChangeFireMode()
{
	UWeaponBarrelComponent* CurrentWeaponBarrelComponent = CurrentEquippedWeapon->GetWeaponBarrelComponent();
	EWeaponFireMode NewFireMode = CurrentWeaponBarrelComponent->GetCurrentShootInfo().FireMode;
	if(OnFireModeChangedDelegate.IsBound())
	{
		OnFireModeChangedDelegate.Broadcast(NewFireMode);
	}
}

AMeleeWeaponItem* UCharacterEquipmentComponent::GetCurrentMeleeWeapon() const
{
	return CurrentMeleeItem;
}

bool UCharacterEquipmentComponent::AddEquipmentItemInSlot(const TSubclassOf<AEquipableItem> EquipableItemClass,
                                                          int32 SlotIndex)
{
	if(!IsValid(EquipableItemClass))
	{
		return false;
	}
	AEquipableItem* DefaultObjectItem = EquipableItemClass->GetDefaultObject<AEquipableItem>();

	if(!DefaultObjectItem->IsSlotCompatable((EEquipmentSlots)SlotIndex))
	{
		return false;
	}

	if(!IsValid(ItemsArray[SlotIndex]))
	{
		AEquipableItem* Item = GetWorld()->SpawnActor<AEquipableItem>(EquipableItemClass);
		Item->AttachToComponent(CachedBaseCharacter->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, Item->GetUnEquipSocketName());
		Item->SetOwner(CachedBaseCharacter.Get());
		ItemsArray[SlotIndex] = Item;
	}

	return true;
}

void UCharacterEquipmentComponent::RemoveEquipmentItemFromSlot(int32 SlotIndex)
{
	if((uint32)CurrentEquipSlot == SlotIndex)
	{
		UnEquipCurrentItem();
	}
	ItemsArray[SlotIndex]->Destroy();
	ItemsArray[SlotIndex] = nullptr;
}

void UCharacterEquipmentComponent::OpenViewEquipment(APlayerController* PlayerController)
{
	if(!ViewWidget.IsValid())
	{
		CreateEquipmentWidgets(PlayerController);
	}
	if(!ViewWidget->IsVisible())
	{
		ViewWidget->AddToViewport();
	}
}

void UCharacterEquipmentComponent::CloseViewEquipment()
{
	if(ViewWidget->IsVisible())
	{
		ViewWidget->RemoveFromParent();
	}
}

bool UCharacterEquipmentComponent::IsViewVisible() const
{
	bool Result = false;
	if(ViewWidget.IsValid())
	{
		Result = ViewWidget->IsVisible();
	}

	return Result;
}

const TArray<AEquipableItem*> UCharacterEquipmentComponent::GetItems() const
{
	return ItemsArray;
}

void UCharacterEquipmentComponent::OpenWheelWidget(APlayerController* PlayerController)
{
	if(!WeaponWheelWidget.IsValid())
	{
		CreateEquipmentWidgets(PlayerController);
	}

	if(!WeaponWheelWidget->IsVisible())
	{
		WeaponWheelWidget->AddToViewport();
	}
}

bool UCharacterEquipmentComponent::IsSelectingWeapon()
{
	return WeaponWheelWidget.IsValid() && WeaponWheelWidget->IsVisible();
}

void UCharacterEquipmentComponent::ConfirmWeaponSelection()
{
	WeaponWheelWidget->ConfirmSelectedSegment();
}

void UCharacterEquipmentComponent::EquipNextItem()
{
	if(CachedBaseCharacter->IsPlayerControlled())
	{
		if(IsSelectingWeapon())
		{
			WeaponWheelWidget->NextSegment();
		}
		else
		{
			APlayerController* PlayerController = CachedBaseCharacter->GetController<APlayerController>();
			OpenWheelWidget(PlayerController);
		}
		return;
	}
	
	uint32 CurrentSlotIndex = (uint32)CurrentEquipSlot;
	uint32 NextSlotIndex = NextItemsArraySlotIndex(CurrentSlotIndex);
	while(CurrentSlotIndex != NextSlotIndex && IgnorSlotsWhileSwitching.Contains((EEquipmentSlots)NextSlotIndex) || !IsValid(ItemsArray[NextSlotIndex]))
	{
		NextSlotIndex = NextItemsArraySlotIndex(NextSlotIndex);
	}
	if(CurrentSlotIndex != NextSlotIndex)
	{
		EquipItemInSlot((EEquipmentSlots)NextSlotIndex, false);
	}
}

void UCharacterEquipmentComponent::EquipPreviousItem()
{
	if(CachedBaseCharacter->IsPlayerControlled())
	{
		if(IsSelectingWeapon())
		{
			WeaponWheelWidget->PreviosSegment();
		}
		else
		{
			APlayerController* PlayerController = CachedBaseCharacter->GetController<APlayerController>();
			OpenWheelWidget(PlayerController);
		}
		return;
	}
	
	uint32 CurrentSlotIndex = (uint32)CurrentEquipSlot;
	uint32 PreviousSlotIndex = PreviousItemsArraySlotIndex(CurrentSlotIndex);
	while(CurrentSlotIndex != PreviousSlotIndex && IgnorSlotsWhileSwitching.Contains((EEquipmentSlots)PreviousSlotIndex) && !IsValid(ItemsArray[PreviousSlotIndex]))
	{
		PreviousSlotIndex = PreviousItemsArraySlotIndex(PreviousSlotIndex);
	}
	if(CurrentSlotIndex != PreviousSlotIndex)
	{
		EquipItemInSlot((EEquipmentSlots)PreviousSlotIndex, false);
	}
}

void UCharacterEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();
	checkf(GetOwner()->IsA<ACGCBaseCharacter>(), TEXT("UCharacterEquipmentComponent::BeginPlay() can work only with ACGCBaseCharacter"));
	CachedBaseCharacter = StaticCast<ACGCBaseCharacter*>(GetOwner());
	CreateLoadout();
	AutoEquip();
}

void UCharacterEquipmentComponent::Server_EquipItemInSlot_Implementation(EEquipmentSlots Slot, bool bIsThrowing)
{
	EquipItemInSlot(Slot, bIsThrowing);
}

void UCharacterEquipmentComponent::CreateEquipmentWidgets(APlayerController* PlayerController)
{
	if(!IsValid(PlayerController))
	{
		return;
	}

	ViewWidget = CreateWidget<UEquipmentViewrWidget>(PlayerController, ViewWidgetClass);
	ViewWidget->InitializeEquipmentWidget(this);

	WeaponWheelWidget = CreateWidget<UWeaponWheelWidget>(PlayerController, WeaponWheelViewWidgetClass);
	WeaponWheelWidget->InitializeWeaponWheelWidget(this);
}

int32 UCharacterEquipmentComponent::GetAvailableAmmunitionForCurrentWeapon()
{
	check(GetCurrentRangeWeapon());
	return AmmunitionArray[(uint32)GetCurrentRangeWeapon()->GetAmmoType()];
}

void UCharacterEquipmentComponent::OnWeaponReloadComplete()
{
	ReloadAmmoInCurrentWeapon();
}

void UCharacterEquipmentComponent::AutoEquip()
{
	if(AutoEquipSlot != EEquipmentSlots::None)
	{
		EquipItemInSlot(AutoEquipSlot, false);
	}
}

void UCharacterEquipmentComponent::ReloadAmmoInCurrentWeapon(int32 AmmoCount, bool CheckIsFull)
{
	int32 AvailableAmmunition = GetAvailableAmmunitionForCurrentWeapon();
	
	int32 CurrentAmmo = CurrentEquippedWeapon->GetAmmo();
	int32 AmmoToReload = CurrentEquippedWeapon->GetMaxAmmo() - CurrentEquippedWeapon->GetAmmo();
	int32 ReloadedAmmo = FMath::Min(AvailableAmmunition, AmmoToReload);
	if(AmmoCount > 0)
	{
		ReloadedAmmo = FMath::Min(ReloadedAmmo, AmmoCount);
	}
	AmmunitionArray[(uint32)CurrentEquippedWeapon->GetAmmoType()] -= ReloadedAmmo;
	CurrentEquippedWeapon->SetAmmo(ReloadedAmmo + CurrentAmmo);

	if(CheckIsFull)
	{
		AvailableAmmunition = AmmunitionArray[(uint32)CurrentEquippedWeapon->GetAmmoType()];
		bool bIsFullyReloaded = CurrentEquippedWeapon->GetAmmo() == CurrentEquippedWeapon->GetMaxAmmo();
		if(!AvailableAmmunition == 0 && bIsFullyReloaded)
		{
			CurrentEquippedWeapon->EndReload(true);
		}
	}
}

void UCharacterEquipmentComponent::CreateLoadout()
{
	if(GetOwner()->GetLocalRole() < ROLE_Authority)
	{
		return;
	}
	
	AmmunitionArray.AddZeroed((uint32)EAmmoType::MAX);
	for(const TPair<EAmmoType, int32>& AmmoPair : MaxAmunitionAmount)
	{
		AmmunitionArray[(uint32)AmmoPair.Key] = FMath::Max(AmmoPair.Value, 0);
	}

	SetAmmo(EAmmoType::FragGrenades, 5);
	
	ItemsArray.AddZeroed((uint32)EEquipmentSlots::MAX);
	for(const TPair<EEquipmentSlots, TSubclassOf<AEquipableItem>>& ItemPair : ItemsLoadout)
	{
		if(!IsValid(ItemPair.Value))
		{
			continue;
		}
		AddEquipmentItemInSlot(ItemPair.Value, (int32)ItemPair.Key);
	}
}

void UCharacterEquipmentComponent::SetAmmo(EAmmoType AmmoType, int32 NewTotalAmmo)
{
	AmmunitionArray[(uint32)AmmoType] = NewTotalAmmo;
	if(OnAmmoCountChanged.IsBound())
	{
		OnAmmoCountChanged.Broadcast(AmmoType, NewTotalAmmo);
	}
}

void UCharacterEquipmentComponent::OnCurrentWeaponAmmoTypeChanged(EAmmoType NewAmmoType)
{
	SetAmmo(NewAmmoType, AmmunitionArray[(uint32)NewAmmoType]);
}

void UCharacterEquipmentComponent::EquipAnimationFinished()
{
	bIsEquiping = false;
	AttachCurrentItemToEquipSocket();
}

uint32 UCharacterEquipmentComponent::NextItemsArraySlotIndex(uint32 CurrentSlotIndex)
{
	if(CurrentSlotIndex == ItemsArray.Num()-1)
	{
		return 0;
	}
	else
	{
		return CurrentSlotIndex + 1;
	}
}

uint32 UCharacterEquipmentComponent::PreviousItemsArraySlotIndex(uint32 CurrentSlotIndex)
{
	if(CurrentSlotIndex == 0)
	{
		return ItemsArray.Num() - 1;
	}
	else
	{
		return CurrentSlotIndex - 1;
	}
}

void UCharacterEquipmentComponent::OnCurrentWeaponAmmoChanged(int32 NewAmmo)
{
	SetAmmo(CurrentEquippedWeapon->GetAmmoType(), GetAvailableAmmunitionForCurrentWeapon());
	if(OnCurrentWeaponAmmoChangedEvent.IsBound())
	{
		OnCurrentWeaponAmmoChangedEvent.Broadcast(NewAmmo);
	}
}

void UCharacterEquipmentComponent::OnRep_CurrentEquipSlot(EEquipmentSlots CurrentEquipSlot_Old)
{
	EquipItemInSlot(CurrentEquipSlot, false);
}


