// Fill out your copyright notice in the Description page of Project Settings.


#include "CGCBaseCharacter.h"

#include "AIController.h"
#include "Actors/Equipment/RangeWeaponItems/RangeWeaponItem.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/CharacterComponents/CharacterEquipmentComponent.h"
#include "Controllers/CGCPlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CourseGameCode/Components/MovementComponents/GCBaseCharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "CourseGameCode/Components/DetectionComponents/LedgeDetectorComponent.h"
#include "Curves/CurveVector.h"
#include "CourseGameCode/Actors/Interactive/Environment/Ladder.h"
#include "CourseGameCode/Actors/Interactive/Environment/Zipline.h"
#include "CourseGameCode/Components/CharacterComponents/CharacterAttributeComponent.h"
#include "GameFramework/PhysicsVolume.h"
#include "Net/UnrealNetwork.h"
#include "UI/Widgets/CGCAttributeProgressBar.h"


ACGCBaseCharacter::ACGCBaseCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UGCBaseCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	CGCBaseCharacterMovementComponent = StaticCast<UGCBaseCharacterMovementComponent*>(GetCharacterMovement());
	LedgeDetectorComponent = CreateDefaultSubobject<ULedgeDetectorComponent>(TEXT("Ledge Detector"));
	CharacterAttributeComponent = CreateDefaultSubobject<UCharacterAttributeComponent>(TEXT("Character Attributes"));
	CharacterEquipmentComponent = CreateDefaultSubobject<UCharacterEquipmentComponent>(TEXT("Character Equipment"));
	CharacterInventoryComponent = CreateDefaultSubobject<UCharacterInventoryComponent>(TEXT("Character Inventory"));
	HealthbarProgressComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthbarComponent"));
	HealthbarProgressComponent->SetupAttachment(GetCapsuleComponent());

	GetMesh()->CastShadow = true;
	GetMesh()->bCastDynamicShadow = true;
}

void ACGCBaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACGCBaseCharacter, bIsMantling);
	DOREPLIFETIME(ACGCBaseCharacter, bIsSliding);
}

void ACGCBaseCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if(OnInteractableObjectFoundDelegate.IsBound())
	{
		OnInteractableObjectFoundDelegate.Unbind();
	}
	Super::EndPlay(EndPlayReason);
}

void ACGCBaseCharacter::StartSprint()
{
	bIsSprintRequested = true;
	if(bIsCrouched)
	{
		UnCrouch();
	}
}

void ACGCBaseCharacter::StopSprint()
{
	bIsSprintRequested = false;
}

void ACGCBaseCharacter::StartFire()
{
	if(!CanFire())
	{
		return;
	}
	ARangeWeaponItem* CurrentRangeWeapon = CharacterEquipmentComponent->GetCurrentRangeWeapon();
	if(IsValid(CurrentRangeWeapon))
	{
		CurrentRangeWeapon->StartFire();
	}
}

void ACGCBaseCharacter::StopFire()
{
	ARangeWeaponItem* CurrentRangeWeapon = CharacterEquipmentComponent->GetCurrentRangeWeapon();
	if(IsValid(CurrentRangeWeapon))
	{
		CurrentRangeWeapon->StopFire();
	}
}

void ACGCBaseCharacter::Interact()
{
	if(LineObject.GetInterface())
	{
		LineObject->Interact(this);
	}
}

void ACGCBaseCharacter::StartAiming()
{
	ARangeWeaponItem* CurrentRangeWeapon = CharacterEquipmentComponent->GetCurrentRangeWeapon();
	if(!IsValid(CurrentRangeWeapon))
	{
		return;
	}
	bIsAiming = true;
	CurrentAimingMovementSpeed = CurrentRangeWeapon->GetAimMaxSpeed();
	CurrentRangeWeapon->StartAiming();
	OnStartAiming();
}

void ACGCBaseCharacter::StopAiming()
{
	if(!bIsAiming)
	{
		return;
	}
	ARangeWeaponItem* CurrentRangeWeapon = CharacterEquipmentComponent->GetCurrentRangeWeapon();
	if(IsValid(CurrentRangeWeapon))
	{
		CurrentRangeWeapon->StopAiming();
	}
	bIsAiming = false;
	CurrentAimingMovementSpeed = 0.0f;
	OnStopAiming();
}

