// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GrabComponent.h"
#include "GameFramework/Character.h"
#include "IHeadMountedDisplay.h"
#include "InputAction.h"
#include "VRHand.h"
#include "Components/SplineComponent.h"
#include "VRChracter.generated.h"

class UCameraComponent;

UCLASS()
class VRSYSTEM_API AVRChracter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AVRChracter();
	virtual void Tick(float DeltaTime) override;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	UPROPERTY(EditAnywhere, Category = "VR")
	TEnumAsByte<EHMDTrackingOrigin::Type> TrackingOrigin = EHMDTrackingOrigin::LocalFloor;

#pragma region Components
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCameraComponent> Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> Origin;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USplineComponent> TeleportSpline;
	
#pragma endregion

#pragma region Input

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VR|Input")
	TObjectPtr<UInputMappingContext> IMC_Default;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VR|Input")
	TObjectPtr<UInputMappingContext> IMC_Hands;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VR|Input")
	TObjectPtr<UInputAction> IA_Turn;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VR|Input")
	TObjectPtr<UInputAction> IA_Teleport;

	void SetupInputMappingContexts();

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
#pragma endregion

#pragma region Turn
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR|Rotation")
	float SnapTurnAngle = 45.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR|Rotation")
	float JoystickDeadZone = 0.5f;
	bool bSnapTurnTriggered = false;

	UFUNCTION(BlueprintCallable)
	void Turn(const FInputActionValue& Value);
	void SnapTurnAroundCamera(float Direction);
	
#pragma endregion 

#pragma region Teleportation

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR|Teleport")
	TObjectPtr<UNiagaraSystem> NS_TeleportArc;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR|Teleport")
	TObjectPtr<UNiagaraSystem> NS_TeleportVisualizer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR|Teleport")
	float TeleportLaunchSpeed = 1200.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR|Teleport")
	float NavMeshCellHeight = 4.f;

	bool bTeleportActive = false;
	bool bValidTeleportLocation = false;
	FVector TeleportDestination = FVector::ZeroVector;
	TArray<FVector> TeleportPathPositions;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR|Teleport")
	TObjectPtr<UNiagaraComponent> NC_TeleportArc = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR|Teleport")
	TObjectPtr<UNiagaraComponent> NC_TeleportVisualizer = nullptr;
	
	void OnTeleportStarted(const FInputActionValue& Value);
	void OnTeleportTriggered(const FInputActionValue& Value);
	void OnTeleportCompleted(const FInputActionValue& Value);
	void UpdateTeleportArc(); 
	void ShowTeleport(bool bShow);  
	bool TraceTeleportDestination(FVector& OutLocation);
	void ExecuteTeleport();

	
#pragma endregion
	
#pragma region Hand

	UPROPERTY(EditDefaultsOnly, Category = "VR|Hand")
	TSubclassOf<AVRHand> HandClassLeft;

	UPROPERTY(EditDefaultsOnly, Category = "VR|Hand")
	TSubclassOf<AVRHand> HandClassRight;
	
	UPROPERTY()
	AVRHand* HandLeft;

	UPROPERTY()
	AVRHand* HandRight;

#pragma endregion

};

