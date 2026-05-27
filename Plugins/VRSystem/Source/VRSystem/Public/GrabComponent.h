// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "NiagaraComponent.h"

#include "CoreMinimal.h"
#include "InteractInterface.h"
#include "MotionControllerComponent.h"
#include "Components/ActorComponent.h"

#include "GrabComponent.generated.h"

UENUM(BlueprintType)
enum class EGrabType : uint8
{
	None UMETA(DisplayName = "None"),
	Free UMETA(DisplayName = "Free"),
	Snap UMETA(DisplayName = "Snap"),
};

class UBoxComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VRSYSTEM_API UGrabComponent : public UActorComponent, public IInteractInterface
{
	GENERATED_BODY()

public:
	UGrabComponent();
	
protected:
	
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void OnGrab(AActor* InComponentActor = nullptr, const FVector& Grabbable = FVector::ZeroVector);
	virtual void OnRelease(AActor* InComponentActor = nullptr, const FVector& Grabbable = FVector::ZeroVector);
	
	
#pragma region Components

	UPROPERTY()
	TObjectPtr<UBoxComponent> GrabRegion;

#pragma endregion
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setup")
	EGrabType GrabType= EGrabType::None;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Setup")
	bool bIsHeld;

	
private:
	UPROPERTY()
	AActor* GrabbedActor;
};