FRotator ACGCBaseCharacter::GetAimOffset()
{
	FVector AimDirectionWorld = GetBaseAimRotation().Vector();
	FVector AimDirectionLocal = GetTransform().InverseTransformVectorNoScale(AimDirectionWorld);
	FRotator Result = AimDirectionLocal.ToOrientationRotator();
	
	return Result;
}

void ACGCBaseCharacter::OnStartAiming_Implementation()
{
	OnStartAimingInternal();
}

void ACGCBaseCharacter::OnStopAiming_Implementation()
{
	OnStopAimingInternal();
}

float ACGCBaseCharacter::GetAimingMovementSpeed() const
{
	return CurrentAimingMovementSpeed;
}

bool ACGCBaseCharacter::IsAiming() const
{
	return bIsAiming;
}

void ACGCBaseCharacter::Reload()
{
	if(IsValid(CharacterEquipmentComponent->GetCurrentRangeWeapon()) && !CharacterEquipmentComponent->GetCurrentRangeWeapon()->IsReloading())
	{
		CharacterEquipmentComponent->ReloadCurrentWeapon();
	}
}

void ACGCBaseCharacter::NextItem()
{
	CharacterEquipmentComponent->EquipNextItem();
}

void ACGCBaseCharacter::PreviousItem()
{
	CharacterEquipmentComponent->EquipPreviousItem();
}

void ACGCBaseCharacter::EquipPrimaryItem()
{
	CharacterEquipmentComponent->EquipItemInSlot(EEquipmentSlots::PrimaryItemSlot, true);
}

bool ACGCBaseCharacter::PickUp(TWeakObjectPtr<UInventoryItem> Item)
{
	bool Result = false;
	if(CharacterInventoryComponent->HasFreeSlots())
	{
		CharacterInventoryComponent->AddItem(Item, 1);
		Result = true;
	}
	return Result;
}

void ACGCBaseCharacter::AttackPrimaryMelee()
{
	AMeleeWeaponItem* CurrentMeleeItem = CharacterEquipmentComponent->GetCurrentMeleeWeapon();
	if(IsValid(CurrentMeleeItem))
	{
		CurrentMeleeItem->StartAttack(EMeleeAttackTypes::Primary);
	}
}

void ACGCBaseCharacter::AttackSecondaryMelee()
{
	AMeleeWeaponItem* CurrentMeleeItem = CharacterEquipmentComponent->GetCurrentMeleeWeapon();
	if(IsValid(CurrentMeleeItem))
	{
		CurrentMeleeItem->StartAttack(EMeleeAttackTypes::Secondary);
	}
}

void ACGCBaseCharacter::ChangeFireMode()
{
	CharacterEquipmentComponent->ChangeFireMode();
}

void ACGCBaseCharacter::ChangeAmmoType()
{
	ARangeWeaponItem* CurrentRangeWeaponItem = CharacterEquipmentComponent->GetCurrentRangeWeapon();
	if(IsValid(CurrentRangeWeaponItem))
	{
		CurrentRangeWeaponItem->ChangeCurrentAmmoType();
	}
}

void ACGCBaseCharacter::ChangeShotType()
{
	CharacterEquipmentComponent->ChangeShotType();
}

void ACGCBaseCharacter::Falling()
{
	Super::Falling();
	GetCharacterMovement()->bNotifyApex = true;
}

void ACGCBaseCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	float FallHeight = (CurrentFallingHeight - GetActorLocation().Z) * 0.01f;
	if(FallHeight > MaxFallingHeight)
	{
		if(OnHardLanding.IsBound() && CharacterAttributeComponent->IsAlive())
		{
			OnHardLanding.Execute();
		}
		SetIgnoreLookMoveInput(true);
	}
	if(IsValid(FallDamageCurve))
	{
		float DamageAmount = FallDamageCurve->GetFloatValue(FallHeight);
		TakeDamage(DamageAmount, FDamageEvent(), GetController(), Hit.Actor.Get());
	}
}

