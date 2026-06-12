// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputSubsystemInterface.h"
#include "GameFramework/Actor.h"
#include "InteractInterface.h"
#include "VRHand.generated.h"

class UMotionControllerComponent;
class UWidgetInteractionComponent;
class USphereComponent;

class UHandVR;

UCLASS()
class VRSYSTEM_API AVRHand : public AActor
{
	GENERATED_BODY()
	
public:	
	AVRHand();

protected:
	
	virtual void BeginPlay() override;
	
	virtual void OnConstruction(const FTransform& Transform) override;

	UPROPERTY()
	TObjectPtr<APlayerController> PlayerController;
	

#pragma region GrabSystem
	
	UFUNCTION(BlueprintCallable)
	void GrabOject();
	UFUNCTION(BlueprintCallable)
	void ReleaseOject();

#pragma endregion
	
#pragma region Components
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UMotionControllerComponent> MotionController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components|Hands")
	TObjectPtr<USkeletalMeshComponent> HandMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components|Hands")
	TObjectPtr<UWidgetInteractionComponent> WidgetInteractionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components|Hands")
	TObjectPtr<USphereComponent> GrabSphere;

#pragma endregion

#pragma region HandData

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components|Hands|HandData")
	EControllerHand HandType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components|Hands|HandData")
	bool bMirrorAnimation;

	UPROPERTY()
	TObjectPtr<UHandVR> HandVR;

	/*UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components|Hands|HandData")
	TSubclassOf<UHandVR> SubHandVR;*/

	
#pragma endregion

#pragma region Input

	void MappingInput();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VR|Input")
	TObjectPtr<UInputAction> IA_Hand_Grasp_Left;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VR|Input")
	TObjectPtr<UInputAction> IA_Hand_IndexCurl_Left;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VR|Input")
	TObjectPtr<UInputAction> IA_Hand_Point_Left;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VR|Input")
	TObjectPtr<UInputAction> IA_Hand_ThumpUp_Left;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VR|Input")
	TObjectPtr<UInputAction> IA_Hand_Grasp_Right;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VR|Input")
	TObjectPtr<UInputAction> IA_Hand_IndexCurl_Right;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VR|Input")
	TObjectPtr<UInputAction> IA_Hand_Point_Right;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VR|Input")
	TObjectPtr<UInputAction> IA_Hand_ThumpUp_Right;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VR|Input")
	TObjectPtr<UInputAction> IA_Grab_Left_Pressed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VR|Input")
	TObjectPtr<UInputAction> IA_Grab_Left_Released;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VR|Input")
	TObjectPtr<UInputAction> IA_Grab_Right_Pressed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VR|Input")
	TObjectPtr<UInputAction> IA_Grab_Right_Released;

#pragma endregion

#pragma region InitializeABP

	UFUNCTION()
	void TriggerPoseAlphaGrasp(const FInputActionValue& Value);

	void PoseAlphaGrasp();

	UFUNCTION()
	void TriggerPoseAlphaIndexCurl(const FInputActionValue& Value);

	void PoseAlphaIndexCurl();

	UFUNCTION()
	void CompletedPoseAlphaPoint(const FInputActionValue& Value);

	void PoseAlphaPoint();

	UFUNCTION()
	void CompletedPoseAlphaThumbUp(const FInputActionValue& Value);

	void PoseAlphaThumbUp();


#pragma endregion

	
	
private:
	
	TScriptInterface<IInteractInterface> CurrentlyGrabbedActor;
};
