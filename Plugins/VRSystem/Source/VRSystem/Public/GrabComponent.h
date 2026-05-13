// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "NiagaraComponent.h"

#include "CoreMinimal.h"
#include "MotionControllerComponent.h"
#include "Components/ActorComponent.h"
#include "GrabComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGrabbed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDropped);
UENUM(BlueprintType)
enum class EGrabType : uint8
{
	None,
	Free,
	Snap,
	Custom,
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VRSYSTEM_API UGrabComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	// Sets default values for this component's properties
	UGrabComponent();

	bool TryGrab(UMotionControllerComponent* MotionController);

	void SetPrimitiveCompPhysics(bool Simulate);

	bool TryRelease();
	
private :
	
	void SetShouldSimulateOnDrop();

	EControllerHand GetHeldByHand();

	void SimulateOnDrop();
	
protected:
	
	// Called when the game starts
	virtual void BeginPlay() override;

private:	
	
	bool bSimulateOnDrop;

	bool bIsHeld;
	
	EGrabType GrabType = EGrabType::Free;

	FRotator RelativeRotation;
	
	UPROPERTY(VisibleAnywhere, Category = "Motion Controller")
	TObjectPtr<UMotionControllerComponent> MotionController;

	UPROPERTY(VisibleAnywhere, Category = "Niagara")
	TObjectPtr<UHapticFeedbackEffect_Base> TeleportTraceNiagaraSystem;
	
protected:
	
	FOnGrabbed OnGrabbed;
	
	FOnGrabbed OnDropped;
	
};