void ACGCBaseCharacter::NotifyJumpApex()
{
	Super::NotifyJumpApex();
	CurrentFallingHeight = GetActorLocation().Z;
}

void ACGCBaseCharacter::Prone()
{
	if(CGCBaseCharacterMovementComponent)
	{
		if(CanProne())
		{
			CGCBaseCharacterMovementComponent->bIsWantsToProne = true;
		}
	}
}

void ACGCBaseCharacter::UnProne(bool bIsFullHeight)
{
	if(CGCBaseCharacterMovementComponent)
	{
		CGCBaseCharacterMovementComponent->bIsWantsToProne = false;
		CGCBaseCharacterMovementComponent->bIsFullHeight = bIsFullHeight;
	}
}

bool ACGCBaseCharacter::CanProne()
{
	return !bIsProned && CGCBaseCharacterMovementComponent && GetRootComponent() && !GetRootComponent()->IsSimulatingPhysics();
}

void ACGCBaseCharacter::Jump()
{
	if(!CGCBaseCharacterMovementComponent->IsProning() && !CGCBaseCharacterMovementComponent->IsSliding())
	{
		Super::Jump();
	}
	else if(CGCBaseCharacterMovementComponent->IsProning())
	{
		UnProne(true);
	}
	if(CGCBaseCharacterMovementComponent->IsWallRuning())
	{
		FVector JumpDirection = FVector::ZeroVector;
		if(CGCBaseCharacterMovementComponent->GetSide() == EWallRunSide::Right)
		{
			JumpDirection = FVector::CrossProduct(CGCBaseCharacterMovementComponent->GetWallRunDirection(), FVector::UpVector);
		}
		else if(CGCBaseCharacterMovementComponent->GetSide() == EWallRunSide::Left)
		{
			JumpDirection = FVector::CrossProduct(FVector::UpVector, CGCBaseCharacterMovementComponent->GetWallRunDirection());
		}

		JumpDirection += FVector::UpVector;
		JumpDirection *= CGCBaseCharacterMovementComponent->GetJumpOffFromWall();
		LaunchCharacter( JumpDirection, false, true);
	}
}

void ACGCBaseCharacter::InteractWithLadder()
{
	if(GetBaseCharacterMovementComponent()->IsOnLadder())
	{
		GetBaseCharacterMovementComponent()->DetachFromLadder(EDetachFromLadderMethod::JumpOff);
	}
	else
	{
		const ALadder* AvailableLadder = GetAvailableLadder();
		if(IsValid(AvailableLadder) && !CGCBaseCharacterMovementComponent->IsSliding())
		{
			if(AvailableLadder->GetIsOnTop())
			{
				PlayAnimMontage(AvailableLadder->GetAttachFromTopAnimMontage());
			}
			GetBaseCharacterMovementComponent()->AttachToLadder(AvailableLadder);
		}
	}
}

void ACGCBaseCharacter::ClimbLadderUp(float Value)
{
	if(!FMath::IsNearlyZero(Value) && GetBaseCharacterMovementComponent()->IsOnLadder())
	{
		FVector LadderUpVector = GetBaseCharacterMovementComponent()->GetCurrentLadder()->GetActorUpVector();
		AddMovementInput(LadderUpVector, Value);
	}
}

void ACGCBaseCharacter::InteractWithZipline()
{
	if(GetBaseCharacterMovementComponent()->IsOnZipline())
	{
		GetBaseCharacterMovementComponent()->DetachFromZipline();
	}
	else
	{
		const AZipline* AvailableZipline = GetAvailableZipline();
		if(IsValid(AvailableZipline) && !CGCBaseCharacterMovementComponent->IsSliding())
		{
			GetBaseCharacterMovementComponent()->AttachToZipline(AvailableZipline);
		}
	}
}

