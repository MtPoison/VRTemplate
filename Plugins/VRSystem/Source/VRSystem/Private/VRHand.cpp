// Fill out your copyright notice in the Description page of Project Settings.


#include "VRHand.h"
#include "EnhancedInputComponent.h"
#include "HandVR.h"
#include "MotionControllerComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetInteractionComponent.h"
// Sets default values
AVRHand::AVRHand()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
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
	
	HandVR = Cast<UHandVR>(HandMesh->GetAnimInstance());
	
	if (HandVR)
	{
		HandVR->SetMirror(bMirrorAnimation);
	}

	APawn* OwnerPawn = Cast<APawn>(GetOwner());

	if (OwnerPawn)
	{
		PlayerController =
			Cast<APlayerController>(OwnerPawn->GetController());
	}

	UE_LOG(LogTemp, Error, TEXT("%s -AnimInstance"), *HandMesh->GetAnimInstance()->GetName());
	EnableInput(PlayerController);
	GetWorldTimerManager().SetTimerForNextTick(this, &AVRHand::MappingInput);
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

#pragma region Input

void AVRHand::MappingInput()
{
	UE_LOG(LogTemp, Warning, TEXT("=== MappingInput called on %s ==="), *GetName());
	UE_LOG(LogTemp, Warning, TEXT("MappingInput - InputComponent: %s"), InputComponent ? TEXT("VALID") : TEXT("NULL"));
	UE_LOG(LogTemp, Warning, TEXT("MappingInput - PlayerController: %s"), PlayerController ? TEXT("VALID") : TEXT("NULL"));
	UE_LOG(LogTemp, Warning, TEXT("MappingInput - HandType: %s"), HandType == EControllerHand::Left ? TEXT("Left") : TEXT("Right"));

	if (!InputComponent) return;

	UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent);
	UE_LOG(LogTemp, Warning, TEXT("EIC: %s"), EIC ? TEXT("VALID") : TEXT("NULL"));

	if (!EIC) return;

	if (HandType == EControllerHand::Left)
	{
		UE_LOG(LogTemp, Warning, TEXT("MappingInput - IA_Hand_Grasp_Left: %s"), IA_Hand_Grasp_Left ? TEXT("VALID") : TEXT("NULL"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("MappingInput - IA_Hand_Grasp_Right: %s"), IA_Hand_Grasp_Right ? TEXT("VALID") : TEXT("NULL"));
	}
	if (EIC)
	{
		
		if (HandType == EControllerHand::Left)
		{
		    if (IA_Hand_Grasp_Left)
		    {
		        EIC->BindAction(IA_Hand_Grasp_Left, ETriggerEvent::Triggered, this, &AVRHand::TriggerPoseAlphaGrasp);
		        EIC->BindAction(IA_Hand_Grasp_Left, ETriggerEvent::Canceled, this, &AVRHand::PoseAlphaGrasp);
		        EIC->BindAction(IA_Hand_Grasp_Left, ETriggerEvent::Completed, this, &AVRHand::PoseAlphaGrasp);
		    }
		    if (IA_Hand_IndexCurl_Left)
		    {
		        EIC->BindAction(IA_Hand_IndexCurl_Left, ETriggerEvent::Triggered, this, &AVRHand::TriggerPoseAlphaIndexCurl);
		        EIC->BindAction(IA_Hand_IndexCurl_Left, ETriggerEvent::Canceled, this, &AVRHand::PoseAlphaIndexCurl);
		        EIC->BindAction(IA_Hand_IndexCurl_Left, ETriggerEvent::Completed, this, &AVRHand::PoseAlphaIndexCurl);
		    }
		    if (IA_Hand_Point_Left)
		    {
		        EIC->BindAction(IA_Hand_Point_Left, ETriggerEvent::Started, this, &AVRHand::PoseAlphaPoint);
		        EIC->BindAction(IA_Hand_Point_Left, ETriggerEvent::Canceled, this, &AVRHand::PoseAlphaPoint);
		        EIC->BindAction(IA_Hand_Point_Left, ETriggerEvent::Completed, this, &AVRHand::CompletedPoseAlphaPoint);
		    }
		    if (IA_Hand_ThumpUp_Left)
		    {
		        EIC->BindAction(IA_Hand_ThumpUp_Left, ETriggerEvent::Started, this, &AVRHand::PoseAlphaThumbUp);
		        EIC->BindAction(IA_Hand_ThumpUp_Left, ETriggerEvent::Canceled, this, &AVRHand::PoseAlphaThumbUp);
		        EIC->BindAction(IA_Hand_ThumpUp_Left, ETriggerEvent::Completed, this, &AVRHand::CompletedPoseAlphaThumbUp);
		    }
		    if (IA_Grab_Left_Pressed)
		    {
		        EIC->BindAction(IA_Grab_Left_Pressed, ETriggerEvent::Started, this, &AVRHand::GrabOject);
		    }
		    if (IA_Grab_Left_Released)
		    {
		        EIC->BindAction(IA_Grab_Left_Released, ETriggerEvent::Completed, this, &AVRHand::ReleaseOject);
		        EIC->BindAction(IA_Grab_Left_Released, ETriggerEvent::Canceled, this, &AVRHand::ReleaseOject);
		    }
		}
		else 
		{
		    if (IA_Hand_Grasp_Right)
		    {
		        EIC->BindAction(IA_Hand_Grasp_Right, ETriggerEvent::Triggered, this, &AVRHand::TriggerPoseAlphaGrasp);
		        EIC->BindAction(IA_Hand_Grasp_Right, ETriggerEvent::Canceled, this, &AVRHand::PoseAlphaGrasp);
		        EIC->BindAction(IA_Hand_Grasp_Right, ETriggerEvent::Completed, this, &AVRHand::PoseAlphaGrasp);
		    }
		    if (IA_Hand_IndexCurl_Right)
		    {
		        EIC->BindAction(IA_Hand_IndexCurl_Right, ETriggerEvent::Triggered, this, &AVRHand::TriggerPoseAlphaIndexCurl);
		        EIC->BindAction(IA_Hand_IndexCurl_Right, ETriggerEvent::Canceled, this, &AVRHand::PoseAlphaIndexCurl);
		        EIC->BindAction(IA_Hand_IndexCurl_Right, ETriggerEvent::Completed, this, &AVRHand::PoseAlphaIndexCurl);
		    }
		    if (IA_Hand_Point_Right)
		    {
		        EIC->BindAction(IA_Hand_Point_Right, ETriggerEvent::Started, this, &AVRHand::PoseAlphaPoint);
		        EIC->BindAction(IA_Hand_Point_Right, ETriggerEvent::Canceled, this, &AVRHand::PoseAlphaPoint);
		        EIC->BindAction(IA_Hand_Point_Right, ETriggerEvent::Completed, this, &AVRHand::CompletedPoseAlphaPoint);
		    }
		    if (IA_Hand_ThumpUp_Right)
		    {
		        EIC->BindAction(IA_Hand_ThumpUp_Right, ETriggerEvent::Started, this, &AVRHand::PoseAlphaThumbUp);
		        EIC->BindAction(IA_Hand_ThumpUp_Right, ETriggerEvent::Canceled, this, &AVRHand::PoseAlphaThumbUp);
		        EIC->BindAction(IA_Hand_ThumpUp_Right, ETriggerEvent::Completed, this, &AVRHand::CompletedPoseAlphaThumbUp);
		    }
		    if (IA_Grab_Right_Pressed)
		    {
		        EIC->BindAction(IA_Grab_Right_Pressed, ETriggerEvent::Started, this, &AVRHand::GrabOject);
		    }
		    if (IA_Grab_Right_Released)
		    {
		        EIC->BindAction(IA_Grab_Right_Released, ETriggerEvent::Completed, this, &AVRHand::ReleaseOject);
		        EIC->BindAction(IA_Grab_Right_Released, ETriggerEvent::Canceled, this, &AVRHand::ReleaseOject);
		    }
		}
		
	}
}

