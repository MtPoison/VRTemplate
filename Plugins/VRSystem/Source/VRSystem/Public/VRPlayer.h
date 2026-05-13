// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GrabComponent.h"
#include "GameFramework/Pawn.h"
#include "HeadMountedDisplayTypes.h"
#include "InputAction.h"
#include "MannequinXR.h"
#include "PawnAnimInterface.h"
#include "TeleportVisualizer.h"
#include "VRPlayer.generated.h"

class USceneComponent;
class UCameraComponent;
class UStaticMeshComponent;
class UMotionControllerComponent;
class UWidgetInteractionComponent;
class USkeletalMeshComponent;
class UNiagaraComponent;
class ATeleportVisualizer;
class UVRNotificationsComponent;

UCLASS()
class VRSYSTEM_API AVRPlayer : public APawn, public IPawnAnimInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AVRPlayer();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

               
private:
	
	void SetupInputMappingContexts();

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	float CalculateSnapTurnAngle(bool bRightTurn);

	void OnTurn(const FInputActionValue& Value);

	void TeleportTrace();

	void StartTeleportTrace();
	
	void EndTeleportTrace();

	void TryTeleport();

	UGrabComponent* GetGrabComponentNearMotionController(UMotionControllerComponent* MontionControllerValue);
	
	void GrabLeftPressed();

	void GrabLeftReleassed();

	void GrabRightPressed();

	void GrabRightReleassed();

	void OnHandGraspLeft_Trigger(const FInputActionValue& Value);
	void OnHandGraspLeft();

	void OnHandIndexCurlLeft_Trigger(const FInputActionValue& Value);
	void OnHandIndexCurlLeft();

	void  OnHandPointLeft(const FInputActionValue& Value); 
	void  OnHandPointLeft_Completed(); 

	void HandThumbUpLeft(const FInputActionValue& Value);
	void HandThumbUpLeft_Completed();

	void OnHandGraspRight_Trigger(const FInputActionValue& Value);
	void OnHandGraspRight();

	void OnHandIndexCurlRight_Trigger(const FInputActionValue& Value);
	void OnHandIndexCurlRight();

	void  OnHandPointRight(const FInputActionValue& Value); 
	void  OnHandPointRight_Completed();  

	void HandThumbUpRight(const FInputActionValue& Value);
	void HandThumbUpRight_Completed();

	virtual void HideUnhideHand_Implementation(bool bHide, bool bRightHand);
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

	void SnapTurn(bool RightTurn);

private:
	
	UPROPERTY(EditAnywhere, Category = "VR")
	TEnumAsByte<EHMDTrackingOrigin::Type> TrackingOrigin = EHMDTrackingOrigin::LocalFloor;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> IMC_Default;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> IMC_Hands;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> IA_Turn;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> IA_Move;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> IA_Grab_Left_Pressed;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> IA_Grab_Left_Relassed;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction>  IA_Grab_Right_Pressed;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> IA_Grab_Right_Relassed;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> IA_Menu_Toggle_Left;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> IA_Hand_Grasp_Left;
	
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> IA_Hand_IndexCurl_Left;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> IA_Hand_Point_Left;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> IA_Hand_ThumbUp_Left;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> IA_Hand_Grasp_Right;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> IA_Hand_IndexCurl_Right;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> IA_Hand_Point_Right;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> IA_Hand_ThumbUp_Right;
	
	// Root VR
	UPROPERTY(VisibleAnywhere, Category = "VR")
    TObjectPtr<USceneComponent> VROrigin;

    // Camera
    UPROPERTY(VisibleAnywhere, Category = "VR")
    TObjectPtr<UCameraComponent> Camera;

    UPROPERTY(VisibleAnywhere, Category = "VR")
    TObjectPtr<UStaticMeshComponent> HeadMountedDisplayMesh;

    // Left Aim
    UPROPERTY(VisibleAnywhere, Category = "VR Left")
    TObjectPtr<UMotionControllerComponent> MotionControllerLeftAim;

    UPROPERTY(VisibleAnywhere, Category = "VR Left")
    TObjectPtr<UWidgetInteractionComponent> WidgetInteractionLeft;

    // Right Aim
    UPROPERTY(VisibleAnywhere, Category = "VR Right")
    TObjectPtr<UMotionControllerComponent> MotionControllerRightAim;

    // Left Grip
    UPROPERTY(VisibleAnywhere, Category = "VR Left")
    TObjectPtr<UMotionControllerComponent> MotionControllerLeftGrip;

    UPROPERTY(VisibleAnywhere, Category = "VR Left")
    TObjectPtr<UMannequinXR> HandLeft;

    UPROPERTY(VisibleAnywhere, Category = "VR Left")
    TObjectPtr<UStaticMeshComponent> XRDeviceVisualizationLeft;

    UPROPERTY(VisibleAnywhere, Category = "VR Right")
    TObjectPtr<UWidgetInteractionComponent> WidgetInteractionRight;

    // Right Grip
    UPROPERTY(VisibleAnywhere, Category = "VR Right")
    TObjectPtr<UMotionControllerComponent> MotionControllerRightGrip;

    UPROPERTY(VisibleAnywhere, Category = "VR Right")
    TObjectPtr<UMannequinXR> HandRight;

    UPROPERTY(VisibleAnywhere, Category = "VR Right")
    TObjectPtr<UStaticMeshComponent> XRDeviceVisualizationRight;
	
    // Teleport
	
    UPROPERTY(VisibleAnywhere, Category = "VR")
    TObjectPtr<UNiagaraComponent> TeleportTraceNiagaraSystem;

    // Notifications
    UPROPERTY(VisibleAnywhere, Category = "VR")
    TObjectPtr<UVRNotificationsComponent> VRNotifications;

	UPROPERTY(EditDefaultsOnly, Category = "VR Locomotion")
	float SnapTurnDegrees = 45.f;
	
	UPROPERTY(EditAnywhere, Category = "TeleportTrace")
	float LocalTeleportLaunchSpeed = 600.f;

	UPROPERTY(EditAnywhere, Category = "TeleportTrace")
	float LocalTeleportProjectileRadius = 3.6f;

	UPROPERTY(EditAnywhere, Category = "TeleportTrace")
	float LocalNavMeshCellHeight = 8.0f;
	
	FVector ProjectedTeleportLocation;
	
	TArray<FVector> TeleportTracePathPositions;

	bool bValidTeleportLocation;

	bool bTeleportTraceActive;

	UPROPERTY()
	ATeleportVisualizer* TeleportVisualizer;

	float GrabRadiusFromGripPosition = 6.0f;

	UPROPERTY()
	TObjectPtr<UGrabComponent> HeldComponentLeft;

	UPROPERTY()
	TObjectPtr<UGrabComponent> HeldComponentRight;

	UPROPERTY()
	TObjectPtr<UMotionControllerComponent> HeldComponentTop;
};