void ACGCBaseCharacter::OnStartProne(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	RecalculateBaseEyeHeight();

	const ACharacter* DefaultChar = GetDefault<ACharacter>(GetClass());
	if(GetMesh() && DefaultChar->GetMesh())
	{
		FVector& MeshRelativeLocation = GetMesh()->GetRelativeLocation_DirectMutable();
		MeshRelativeLocation.Z = DefaultChar->GetMesh()->GetRelativeLocation().Z + HalfHeightAdjust + CGCBaseCharacterMovementComponent->CrouchedHalfHeight;
		BaseTranslationOffset.Z = MeshRelativeLocation.Z;
	}
	else
	{
		BaseTranslationOffset.Z = DefaultChar->GetBaseTranslationOffset().Z + HalfHeightAdjust + CGCBaseCharacterMovementComponent->CrouchedHalfHeight;
	}

	K2_OnStartProne(HalfHeightAdjust, ScaledHalfHeightAdjust);
}

void ACGCBaseCharacter::OnEndProne(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	RecalculateBaseEyeHeight();

	const ACharacter* DefaultChar = GetDefault<ACharacter>(GetClass());
	const float HeightDifference = CGCBaseCharacterMovementComponent->bIsFullHeight ?
		0.0f : CGCBaseCharacterMovementComponent->CrouchedHalfHeight - CGCBaseCharacterMovementComponent->ProneCapsuleHalfHeight;
	if(GetMesh() && DefaultChar->GetMesh())
	{
		FVector& MeshRelativeLocation = GetMesh()->GetRelativeLocation_DirectMutable();
		MeshRelativeLocation.Z = DefaultChar->GetMesh()->GetRelativeLocation().Z + HeightDifference;
		BaseTranslationOffset.Z = MeshRelativeLocation.Z;
	}
	else
	{
		BaseTranslationOffset.Z = DefaultChar->GetBaseTranslationOffset().Z + HeightDifference;
	}

	K2_OnEndProne(HalfHeightAdjust, ScaledHalfHeightAdjust);
}

void ACGCBaseCharacter::OnStartSlide(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	RecalculateBaseEyeHeight();

	const ACharacter* DefaultChar = GetDefault<ACharacter>(GetClass());
	if(GetMesh() && DefaultChar->GetMesh())
	{
		FVector& MeshRelativeLocation = GetMesh()->GetRelativeLocation_DirectMutable();
		MeshRelativeLocation.Z = DefaultChar->GetMesh()->GetRelativeLocation().Z + HalfHeightAdjust + CGCBaseCharacterMovementComponent->SlideCapsuleHalfHeight;
		BaseTranslationOffset.Z = MeshRelativeLocation.Z;
	}
	else
	{
		BaseTranslationOffset.Z = DefaultChar->GetBaseTranslationOffset().Z + HalfHeightAdjust + CGCBaseCharacterMovementComponent->SlideCapsuleHalfHeight;
	}
}

void ACGCBaseCharacter::OnEndSlide(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	RecalculateBaseEyeHeight();

	const ACharacter* DefaultChar = GetDefault<ACharacter>(GetClass());
	const float HeightDifference = CGCBaseCharacterMovementComponent->bIsFullHeight ?
		0.0f : CGCBaseCharacterMovementComponent->CrouchedHalfHeight - CGCBaseCharacterMovementComponent->SlideCapsuleHalfHeight;
	if(GetMesh() && DefaultChar->GetMesh())
	{
		FVector& MeshRelativeLocation = GetMesh()->GetRelativeLocation_DirectMutable();
		MeshRelativeLocation.Z = DefaultChar->GetMesh()->GetRelativeLocation().Z + HalfHeightAdjust;
		BaseTranslationOffset.Z = MeshRelativeLocation.Z;
	}
	else
	{
		BaseTranslationOffset.Z = DefaultChar->GetBaseTranslationOffset().Z + HalfHeightAdjust;
	}
}

bool ACGCBaseCharacter::CanSprint()
{
	if(CGCBaseCharacterMovementComponent->Velocity.Size() == 0.0f || CGCBaseCharacterMovementComponent->IsOutOfStamina())
	{
		return false;
	}
	return true;
}

void ACGCBaseCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	TryChangeStateSprint(DeltaSeconds);
	CalculateOffsets(DeltaSeconds);
	TraceLineOfSight();
}

void ACGCBaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	CharacterAttributeComponent->OnDeathEvent.AddUObject(this, &ACGCBaseCharacter::OnDeath);
	CharacterAttributeComponent->OnStaminaIsOut.AddUObject(CGCBaseCharacterMovementComponent, &UGCBaseCharacterMovementComponent::SetIsOutOfStamina);
	InitializeHealthProgress();
}

bool ACGCBaseCharacter::CanJumpInternal_Implementation() const
{
	return Super::CanJumpInternal_Implementation() && !GetBaseCharacterMovementComponent()->IsMantling();
}

void ACGCBaseCharacter::OnSprintStart_Implementation()
{
	
}

void ACGCBaseCharacter::OnSprintEnd_Implementation()
{
	
}

void ACGCBaseCharacter::OnDeath()
{
	PlayAnimMontage(OnDeathAnimMontage);
	GetCharacterMovement()->DisableMovement();
}

void ACGCBaseCharacter::OnStartAimingInternal()
{
	if(AimStateChanged.IsBound())
	{
		AimStateChanged.Broadcast(true);
	}
}

void ACGCBaseCharacter::OnStopAimingInternal()
{
	if(AimStateChanged.IsBound())
	{
		AimStateChanged.Broadcast(false);
	}
}

void ACGCBaseCharacter::TraceLineOfSight()
{
	if(!IsPlayerControlled())
	{
		return;
	}

	FVector ViewLocation;
	FRotator ViewRotation;
	PlayerController->GetPlayerViewPoint(ViewLocation, ViewRotation);
	FVector ViewDirection = ViewRotation.Vector();
	FVector TraceEnd = ViewLocation + ViewDirection * LineDistance;
	FHitResult HitResult;
	GetWorld()->LineTraceSingleByChannel(HitResult, ViewLocation, TraceEnd, ECC_Visibility);
	if(LineObject.GetObject() != HitResult.Actor)
	{
		LineObject = HitResult.Actor.Get();

		FName ActionName;
		if(LineObject.GetInterface())
		{
			ActionName = LineObject->GetActionEventName();
		}
		else
		{
			ActionName = FName(NAME_None);
		}
		if(OnInteractableObjectFoundDelegate.IsBound())
		{
			OnInteractableObjectFoundDelegate.Execute(ActionName);
		}
	}
	
}

void ACGCBaseCharacter::UseInventory()
{
	if(!IsPlayerControlled())
	{
		return;
	}

	if(!CharacterInventoryComponent->IsWidgetVisible())
	{
		CharacterInventoryComponent->OpenWidget(PlayerController.Get());
		CharacterEquipmentComponent->OpenViewEquipment(PlayerController.Get());
		PlayerController->SetInputMode(FInputModeGameAndUI {});
		PlayerController->bShowMouseCursor = true;
		CharacterInventoryComponent->DebugItemsInArray();
	}
	else
	{
		CharacterInventoryComponent->CloseWidget();
		CharacterEquipmentComponent->CloseViewEquipment();
		PlayerController->SetInputMode(FInputModeGameOnly {});
		PlayerController->bShowMouseCursor = false;
	}
}

void ACGCBaseCharacter::ConfirmWeaponSelection()
{
	if(CharacterEquipmentComponent->IsSelectingWeapon())
	{
		CharacterEquipmentComponent->ConfirmWeaponSelection();
	}
}

void ACGCBaseCharacter::TryChangeStateSprint(float DeltaTime)
{
	if(bIsSprintRequested && !CGCBaseCharacterMovementComponent->GetIsSprinting() && CanSprint() && !CGCBaseCharacterMovementComponent->IsSliding())
	{
		CGCBaseCharacterMovementComponent->StartSprint();
		OnSprintStart();
	}
	if(!bIsSprintRequested)
	{
		CGCBaseCharacterMovementComponent->StopSprint();
		OnSprintEnd();
	}
}

void ACGCBaseCharacter::ChangeCrouchState()
{
	if(GetCharacterMovement()->IsCrouching())
	{
		UnCrouch();
	}
	if(!CGCBaseCharacterMovementComponent->GetIsSprinting() && !CGCBaseCharacterMovementComponent->IsSliding())
	{
		Crouch();
	}
}

