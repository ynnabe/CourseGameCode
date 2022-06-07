// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "Actors/Environment/PlatformTrigger.h"
#include "Actors/Interactive/Interface/Interactive.h"
#include "Components/CharacterComponents/CharacterEquipmentComponent.h"
#include "Components/CharacterComponents/CharacterInventoryComponent.h"
#include "CourseGameCode/Actors/Interactive/InteractiveActor.h"
#include "CourseGameCode/Components/CharacterComponents/CharacterAttributeComponent.h"
#include "GameFramework/Character.h"
#include "CGCBaseCharacter.generated.h"

DECLARE_DELEGATE_OneParam(FOnChangingSlideState, bool);
DECLARE_DELEGATE(FOnHardLanding);
DECLARE_MULTICAST_DELEGATE_OneParam(FAimStateChanged, bool);
DECLARE_DELEGATE_OneParam(FOnInteractableObjectFound, FName);

USTRUCT(BlueprintType)
struct FMantlingSettings
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UAnimMontage* MantlingMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UAnimMontage* FPMantlingMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UCurveVector* MantlingCurve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float AnimationCorrectionXY = 65.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float AnimationCorrectionZ = 200.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta= (ClampMin = 0.0f, UIMin = 0.0f))
	float MaxHeight = 200.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float MinHeight = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta= (ClampMin = 0.0f, UIMin = 0.0f))
	float MaxHeightStartTime = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float MinHeightStartTime = 0.5f;
};

class UGCBaseCharacterMovementComponent;
class UWidgetComponent;

UCLASS(Abstract, NotBlueprintable)
class COURSEGAMECODE_API ACGCBaseCharacter : public ACharacter, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	ACGCBaseCharacter(const FObjectInitializer& ObjectInitializer);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	FOnChangingSlideState OnChangingSlideState;
	FOnHardLanding OnHardLanding;
	FAimStateChanged AimStateChanged;
	FOnInteractableObjectFound OnInteractableObjectFoundDelegate;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void UseInventory();

	void ConfirmWeaponSelection();
	
	
	virtual void MoveForward(float Value) {};
	virtual void MoveRight(float Value) {};
	virtual void TurnUp(float Value) {};
	virtual void TurnRight(float Value) {};
	virtual void StartSprint();
	virtual void StopSprint();

	void StartFire();
	void StopFire();

	void Interact();

	void StartAiming();
	void StopAiming();

	FRotator GetAimOffset();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Character")
	void OnStartAiming();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Character")
	void OnStopAiming();
	
	float GetAimingMovementSpeed() const;

	bool IsAiming() const;

	void Reload();

	void NextItem();
	void PreviousItem();

	void EquipPrimaryItem();

	bool PickUp(TWeakObjectPtr<UInventoryItem> Item);

	void AttackPrimaryMelee();
	void AttackSecondaryMelee();

	void ChangeFireMode();
	void ChangeAmmoType();
	void ChangeShotType();

	virtual void Falling() override;
	virtual void Landed(const FHitResult& Hit) override;
	virtual void NotifyJumpApex() override;

	virtual void Prone();
	virtual void UnProne(bool bIsFullHeight);
	float GetPronePelvisOffset() {return PronePelvisOffset;}
	bool bIsProned = false;
	bool CanProne();
	void ChangeStateProne();

	UFUNCTION(BlueprintCallable)
	virtual void Mantle(bool bForce = false);

	UPROPERTY(ReplicatedUsing=OnRep_IsMantling)
	bool bIsMantling;

	UPROPERTY(ReplicatedUsing=OnRep_IsSliding)
	bool bIsSliding;

	UFUNCTION()
	void OnRep_IsSliding(bool bIsSliding_old);

	UFUNCTION()
	void OnRep_IsMantling(bool bIsMantling_old);

	virtual void OnMantle(const FMantlingSettings& MantlingSettings, float MantlingAnimationStartTime);

	virtual void Jump() override;

	virtual void SwimForward(float Value) {};
	virtual void SwimRight(float Value) {};
	virtual void SwimUp(float Value) {};

	virtual void InteractWithLadder();
	virtual void ClimbLadderUp(float Value);

	virtual void InteractWithZipline();

	virtual void OnStartProne(float HalfHeightAdjust, float ScaledHalfHeightAdjust);
	virtual void OnEndProne(float HalfHeightAdjust, float ScaledHalfHeightAdjust);

	virtual void OnStartSlide(float HalfHeightAdjust, float ScaledHalfHeightAdjust);
	virtual void OnEndSlide(float HalfHeightAdjust, float ScaledHalfHeightAdjust);

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="OnStartProne", ScriptName="OnStartProne"))
	void K2_OnStartProne(float HalfHeightAdjust, float ScaledHalfHeightAdjust);

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="OnEndProne", ScriptName="OnEndProne"))
	void K2_OnEndProne(float HalfHeightAdjust, float ScaledHalfHeightAdjust);

	virtual bool CanSprint();
	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;

	virtual bool CanJumpInternal_Implementation() const override;

	FORCEINLINE UGCBaseCharacterMovementComponent* GetBaseCharacterMovementComponent() const {return CGCBaseCharacterMovementComponent;}

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE float GetIKLeftFootOffset() const {return IKLeftFootOffset;}

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE float GetIKRightFootOffset() const {return IKRightFootOffset;}

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE float GetIKPelvisOffset() const {return IKPelvisOffset;}

	float IKPelvisOffset = 0.0f;

	void ChangeCrouchState();
	void Slide(bool bForce = false);

	bool CanSlide();

	void RegisterInteractiveActor(AInteractiveActor* InteractiveActor);
	void UnregisterInteractiveActor(AInteractiveActor* InteractiveActor);
	const class ALadder* GetAvailableLadder() const;
	const class AZipline* GetAvailableZipline() const;

	void SetIgnoreLookMoveInput(bool State);

	virtual void PossessedBy(AController* NewController) override;

	bool IsSwimingUnderWater() const;

	const UCharacterAttributeComponent* GetCharacterAttributesComponent() const;

	UCharacterAttributeComponent* GetCharacterAttributeComponent_Mutable() const;

	const UCharacterEquipmentComponent* GetCharacterEquipmentComponent() const;

	UCharacterEquipmentComponent* GetCharacterEquipmentComponent_Mutable() const;

	const UCharacterInventoryComponent* GetCharacterInventoryComponent() const;

	UCharacterInventoryComponent* GetCharacterInventoryComponent_Mutable() const;

	virtual FGenericTeamId GetGenericTeamId() const override;

	UPROPERTY(VisibleDefaultsOnly, Category="Character | Components")
	UWidgetComponent* HealthbarProgressComponent;

	void InitializeHealthProgress();

	UFUNCTION(Server, Reliable)
	void Server_ActivatePlatform(class APlatformTrigger* PlatformTrigger, bool bIsActivate);

	UFUNCTION(Client, Reliable)
	void Client_ActivatePlatform(class APlatformTrigger* PlatformTrigger, bool bIsActivate);