#pragma endregion

#pragma region InitializeABP

void AVRHand::TriggerPoseAlphaGrasp(const FInputActionValue& Value)
{
	float FloatValue = Value.Get<float>();

	if (HandVR)
	{
		HandVR->SetPoseAlphaGrasp(FloatValue);
	}
}

void AVRHand::PoseAlphaGrasp()
{
	if (HandVR)
	{
		HandVR->SetPoseAlphaGrasp(0);
	}
}

void AVRHand::TriggerPoseAlphaIndexCurl(const FInputActionValue& Value)
{
	float FloatValue = Value.Get<float>();

	if (HandVR)
	{
		HandVR->SetPoseAlphaIndexCurl(FloatValue);
	}
}

void AVRHand::PoseAlphaIndexCurl()
{
	if (HandVR)
	{
		HandVR->SetPoseAlphaIndexCurl(0);
	}
}

void AVRHand::CompletedPoseAlphaPoint(const FInputActionValue& Value)
{
	float FloatValue = Value.Get<float>();

	if (HandVR)
	{
		HandVR->SetPoseAlphaPoint(FloatValue);
	}
}

void AVRHand::PoseAlphaPoint()
{
	if (HandVR)
	{
		HandVR->SetPoseAlphaPoint(1);
	}
}

void AVRHand::CompletedPoseAlphaThumbUp(const FInputActionValue& Value)
{
	float FloatValue = Value.Get<float>();

	if (HandVR)
	{
		HandVR->SetPoseAlphaThumbUp(1);
	}
}

void AVRHand::PoseAlphaThumbUp()
{
	if (HandVR)
	{
		HandVR->SetPoseAlphaThumbUp(0);
	}
}

#pragma endregion

#pragma region GrabSystem

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

#pragma endregion