void ACGCBaseCharacter::Slide(bool bForce)
{
	if(!(CanSlide() || bForce))
	{
		return;
	}
	
	if(OnChangingSlideState.IsBound())
	{
		OnChangingSlideState.Execute(true);
	}
	CGCBaseCharacterMovementComponent->StartSlide();
}

bool ACGCBaseCharacter::CanSlide()
{
	return CGCBaseCharacterMovementComponent->GetIsSprinting();
}

void ACGCBaseCharacter::RegisterInteractiveActor(AInteractiveActor* InteractiveActor)
{
	AvailableInteractiveActors.AddUnique(InteractiveActor);
}

void ACGCBaseCharacter::UnregisterInteractiveActor(AInteractiveActor* InteractiveActor)
{
	AvailableInteractiveActors.RemoveSingleSwap(InteractiveActor);
}

const ALadder* ACGCBaseCharacter::GetAvailableLadder() const
{
	const ALadder* Result = nullptr;
	for(const AInteractiveActor* InteractiveActor : AvailableInteractiveActors)
	{
		if(InteractiveActor->IsA<ALadder>())
		{
			Result = StaticCast<const ALadder*>(InteractiveActor);
			break;
		}
	}
	return Result;
}

const AZipline* ACGCBaseCharacter::GetAvailableZipline() const
{
	const AZipline* Result = nullptr;
	for(const AInteractiveActor* InteractiveActor : AvailableInteractiveActors)
	{
		if(InteractiveActor->IsA<AZipline>())
		{
			Result = StaticCast<const AZipline*>(InteractiveActor);
			break;
		}
	}
	return Result;
}

void ACGCBaseCharacter::SetIgnoreLookMoveInput(bool State)
{
	if(State)
	{
		PlayerController->SetIgnoreLookInput(true);
		PlayerController->SetIgnoreMoveInput(true);
	}
	else
	{
		PlayerController->SetIgnoreLookInput(false);
		PlayerController->SetIgnoreMoveInput(false);
	}
}

void ACGCBaseCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	PlayerController = Cast<ACGCPlayerController>(NewController);
	AAIController* AIController = Cast<AAIController>(NewController);
	if(IsValid(AIController))
	{
		FGenericTeamId GenericTeamId((uint8)Team);
		AIController->SetGenericTeamId(GenericTeamId);
	}
}

bool ACGCBaseCharacter::IsSwimingUnderWater() const
{
	if (!GetCharacterMovement()->IsSwimming())
	{
		return false;
	}
	
	FVector HeadLocation = GetMesh()->GetSocketLocation(FName("head"));
	APhysicsVolume* Volume = GetCharacterMovement()->GetPhysicsVolume();
	float VolumeTop = Volume->GetActorLocation().Z + Volume->GetBounds().BoxExtent.Z * Volume->GetActorScale3D().Z;
	
	return VolumeTop > HeadLocation.Z;
}

const UCharacterAttributeComponent* ACGCBaseCharacter::GetCharacterAttributesComponent() const
{
	return CharacterAttributeComponent;
}

UCharacterAttributeComponent* ACGCBaseCharacter::GetCharacterAttributeComponent_Mutable() const
{
	return CharacterAttributeComponent;
}

const UCharacterEquipmentComponent* ACGCBaseCharacter::GetCharacterEquipmentComponent() const
{
	return CharacterEquipmentComponent;
}

UCharacterEquipmentComponent* ACGCBaseCharacter::GetCharacterEquipmentComponent_Mutable() const
{
	return CharacterEquipmentComponent;
}

const UCharacterInventoryComponent* ACGCBaseCharacter::GetCharacterInventoryComponent() const
{
	return CharacterInventoryComponent;
}

UCharacterInventoryComponent* ACGCBaseCharacter::GetCharacterInventoryComponent_Mutable() const
{
	return CharacterInventoryComponent;
}

FGenericTeamId ACGCBaseCharacter::GetGenericTeamId() const
{
	return FGenericTeamId((uint8)Team);
}

