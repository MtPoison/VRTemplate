// Fill out your copyright notice in the Description page of Project Settings.


#include "GrabComponent.h"

#include "ComponentUtils.h"

// Sets default values for this component's properties
UGrabComponent::UGrabComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

bool UGrabComponent::TryGrab(UMotionControllerComponent* MotionControllerValue)
{
	AActor* ParentActor = GetOwner();
	USceneComponent* AttachParent = GetOwner()->GetRootComponent()->GetAttachParent();
	switch (GrabType)
	{
		case EGrabType::None:
			break;
	        
		case EGrabType::Free:
			
			SetPrimitiveCompPhysics(false);

			ParentActor = GetOwner();
			if (ParentActor && MotionController)
			{
				FAttachmentTransformRules AttachRules(EAttachmentRule::KeepWorld, true);
				bool bAttached = ParentActor->AttachToComponent(MotionController, AttachRules);

				if (bAttached)
				{
					bIsHeld = true;
				}
			}
		
			break;
	        
		case EGrabType::Snap:

			SetPrimitiveCompPhysics(false);

			ParentActor = GetOwner();
			if (ParentActor && MotionController)
			{
				FAttachmentTransformRules AttachRules(EAttachmentRule::KeepWorld, true);
				bool bAttached = ParentActor->AttachToComponent(MotionController, AttachRules);

				if (bAttached)
				{
					bIsHeld = true;
					AttachParent->SetRelativeRotation(RelativeRotation.GetInverse(),false, nullptr, ETeleportType::TeleportPhysics);
					FVector MotionControllerLocation = MotionController->GetComponentLocation();
					FVector SelfLocation = GetOwner()->GetRootComponent()->GetComponentLocation();

					AttachParent = GetOwner()->GetRootComponent()->GetAttachParent();
					FVector AttachParentLocation = AttachParent->GetComponentLocation();

					FVector Offset = (SelfLocation - AttachParentLocation) * -1.f;

					FVector NewLocation = MotionControllerLocation + AttachParentLocation + Offset;

					AttachParent->SetWorldLocation(NewLocation, false, nullptr, ETeleportType::TeleportPhysics);
				}
			}
			break;
	        
		case EGrabType::Custom:
			bIsHeld = true;
			break;
	        
		default:
			break;
	}
	if (bIsHeld)
	{
		MotionController = MotionControllerValue;
		OnGrabbed.Broadcast();
		APlayerController* PC = Cast<APlayerController>(GetOwner());
		PC->PlayHapticEffect(TeleportTraceNiagaraSystem, GetHeldByHand(), 1.0, false);
		return true;
	}
	return false;
}

void UGrabComponent::SetPrimitiveCompPhysics(bool Simulate)
{
	USceneComponent* AttachParent = GetOwner()->GetRootComponent()->GetAttachParent();
    
	UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(AttachParent);
	if (PrimComp)
	{
		PrimComp->SetSimulatePhysics(Simulate);
	}
}

bool UGrabComponent::TryRelease()
{
	switch (GrabType)
	{
		case EGrabType::None:
			break;
	        
		case EGrabType::Free:
			SimulateOnDrop();
			break;
	        
		case EGrabType::Snap:
			SimulateOnDrop();
			break;
	        
		case EGrabType::Custom:
			bIsHeld = true;
			break;
	        
		default:
			break;
	}
	if (bIsHeld)
	{
		return false;
	}
	OnDropped.Broadcast();
	return true;
}


// Called when the game starts
void UGrabComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	SetShouldSimulateOnDrop();
	USceneComponent* AttachParent = GetOwner()->GetRootComponent()->GetAttachParent();
    
	UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(AttachParent);
	if (PrimComp)
	{
		PrimComp->SetCollisionProfileName(FName("PhysicsActor"), true);
	}
}

void UGrabComponent::SetShouldSimulateOnDrop()
{
	USceneComponent* AttachParent = GetOwner()->GetRootComponent()->GetAttachParent();
    
	UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(AttachParent);
	if (PrimComp)
	{
		if (PrimComp->IsAnySimulatingPhysics())
		{
			bSimulateOnDrop = true;
		}
	}
}

EControllerHand UGrabComponent::GetHeldByHand()
{
	if (MotionController->GetTrackingMotionSource().ToString().Contains("Left"))
	{
		return EControllerHand::Left;
	}
	return EControllerHand::Right;
}

void UGrabComponent::SimulateOnDrop()
{
	USceneComponent* AttachParent = GetOwner()->GetRootComponent()->GetAttachParent();
	
	if (bSimulateOnDrop)
	{
		SetPrimitiveCompPhysics(true);
	}

	AttachParent->DetachFromComponent( FDetachmentTransformRules::KeepWorldTransform);
	bIsHeld = false;
}