protected:
	UGCBaseCharacterMovementComponent* CGCBaseCharacterMovementComponent;

	bool CanMantle() const;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Character | Movement")
	class ULedgeDetectorComponent* LedgeDetectorComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Character | Movement")
	class UCharacterAttributeComponent* CharacterAttributeComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Character | Inventory")
	UCharacterInventoryComponent* CharacterInventoryComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Character | Movement | Mantling")
	FMantlingSettings HighMantlingSettings;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Character | Movement | Mantling")
	FMantlingSettings LowMantlingSettings;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Character | Movement | Mantling")
	float LowMantleMaxHeight = 125.0f;

	UFUNCTION(BlueprintNativeEvent, Category = "Character movement: sprint")
	void OnSprintStart();
	virtual void OnSprintStart_Implementation();

	UFUNCTION(BlueprintNativeEvent, Category= "Character movement: sprint")
	void OnSprintEnd();
	virtual void OnSprintEnd_Implementation();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character | IK System")
	FName IKLeftFootSocketName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character | IK System")
	FName IKRightFootSocketName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character | IK System")
	float IKInterpSpeed = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character | IK System")
	float IKExtendedDistance = 60.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character | Falling")
	float MaxFallingHeight = 1000.0f;
	
	virtual void OnDeath();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Animations")
	class UAnimMontage* OnDeathAnimMontage;

	// Damage depending from fall height (in meters)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Character | Attributes")
	class UCurveFloat* FallDamageCurve;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Character | Component")
	class UCharacterEquipmentComponent* CharacterEquipmentComponent;

	virtual void OnStartAimingInternal();
	virtual void OnStopAimingInternal();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character | Team")
	ETeams Team = ETeams::Enemy;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Line of sight")
	float LineDistance = 500.0f;

	void TraceLineOfSight();

	UPROPERTY()
	TScriptInterface<IInteractable> LineObject;
	

private:
	bool bIsSprintRequested = false;
	void TryChangeStateSprint(float DeltaTime);

	float GetIKFootOffset(const FName& SocketName);
	float GetPelvisOffset();
	void CalculateOffsets(float DeltaSeconds);
	
	float IKLeftFootOffset = 0.0f;
	float IKRightFootOffset = 0.0f;
	float IKTraceDistance = 50.0f;
	float IKScale = 0.0f;
	float CapsuleRadius = 0.0f;
	float MinimumOffset = 3.0f;
	float PronePelvisOffset = -50.0f;

	float CurrentFallingHeight = 0.0f;

	bool CanFire() const;

	float CurrentAimingMovementSpeed;

	bool bIsAiming = false;

	const FMantlingSettings& GetMantlingSettings(float LedgheHeight) const;

	TArray<AInteractiveActor*> AvailableInteractiveActors;
	
	TWeakObjectPtr<class ACGCPlayerController> PlayerController;
	
};
