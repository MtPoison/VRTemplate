// Fill out your copyright notice in the Description page of Project Settings.


#include "VRPlayer.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/WidgetInteractionComponent.h"
#include "MotionControllerComponent.h"
#include "NiagaraComponent.h"
#include "EnhancedInputComponent.h"
#include "GrabComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "NavigationSystem.h"
#include "VRNotificationsComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraDataInterfaceArrayFunctionLibrary.h"
#include "Kismet/GameplayStaticsTypes.h"

// Sets default values
AVRPlayer::AVRPlayer()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    VROrigin = CreateDefaultSubobject<USceneComponent>(TEXT("VROrigin"));
    SetRootComponent(VROrigin);

    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(VROrigin);

    HeadMountedDisplayMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HeadMountedDisplayMesh"));
    HeadMountedDisplayMesh->SetupAttachment(Camera);

    MotionControllerLeftAim = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("MotionControllerLeftAim"));
    MotionControllerLeftAim->SetupAttachment(VROrigin);
    MotionControllerLeftAim->MotionSource = FName("LeftAim");

    WidgetInteractionLeft = CreateDefaultSubobject<UWidgetInteractionComponent>(TEXT("WidgetInteractionLeft"));
    WidgetInteractionLeft->SetupAttachment(MotionControllerLeftAim);

    MotionControllerRightAim = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("MotionControllerRightAim"));
    MotionControllerRightAim->SetupAttachment(VROrigin);
    MotionControllerRightAim->MotionSource = FName("RightAim");

    MotionControllerLeftGrip = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("MotionControllerLeftGrip"));
    MotionControllerLeftGrip->SetupAttachment(MotionControllerRightAim);
    MotionControllerLeftGrip->MotionSource = FName("LeftGrip");

  

    XRDeviceVisualizationLeft = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("XRDeviceVisualizationLeft"));
    XRDeviceVisualizationLeft->SetupAttachment(MotionControllerLeftGrip);

    WidgetInteractionRight = CreateDefaultSubobject<UWidgetInteractionComponent>(TEXT("WidgetInteractionRight"));
    WidgetInteractionRight->SetupAttachment(MotionControllerRightAim);

    MotionControllerRightGrip = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("MotionControllerRightGrip"));
    MotionControllerRightGrip->SetupAttachment(VROrigin);
    MotionControllerRightGrip->MotionSource = FName("RightGrip");


    XRDeviceVisualizationRight = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("XRDeviceVisualizationRight"));
    XRDeviceVisualizationRight->SetupAttachment(MotionControllerRightGrip);

    TeleportTraceNiagaraSystem = CreateDefaultSubobject<UNiagaraComponent>(TEXT("TeleportTraceNiagaraSystem"));
    TeleportTraceNiagaraSystem->SetupAttachment(VROrigin);

    VRNotifications = CreateDefaultSubobject<UVRNotificationsComponent>(TEXT("VRNotifications"));

}

// Called when the game starts or when spawned
void AVRPlayer::BeginPlay()
{
	Super::BeginPlay();
	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
	{
		UHeadMountedDisplayFunctionLibrary::SetTrackingOrigin(TrackingOrigin);
		UHeadMountedDisplayFunctionLibrary::SetSpectatorScreenMode(ESpectatorScreenMode::Disabled);
		UKismetSystemLibrary::ExecuteConsoleCommand(GetWorld(),TEXT("xr.SecondaryScreenPercentage.HMDRenderTarget 100"),nullptr);
		if (IsLocallyControlled())
		{
			SetupInputMappingContexts();
		}
	}
}

void AVRPlayer::SetupInputMappingContexts()
{
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC)
	{
		return;
	}

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
	

	if (!Subsystem)
	{
		return;
	}

	if (IMC_Default)
	{
		Subsystem->AddMappingContext(IMC_Default, 0);
	}

	if (IMC_Hands)
	{
		Subsystem->AddMappingContext(IMC_Hands, 0);
	}
}

float AVRPlayer::CalculateSnapTurnAngle(bool bRightTurn)
{
	float A = FMath::Abs(SnapTurnDegrees);
	float B = 0.f;

	float SnapAngle = bRightTurn ? A : B;

	return SnapAngle;
}

