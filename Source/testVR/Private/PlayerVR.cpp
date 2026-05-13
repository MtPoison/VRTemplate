// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerVR.h"
#include "HeadMountedDisplayFunctionLibrary.h"

// Sets default values
APlayerVR::APlayerVR()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APlayerVR::BeginPlay()
{
	Super::BeginPlay();
	UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled();
	
}

// Called every frame
void APlayerVR::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APlayerVR::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

