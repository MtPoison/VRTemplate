// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractInterface.h"
#include "VRHand.generated.h"

class UMotionControllerComponent;
class UWidgetInteractionComponent;
class USphereComponent;
UCLASS()
class VRSYSTEM_API AVRHand : public AActor
{
	GENERATED_BODY()
	
public:	
	AVRHand();

protected:
	
	virtual void BeginPlay() override;
	
	virtual void OnConstruction(const FTransform& Transform) override;

	UFUNCTION(BlueprintCallable)
	void GrabOject();
	UFUNCTION(BlueprintCallable)
	void ReleaseOject();
	
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

	// Defines Which Hand It Is
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components|Hands|HandData")
	EControllerHand HandType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components|Hands|HandData")
	bool bMirrorAnimation;

#pragma endregion
	virtual void Tick(float DeltaTime) override;
private:	
TScriptInterface<IInteractInterface> CurrentlyGrabbedActor;
};