void AVRPlayer::OnTurn(const FInputActionValue& Value)
{
	float TurnValue = Value.Get<float>();

	bool bRightTurn = TurnValue > 0.f;

	SnapTurn(bRightTurn);
}

void AVRPlayer::TeleportTrace()
{

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));

	FPredictProjectilePathParams PredictParams;
	PredictParams.StartLocation = MotionControllerRightAim->GetComponentLocation();
	PredictParams.LaunchVelocity = LocalTeleportLaunchSpeed * MotionControllerRightAim->GetForwardVector();
	PredictParams.bTraceWithChannel = false;
	PredictParams.bTraceWithCollision = true;
	PredictParams.ProjectileRadius = LocalTeleportProjectileRadius;
	PredictParams.ObjectTypes = ObjectTypes;
	PredictParams.bTraceComplex = false;
	PredictParams.ActorsToIgnore = {};

	FPredictProjectilePathResult PredictResult;

	bool bHit = UGameplayStatics::PredictProjectilePath(GetWorld(), PredictParams, PredictResult);

	FHitResult OutHit= PredictResult.HitResult;
	TArray<FPredictProjectilePathPointData> OutPathPositions= PredictResult.PathData;
	FVector OutLastTraceDestination = PredictResult.LastTraceDestination.Location;

	for (const FPredictProjectilePathPointData& PathPoint : OutPathPositions)
	{
		TeleportTracePathPositions.Add(PathPoint.Location);
	}

	TeleportTracePathPositions.Insert(MotionControllerRightAim->GetComponentLocation(), 0);

	ProjectedTeleportLocation = FVector(OutHit.Location.X, OutHit.Location.Y, OutHit.Location.Z - LocalNavMeshCellHeight);

	const UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	FNavLocation NavLocation;
	bool ValidTeleportLocation = NavSystem && NavSystem->ProjectPointToNavigation(
		OutHit.Location,
		NavLocation,
		FVector(0, 0, 100)
	);
	
	
	UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayVector(
		TeleportTraceNiagaraSystem,
		FName("MyVectorArray"),
		TeleportTracePathPositions
	);
}

/*void AVRPlayer::StartTeleportTrace()
{
	bTeleportTraceActive = true;
	TeleportTraceNiagaraSystem->SetVisibility(true);

void AVRPlayer::EndTeleportTrace()
{
	bTeleportTraceActive = false;
	if (TeleportVisualizer)
	{
	}
	TeleportTraceNiagaraSystem->SetVisibility(false);
}

void AVRPlayer::TryTeleport()
{
	if (bValidTeleportLocation)
	{
		bValidTeleportLocation = false;
		FVector FlatLocation = FVector(Camera->GetRelativeLocation().X, Camera->GetRelativeLocation().Y, 0);
		FRotator ActorRotation = FRotator (GetActorRotation().Pitch, GetActorRotation().Yaw,0);

		FVector RotatedVector = ActorRotation.RotateVector(FlatLocation);
		
		TeleportTo((ProjectedTeleportLocation - FlatLocation), FRotator(0,0,ActorRotation.Roll));
	}
}*/

UGrabComponent* AVRPlayer::GetGrabComponentNearMotionController(UMotionControllerComponent* MontionControllerValue)
{
	
	float LocalNearestComponentDistance = FLT_MAX;
	UGrabComponent* LocalNearestComponent = nullptr;
	
	FVector LocalGripPosition = MontionControllerValue->GetComponentLocation();
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_PhysicsBody));

	FHitResult OutHit;
	
	
	return LocalNearestComponent;
}

void AVRPlayer::GrabLeftPressed()
{
	UGrabComponent* TmpHeldComponentLeft = GetGrabComponentNearMotionController(MotionControllerLeftGrip);
	
	if (TmpHeldComponentLeft)
	{
		
		
	}
}

void AVRPlayer::GrabLeftReleassed()
{

}

void AVRPlayer::GrabRightPressed()
{
	UGrabComponent* TmpHeldComponentRight = GetGrabComponentNearMotionController(MotionControllerLeftGrip);
	
	if (TmpHeldComponentRight)
	{
		
		
	}
}

void AVRPlayer::GrabRightReleassed()
{
	
}