void ACGCBaseCharacter::InitializeHealthProgress()
{
	UCGCAttributeProgressBar* Widget = Cast<UCGCAttributeProgressBar>(HealthbarProgressComponent->GetUserWidgetObject());
	if(!IsValid(Widget))
	{
		HealthbarProgressComponent->SetVisibility(false);
		return;
	}

	if(IsPlayerControlled() && IsLocallyControlled())
	{
		HealthbarProgressComponent->SetVisibility(false);
		return;
	}

	CharacterAttributeComponent->OnHealthChangedDelegate.AddUObject(Widget, &UCGCAttributeProgressBar::SetProgressPercantage);
	CharacterAttributeComponent->OnDeathEvent.AddLambda([=]() {HealthbarProgressComponent->SetVisibility(false);});
	Widget->SetProgressPercantage(CharacterAttributeComponent->GetHealthProcent());
}

void ACGCBaseCharacter::Client_ActivatePlatform_Implementation(APlatformTrigger* PlatformTrigger, bool bIsActivate)
{
	PlatformTrigger->SetIsActivated(bIsActivate);
}

void ACGCBaseCharacter::Server_ActivatePlatform_Implementation(class APlatformTrigger* PlatformTrigger, bool bIsActivate)
{
	PlatformTrigger->Multicast_SetIsActivated(bIsActivate);
}

bool ACGCBaseCharacter::CanMantle() const
{
	return !GetBaseCharacterMovementComponent()->IsOnLadder();
}

void ACGCBaseCharacter::ChangeStateProne()
{
	if(GetCharacterMovement()->IsCrouching() && !CGCBaseCharacterMovementComponent->IsProning())
	{
		Prone();
	}
	else if(!GetCharacterMovement()->IsCrouching() && CGCBaseCharacterMovementComponent->IsProning())
	{
		UnProne(false);
	}
}

void ACGCBaseCharacter::Mantle(bool bForce /*= false*/)
{
	if(!(CanMantle() || bForce))
	{
		return;
	}
	
	FLedgeDescription LedgeDescription;
	if(LedgeDetectorComponent->LedgeDetect(LedgeDescription) && !GetBaseCharacterMovementComponent()->IsMantling() && !CGCBaseCharacterMovementComponent->IsSliding())
	{
		ACharacter* DefaultCharacter = GetClass()->GetDefaultObject<ACharacter>();
		float DefaultCapsuleHalfHeight = DefaultCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		float CurrentCapsuleHalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		float CapsuleHalfHeightOffset = 0.0f;
		if(bIsCrouched)
		{
			UnCrouch();
			CapsuleHalfHeightOffset = DefaultCapsuleHalfHeight - CurrentCapsuleHalfHeight;
		}

		bIsMantling = true;
		
		FMantlingMovementParameters MantlingParameters;
		MantlingParameters.InitialLocation = GetActorLocation() + CapsuleHalfHeightOffset;
		MantlingParameters.InitialRotation = GetActorRotation();
		MantlingParameters.TargetLocation = LedgeDescription.Location;
		MantlingParameters.TargetRotation = LedgeDescription.Rotation;
		MantlingParameters.PrimitiveComponent = LedgeDescription.PrimitiveComponent;
		MantlingParameters.PrimitiveComponentInitialLocation = LedgeDescription.PrimitiveInitialLocation;
		float MantlingHeight = ((MantlingParameters.TargetLocation - DefaultCapsuleHalfHeight * FVector::UpVector) - (MantlingParameters.InitialLocation - DefaultCapsuleHalfHeight * FVector::UpVector)).Z;
		const FMantlingSettings& MantlingSettings = GetMantlingSettings(MantlingHeight);
		float MinRange;
		float MaxRange;
		MantlingSettings.MantlingCurve->GetTimeRange(MinRange, MaxRange);
		MantlingParameters.Duration = MaxRange - MinRange;
		
		MantlingParameters.MantlingCurve = MantlingSettings.MantlingCurve;
		FVector2D SourceRange(MantlingSettings.MinHeight, MantlingSettings.MaxHeight);
		FVector2D TargetRange(MantlingSettings.MinHeightStartTime, MantlingSettings.MaxHeightStartTime);
		MantlingParameters.StartTime = FMath::GetMappedRangeValueClamped(SourceRange, TargetRange, MantlingHeight);

		MantlingParameters.InitialAnimationLocation = MantlingParameters.TargetLocation - MantlingSettings.AnimationCorrectionZ * FVector::UpVector + MantlingSettings.AnimationCorrectionXY * LedgeDescription.LedgeNormal;

		if(IsLocallyControlled() || GetLocalRole() == ROLE_Authority)
		{
			GetBaseCharacterMovementComponent()->StartMantle(MantlingParameters);
		}
		
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		AnimInstance->Montage_Play(MantlingSettings.MantlingMontage, 1.0f, EMontagePlayReturnType::Duration, MantlingParameters.StartTime);
		OnMantle(MantlingSettings, MantlingParameters.StartTime);
	}
}

