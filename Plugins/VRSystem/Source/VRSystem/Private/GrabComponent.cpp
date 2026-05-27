// Fill out your copyright notice in the Description page of Project Settings.


#include "GrabComponent.h"
#include "Components/BoxComponent.h"
#include "ComponentUtils.h"

// Sets default values for this component's properties
UGrabComponent::UGrabComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	
}

void UGrabComponent::BeginPlay()
{
	Super::BeginPlay();
	TArray<UBoxComponent*> Boxes;
	GetOwner()->GetComponents<UBoxComponent>(Boxes);

	for (UBoxComponent* Box : Boxes)
	{
		// Debug pour voir les noms exacts
		UE_LOG(LogTemp, Warning, TEXT("Box trouvé : %s"), *Box->GetName());
        
		if (Box->GetName() == "GrabRegion")
		{
			GrabRegion = Box;
			break;
		}
	}

	if (GrabRegion)
	{
		GrabRegion->SetCollisionProfileName("Grabbable");
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("GrabComponent sur %s : aucun BoxComponent 'GrabRegion' trouvé !"), *GetOwner()->GetName());
	}
}

void UGrabComponent::OnGrab(AActor* InComponentActor, const FVector& Grabbable)
{
	if (InComponentActor)
	{
		switch (GrabType)
		{
		case EGrabType::Free:
			{
				UPrimitiveComponent* Root = Cast<UPrimitiveComponent>(GetOwner()->GetRootComponent());
				if (Root)
				{
					Root->SetSimulatePhysics(false);
				}
				bIsHeld = GetOwner()->AttachToActor(InComponentActor, FAttachmentTransformRules::KeepWorldTransform);
				if (bIsHeld)
				{
					GrabbedActor = InComponentActor;
				}
				break;
				
			}
		case EGrabType::Snap:
			{
				break;
			}
		default:
			break;
		}
	}
}

void UGrabComponent::OnRelease(AActor* InComponentActor, const FVector& Grabbable)
{
	if (InComponentActor)
	{
		switch (GrabType)
		{
		case EGrabType::Free:
			{
				if (bIsHeld)
				{
					if (InComponentActor == GrabbedActor)
					{
						UPrimitiveComponent* Root = Cast<UPrimitiveComponent>(GetOwner()->GetRootComponent());

						if (Root)
						{
							Root->SetSimulatePhysics(true);
							bIsHeld = false;
						}
					}
					
				}
			}
			break;
		case EGrabType::Snap:
		
			break;
		default:
			
			break;
		}
	}
}