void AVRPlayer::OnHandGraspLeft_Trigger(const FInputActionValue& Value)
{
}

void AVRPlayer::OnHandGraspLeft()
{
	
}

void AVRPlayer::OnHandIndexCurlLeft_Trigger(const FInputActionValue& Value)
{
	
}

void AVRPlayer::OnHandIndexCurlLeft()
{
	
}

void AVRPlayer::OnHandPointLeft(const FInputActionValue& Value)
{
	
}

void AVRPlayer::OnHandPointLeft_Completed()
{
	
}

void AVRPlayer::HandThumbUpLeft(const FInputActionValue& Value)
{
	
}

void AVRPlayer::HandThumbUpLeft_Completed()
{
	
}

void AVRPlayer::OnHandGraspRight_Trigger(const FInputActionValue& Value)
{
	
}

void AVRPlayer::OnHandGraspRight()
{
	
}

void AVRPlayer::OnHandIndexCurlRight_Trigger(const FInputActionValue& Value)
{
	
}

void AVRPlayer::OnHandIndexCurlRight()
{
	
}

void AVRPlayer::OnHandPointRight(const FInputActionValue& Value)
{
	
}

void AVRPlayer::OnHandPointRight_Completed()
{
	
}

void AVRPlayer::HandThumbUpRight(const FInputActionValue& Value)
{
	
}

void AVRPlayer::HandThumbUpRight_Completed()
{
	
}

void AVRPlayer::HideUnhideHand_Implementation(bool bHide, bool bRightHand)
{
	
}

