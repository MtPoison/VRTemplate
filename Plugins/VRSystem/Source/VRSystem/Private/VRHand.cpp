// Fill out your copyright notice in the Description page of Project Settings.


#include "VRHand.h"
#include "MotionControllerComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetInteractionComponent.h"
// Sets default values
AVRHand::AVRHand()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	MotionController = CreateDefaultSubobject<UMotionControllerComponent>("MotionController");
	SetRootComponent(MotionController);

	HandMesh = CreateDefaultSubobject<USkeletalMeshComponent>("SkeletalMeshComponent");
	HandMesh->SetupAttachment(MotionController);

	WidgetInteractionComponent = CreateDefaultSubobject<UWidgetInteractionComponent>("WidgetInteraction");
	WidgetInteractionComponent->SetupAttachment(HandMesh);

	GrabSphere = CreateDefaultSubobject<USphereComponent>("GrabSphere");
	GrabSphere->SetupAttachment(HandMesh);
}

// Called when the game starts or when spawned
void AVRHand::BeginPlay()
{
	Super::BeginPlay();

	if (HandType != EControllerHand::Left && HandType != EControllerHand::Right)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1,4.f,FColor::Red, FString::Printf(TEXT("Class %s : Wrong Type"), *GetName()));
		}
	}
	
}

void AVRHand::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	switch (HandType)
	{
	case EControllerHand::Left :
		MotionController->MotionSource = "Left";
		break;
		
	case EControllerHand::Right :
		MotionController->MotionSource = "Right";
		break;
	}
}

void AVRHand::GrabOject()
{
	TArray<AActor*> OverlappingActors;
	GrabSphere->GetOverlappingActors(OverlappingActors);
	if (!OverlappingActors.IsEmpty())
	{
		AActor* FirstActorUnderCollision = OverlappingActors[0];
		if (FirstActorUnderCollision)
		{
			UActorComponent* GrabComp = nullptr;

			for (UActorComponent* Comp : FirstActorUnderCollision->GetComponents())
			{
				if (Comp->GetClass()->ImplementsInterface(UInteractInterface::StaticClass()))
				{
					GrabComp = Comp;
					break;
				}
			}

			if (GrabComp)
			{
				CurrentlyGrabbedActor = TScriptInterface<IInteractInterface>(GrabComp);
				if (CurrentlyGrabbedActor)
				{
					CurrentlyGrabbedActor->OnGrab(this, HandMesh->GetComponentLocation());
				}
			}
		}
	}
}

void AVRHand::ReleaseOject()
{
	if (CurrentlyGrabbedActor)
	{
		CurrentlyGrabbedActor->OnRelease(this, HandMesh->GetComponentLocation());
		CurrentlyGrabbedActor = nullptr;
	}
}

// Called every frame
void AVRHand::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

