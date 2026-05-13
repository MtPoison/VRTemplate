// Fill out your copyright notice in the Description page of Project Settings.


#include "TeleportVisualizer.h"

#include "HeadMountedDisplayFunctionLibrary.h"

// Sets default values
ATeleportVisualizer::ATeleportVisualizer()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	PlayerAreaBounds = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NS_PlayAreaBounds"));
	PlayerAreaBounds->SetupAttachment(RootComponent);

	TeleportingRing = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NS_TeleportRing"));
	TeleportingRing->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ATeleportVisualizer::BeginPlay()
{
	Super::BeginPlay();
	
	FVector2D Bounds;
	Bounds = UHeadMountedDisplayFunctionLibrary::GetPlayAreaBounds(EHMDTrackingOrigin::Stage);

	if (PlayerAreaBounds)
	{
		PlayerAreaBounds->SetVariableVec3(FName("User.PlayAreaBounds"), FVector(Bounds.X, Bounds.Y, 0));
	}
}

// Called every frame
void ATeleportVisualizer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector LocactionPlayer = PlayerPawn->GetActorTransform().InverseTransformPosition(PlayerCameraManager->GetActorTransform().GetLocation());
	PlayerAreaBounds->SetRelativeLocation(FVector(-LocactionPlayer.X, -LocactionPlayer.Y, 0));
	SetActorRotation(PlayerPawn->GetActorRotation());
}