void ACGCBaseCharacter::OnRep_IsSliding(bool bIsSliding_old)
{
	if(GetLocalRole() == ROLE_SimulatedProxy)
	{
		if(!bIsSliding_old && bIsSliding)
		{
			Slide(true);
		}
	}
}

void ACGCBaseCharacter::OnRep_IsMantling(bool bIsMantling_old)
{
	if(GetLocalRole() == ROLE_SimulatedProxy)
	{
		if(!bIsMantling_old && bIsMantling)
		{
			Mantle(true);
		}
	}
}


void ACGCBaseCharacter::OnMantle(const FMantlingSettings& MantlingSettings, float MantlingAnimationStartTime)
{
}

float ACGCBaseCharacter::GetIKFootOffset(const FName& SocketName)
{
	float Result = 0.0f;

	CapsuleRadius = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	
	FVector SocketLocation = GetMesh()->GetSocketLocation(SocketName);
	FVector LineStartPosition(SocketLocation.X, SocketLocation.Y, GetActorLocation().Z);
	FVector LineEndPostion = LineStartPosition - (CapsuleRadius + IKTraceDistance) * FVector::UpVector;

	FHitResult HitResult;
	ETraceTypeQuery TraceType = UEngineTypes::ConvertToTraceType(ECC_Visibility);

	FVector BoxFoot(2.0f, 15.0f, 7.0f);
	if(UKismetSystemLibrary::BoxTraceSingle(GetWorld(), LineStartPosition, LineEndPostion, BoxFoot, GetMesh()->GetSocketRotation(SocketName), TraceType, true, TArray<AActor*>(), EDrawDebugTrace::None, HitResult, true))
	{
		Result = LineStartPosition.Z - CapsuleRadius - HitResult.Location.Z;
	}
	return Result;
}

float ACGCBaseCharacter::GetPelvisOffset()
{
	if(IKLeftFootOffset > MinimumOffset || IKRightFootOffset > MinimumOffset)
	{
		return -FMath::Max(IKLeftFootOffset ,IKRightFootOffset);
	}
	else
	{
		return 0.0f;
	}
}

void ACGCBaseCharacter::CalculateOffsets(float DeltaSeconds)
{
	IKLeftFootOffset = FMath::FInterpTo(IKLeftFootOffset, GetIKFootOffset(IKLeftFootSocketName), DeltaSeconds, IKInterpSpeed);
	IKRightFootOffset = FMath::FInterpTo(IKRightFootOffset, GetIKFootOffset(IKRightFootSocketName), DeltaSeconds, IKInterpSpeed);
	IKPelvisOffset = FMath::FInterpTo(IKPelvisOffset, GetPelvisOffset(), DeltaSeconds, IKInterpSpeed);
}

bool ACGCBaseCharacter::CanFire() const
{
	if(CharacterEquipmentComponent->GetIsEquipping())
	{
		return false;
	}
	if(CharacterEquipmentComponent->IsSelectingWeapon())
	{
		return false;
	}
	
	return true;
}

const FMantlingSettings& ACGCBaseCharacter::GetMantlingSettings(float LedgheHeight) const
{
	return LedgheHeight > LowMantleMaxHeight ? HighMantlingSettings : LowMantlingSettings;
}
