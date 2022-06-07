// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractiveCameraActor.h"

#include <ThirdParty/openexr/Deploy/OpenEXR-2.3.0/OpenEXR/include/ImathBox.h>

#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

AInteractiveCameraActor::AInteractiveCameraActor()
{
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Camera interaction volume"));
	BoxComponent->SetBoxExtent(FVector(500.0f, 500.0f, 500.0f));
	BoxComponent->SetCollisionObjectType(ECC_WorldDynamic);
	BoxComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	BoxComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	BoxComponent->SetupAttachment(RootComponent);
}

void AInteractiveCameraActor::BeginPlay()
{
	Super::BeginPlay();

	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AInteractiveCameraActor::OnBeginOverlap);
	BoxComponent->OnComponentEndOverlap.AddDynamic(this, &AInteractiveCameraActor::OnEndOverlap);
	
}

void AInteractiveCameraActor::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	FViewTargetTransitionParams TransitionCameraParams;
	TransitionCameraParams.BlendTime = TransitionToCameraTime;
	PlayerController->SetViewTarget(this, TransitionCameraParams);
}

void AInteractiveCameraActor::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	APawn* Pawn = PlayerController->GetPawn();
	FViewTargetTransitionParams TransitionPawnParams;
	TransitionPawnParams.BlendTime = TransitionToPawnTime;
	TransitionPawnParams.bLockOutgoing = true;
	PlayerController->SetViewTarget(Pawn, TransitionPawnParams);
}