// Called every frame
void AVRPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AVRPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!EIC)
	{
		return;
	}

	if (IA_Turn)
	{
		EIC->BindAction(IA_Turn, ETriggerEvent::Triggered, this, &AVRPlayer::OnTurn);
	}
	
	if (IA_Move)
	{
		EIC->BindAction(IA_Move, ETriggerEvent::Triggered, this, &AVRPlayer::TeleportTrace);
	}
	
	if (IA_Grab_Left_Pressed)
	{
		EIC->BindAction(IA_Grab_Left_Pressed, ETriggerEvent::Triggered, this, &AVRPlayer::GrabLeftPressed);
	}

	if (IA_Grab_Left_Relassed)
	{
		EIC->BindAction(IA_Grab_Left_Relassed, ETriggerEvent::Triggered, this, &AVRPlayer::GrabLeftReleassed);
	}

	if (IA_Grab_Right_Pressed)
	{
		EIC->BindAction(IA_Grab_Right_Pressed, ETriggerEvent::Triggered, this, &AVRPlayer::GrabRightPressed);
	}

	if (IA_Grab_Right_Relassed)
	{
		EIC->BindAction(IA_Grab_Right_Relassed, ETriggerEvent::Triggered, this, &AVRPlayer::GrabRightReleassed);
	}

	if (IA_Hand_Grasp_Left)
	{
		EIC->BindAction(IA_Hand_Grasp_Left, ETriggerEvent::Triggered, this, &AVRPlayer::OnHandGraspLeft_Trigger);
		EIC->BindAction(IA_Hand_Grasp_Left, ETriggerEvent::Canceled, this, &AVRPlayer::OnHandGraspLeft);
		EIC->BindAction(IA_Hand_Grasp_Left, ETriggerEvent::Completed, this, &AVRPlayer::OnHandGraspLeft);
	}

	if (IA_Hand_IndexCurl_Left)
	{
		EIC->BindAction(IA_Hand_IndexCurl_Left, ETriggerEvent::Triggered, this, &AVRPlayer::OnHandIndexCurlLeft_Trigger);
		EIC->BindAction(IA_Hand_IndexCurl_Left, ETriggerEvent::Canceled, this, &AVRPlayer::OnHandIndexCurlLeft);
		EIC->BindAction(IA_Hand_IndexCurl_Left, ETriggerEvent::Completed, this, &AVRPlayer::OnHandIndexCurlLeft);
	}

	if (IA_Hand_Point_Left)
	{
		EIC->BindAction(IA_Hand_IndexCurl_Left, ETriggerEvent::Triggered, this, &AVRPlayer::OnHandPointLeft);
		EIC->BindAction(IA_Hand_IndexCurl_Left, ETriggerEvent::Canceled, this, &AVRPlayer::OnHandPointLeft);
		EIC->BindAction(IA_Hand_IndexCurl_Left, ETriggerEvent::Completed, this, &AVRPlayer::OnHandPointLeft_Completed);
	}

	if (IA_Hand_Point_Left)
	{
		EIC->BindAction(IA_Hand_IndexCurl_Left, ETriggerEvent::Triggered, this, &AVRPlayer::HandThumbUpLeft);
		EIC->BindAction(IA_Hand_IndexCurl_Left, ETriggerEvent::Canceled, this, &AVRPlayer::HandThumbUpLeft);
		EIC->BindAction(IA_Hand_IndexCurl_Left, ETriggerEvent::Completed, this, &AVRPlayer::HandThumbUpLeft_Completed);
	}

	if (IA_Hand_Grasp_Right)
	{
		EIC->BindAction(IA_Hand_Grasp_Right, ETriggerEvent::Triggered, this, &AVRPlayer::OnHandGraspRight_Trigger);
		EIC->BindAction(IA_Hand_Grasp_Right, ETriggerEvent::Canceled, this, &AVRPlayer::OnHandGraspRight);
		EIC->BindAction(IA_Hand_Grasp_Right, ETriggerEvent::Completed, this, &AVRPlayer::OnHandGraspRight);
	}

	if (IA_Hand_IndexCurl_Right)
	{
		EIC->BindAction(IA_Hand_IndexCurl_Right, ETriggerEvent::Triggered, this, &AVRPlayer::OnHandIndexCurlRight_Trigger);
		EIC->BindAction(IA_Hand_IndexCurl_Right, ETriggerEvent::Canceled, this, &AVRPlayer::OnHandIndexCurlRight);
		EIC->BindAction(IA_Hand_IndexCurl_Right, ETriggerEvent::Completed, this, &AVRPlayer::OnHandIndexCurlRight);
	}

	if (IA_Hand_Point_Right)
	{
		EIC->BindAction(IA_Hand_IndexCurl_Right, ETriggerEvent::Triggered, this, &AVRPlayer::OnHandPointRight);
		EIC->BindAction(IA_Hand_IndexCurl_Right, ETriggerEvent::Canceled, this, &AVRPlayer::OnHandPointRight);
		EIC->BindAction(IA_Hand_IndexCurl_Right, ETriggerEvent::Completed, this, &AVRPlayer::OnHandPointRight_Completed);
	}

	if (IA_Hand_Point_Right)
	{
		EIC->BindAction(IA_Hand_IndexCurl_Right, ETriggerEvent::Triggered, this, &AVRPlayer::HandThumbUpRight);
		EIC->BindAction(IA_Hand_IndexCurl_Right, ETriggerEvent::Canceled, this, &AVRPlayer::HandThumbUpRight);
		EIC->BindAction(IA_Hand_IndexCurl_Right, ETriggerEvent::Completed, this, &AVRPlayer::HandThumbUpRight_Completed);
	}
}

void AVRPlayer::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
}

void AVRPlayer::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
}

void AVRPlayer::SnapTurn(bool RightTurn)
{
	float LocalYawDelta = CalculateSnapTurnAngle(RightTurn);

	FVector LocalCameraLocation = Camera->GetComponentLocation();

	FTransform LocalCameraRelativeTransform = Camera->GetRelativeTransform();

	FVector ActorLocation = GetActorLocation();
	FRotator ActorRotation = GetActorRotation();

	FRotator CombinedRotation = ActorRotation + FRotator(0.f, LocalYawDelta, 0.f);

	FTransform NewTransform(CombinedRotation, ActorLocation, FVector(1.f, 1.f, 1.f));
	
	FTransform LocalNewTransform = NewTransform;

	AddActorWorldRotation(FRotator(0.f, LocalYawDelta, 0.f));

	FTransform TmpTransform = LocalCameraRelativeTransform * LocalNewTransform;

	FVector TmpLocation = LocalCameraLocation - TmpTransform.GetLocation();

	SetActorLocation(TmpLocation + GetActorLocation(),false, nullptr, ETeleportType::TeleportPhysics);
}

