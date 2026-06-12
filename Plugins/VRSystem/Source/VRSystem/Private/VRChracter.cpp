

#include "VRChracter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "HeadMountedDisplayTypes.h"
#include "NiagaraDataInterfaceArrayFunctionLibrary.h"
#include "NavigationSystem.h"
#include "Camera/CameraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/GameplayStaticsTypes.h"
#include "Kismet/KismetSystemLibrary.h"

AVRChracter::AVRChracter()
{
	PrimaryActorTick.bCanEverTick = true;

	Origin = CreateDefaultSubobject<USceneComponent>(TEXT("Origin"));
	Origin->SetupAttachment(GetMesh());
	
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(Origin);

	TeleportSpline = CreateDefaultSubobject<USplineComponent>(TEXT("TeleportSpline"));
	TeleportSpline->SetupAttachment(RootComponent);
}


void AVRChracter::BeginPlay()
{
	Super::BeginPlay();
	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
	{
		UHeadMountedDisplayFunctionLibrary::SetTrackingOrigin(TrackingOrigin);
		UHeadMountedDisplayFunctionLibrary::EnableHMD(true);
		UKismetSystemLibrary::ExecuteConsoleCommand(GetWorld(),TEXT("xr.SecondaryScreenPercentage.HMDRenderTarget 100"),nullptr);
		bSnapTurnTriggered = false;
	
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = GetInstigator();

		HandLeft = GetWorld()->SpawnActor<AVRHand>(
			HandClassLeft,
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			SpawnParams
		);
		if (HandLeft)
		{
			HandLeft->AttachToComponent(
				Origin,
				FAttachmentTransformRules::SnapToTargetNotIncludingScale
			);
		}

		HandRight = GetWorld()->SpawnActor<AVRHand>(
			HandClassRight,
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			SpawnParams
		);
		if (HandRight)
		{
			HandRight->AttachToComponent(
				Origin,
				FAttachmentTransformRules::SnapToTargetNotIncludingScale
			);
		}

		if (NS_TeleportArc)
		{
			NC_TeleportArc = UNiagaraFunctionLibrary::SpawnSystemAttached(
				NS_TeleportArc,
				RootComponent,
				NAME_None,
				FVector::ZeroVector,
				FRotator::ZeroRotator,
				EAttachLocation::KeepRelativeOffset,
				false
			);
			NC_TeleportArc->SetVisibility(false);
		}

		if (NS_TeleportVisualizer)
		{
			NC_TeleportVisualizer = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				GetWorld(),
				NS_TeleportVisualizer,
				FVector::ZeroVector,
				FRotator::ZeroRotator,
				FVector(1.f),
				false,
				false
			);
			NC_TeleportVisualizer->SetVisibility(false);
		}
	}
}


void AVRChracter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	GetWorldTimerManager().SetTimerForNextTick([this]()
	{
		SetupInputMappingContexts();
	});
}

void AVRChracter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	
}

void AVRChracter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

   UE_LOG(LogTemp, Warning, TEXT("[VR Input] SetupPlayerInputComponent appele"));

    UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent);
    if (!EIC)
    {
        UE_LOG(LogTemp, Error, TEXT("[VR Input] ERREUR - EnhancedInputComponent introuvable ! Verifie le DefaultPawn dans les settings"));
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("[VR Input] EnhancedInputComponent OK"));

    if (IA_Turn)
    {
        EIC->BindAction(IA_Turn, ETriggerEvent::Triggered, this, &AVRChracter::Turn);
        UE_LOG(LogTemp, Warning, TEXT("[VR Input] IA_Turn bind OK"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[VR Input] ERREUR - IA_Turn est NULL ! Assigne le dans le Blueprint"));
    }

    if (IA_Teleport)
    {
    	EIC->BindAction(IA_Teleport, ETriggerEvent::Started, this, &AVRChracter::OnTeleportStarted);
    	// Triggered = update l'arc chaque frame
    	EIC->BindAction(IA_Teleport, ETriggerEvent::Triggered, this, &AVRChracter::OnTeleportTriggered);
    	// Completed = cache l'arc + téléporte
    	EIC->BindAction(IA_Teleport, ETriggerEvent::Completed, this, &AVRChracter::OnTeleportCompleted);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[VR Input] ERREUR - IA_Teleport est NULL ! Assigne le dans le Blueprint"));
    }
}

void AVRChracter::SetupInputMappingContexts()
{
    UE_LOG(LogTemp, Warning, TEXT("[VR Input] SetupInputMappingContexts appele"));

    APlayerController* PC = Cast<APlayerController>(GetController());
    if (!PC)
    {
        UE_LOG(LogTemp, Error, TEXT("[VR Input] ERREUR - PlayerController NULL"));
        return;
    }
    UE_LOG(LogTemp, Warning, TEXT("[VR Input] PlayerController OK"));

    UEnhancedInputLocalPlayerSubsystem* Subsystem =
        ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
    if (!Subsystem)
    {
        UE_LOG(LogTemp, Error, TEXT("[VR Input] ERREUR - EnhancedInputSubsystem NULL"));
        return;
    }
    UE_LOG(LogTemp, Warning, TEXT("[VR Input] EnhancedInputSubsystem OK"));

	if (IMC_Default)
	{
		Subsystem->AddMappingContext(IMC_Default, 0);
		UE_LOG(LogTemp, Warning, TEXT("[VR Input] IMC_Default ajoute OK"));
	}

	if (IMC_Hands)
	{
		Subsystem->AddMappingContext(IMC_Hands, 1);
		UE_LOG(LogTemp, Warning, TEXT("[VR Input] IMC_Hands ajoute OK"));
	}
	
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[VR Input] ERREUR - IMC_VR est NULL ! Assigne le dans le Blueprint"));
    }
}
void AVRChracter::SnapTurnAroundCamera(float Direction)
{
	float LocalYawDelta = Direction * SnapTurnAngle;

	FRotator CurrentRotation = Origin->GetRelativeRotation();
	FRotator NewRotation = CurrentRotation + FRotator(0.f, LocalYawDelta, 0.f);
	Origin->SetRelativeRotation(NewRotation);
	GetMesh()->SetRelativeRotation(NewRotation);

	UE_LOG(LogTemp, Warning, TEXT("[VR Turn] Origin Yaw: %f"), NewRotation.Yaw);
}

void AVRChracter::Turn(const FInputActionValue& Value)
{
	float AxisValue = Value.Get<FVector2D>().X;
	UE_LOG(LogTemp, Warning, TEXT("[VR Turn] AxisValue: %f"), AxisValue);

	if (FMath::Abs(AxisValue) > JoystickDeadZone)
	{
		UE_LOG(LogTemp, Warning, TEXT("[VR Turn] Direction: %s"),
			AxisValue > 0 ? TEXT("DROITE") : TEXT("GAUCHE"));

		SnapTurnAroundCamera(FMath::Sign(AxisValue));
	}
		
}

void AVRChracter::OnTeleportStarted(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Warning, TEXT("[VR Teleport] Started - arc actif"));

	bTeleportActive = true;

	if (NC_TeleportArc)
		NC_TeleportArc->SetVisibility(true);

	// Spawn le visualizer
	if (NS_TeleportVisualizer && !NC_TeleportVisualizer)
	{
		NC_TeleportVisualizer = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			NS_TeleportVisualizer,
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			FVector(1.f),
			true,  // auto activate
			true   // pre cull check
		);

		if (NC_TeleportVisualizer)
		{
			NC_TeleportVisualizer->SetVisibility(true);
			UE_LOG(LogTemp, Warning, TEXT("[VR Teleport] Visualizer spawne OK"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[VR Teleport] ERREUR - Visualizer spawn failed ! NS_TeleportVisualizer est-il assigne dans le Blueprint ?"));
		}
	}
	else
	{
		if (!NS_TeleportVisualizer)
			UE_LOG(LogTemp, Error, TEXT("[VR Teleport] ERREUR - NS_TeleportVisualizer est NULL"));
		if (NC_TeleportVisualizer)
			UE_LOG(LogTemp, Warning, TEXT("[VR Teleport] Visualizer deja spawne"));
	}
}

void AVRChracter::OnTeleportTriggered(const FInputActionValue& Value)
{
	if (!bTeleportActive) return;
	UpdateTeleportArc();
}

void AVRChracter::OnTeleportCompleted(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Warning, TEXT("[VR Teleport] Completed - cache arc"));

	bTeleportActive = false;

	// Cache le Niagara arc
	if (NC_TeleportArc)
		NC_TeleportArc->SetVisibility(false);

	// Destroy le visualizer
	if (NC_TeleportVisualizer)
	{
		NC_TeleportVisualizer->DestroyComponent();
		NC_TeleportVisualizer = nullptr;
	}

	// Téléporte si destination valide
	if (bValidTeleportLocation)
		ExecuteTeleport();
	else
		UE_LOG(LogTemp, Warning, TEXT("[VR Teleport] Destination invalide - annule"));

	bValidTeleportLocation = false;
}

bool AVRChracter::TraceTeleportDestination(FVector& OutLocation)
{
	// Start Pos et Forward Vector viennent du Motion Controller Right Aim (image 1)
	// En C++ on utilise la caméra comme approximation
	FVector StartLocation = Camera->GetComponentLocation();
	FVector LaunchVelocity = Camera->GetForwardVector() * TeleportLaunchSpeed;

	FPredictProjectilePathParams Params;
	Params.StartLocation = StartLocation;
	Params.LaunchVelocity = LaunchVelocity;
	Params.bTraceWithCollision = true;
	Params.ProjectileRadius = 5.f;
	Params.MaxSimTime = 3.f;
	Params.SimFrequency = 15.f;
	Params.DrawDebugType = EDrawDebugTrace::None;
	Params.TraceChannel = ECC_Visibility;
	Params.ActorsToIgnore.Add(this);

	FPredictProjectilePathResult Result;
	bool bHit = UGameplayStatics::PredictProjectilePath(this, Params, Result);

	TeleportPathPositions.Empty();
	for (const FPredictProjectilePathPointData& Point : Result.PathData)
		TeleportPathPositions.Add(Point.Location);

	if (bHit)
	{
		UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
		FNavLocation NavLocation;
		if (NavSystem && NavSystem->ProjectPointToNavigation(Result.HitResult.Location, NavLocation))
		{
			OutLocation = NavLocation.Location;
			OutLocation.Z -= NavMeshCellHeight;
			return true;
		}
	}

	OutLocation = FVector::ZeroVector;
	return false;
}

void AVRChracter::UpdateTeleportArc()
{
	bValidTeleportLocation = TraceTeleportDestination(TeleportDestination);

	if (NC_TeleportArc)
	{
		UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayVector(
			NC_TeleportArc,
			FName("User.PointArray"),
			TeleportPathPositions
		);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[VR Teleport] ERREUR - NC_TeleportArc est NULL"));
	}

	if (NC_TeleportVisualizer)
	{
		if (bValidTeleportLocation)
		{
			NC_TeleportVisualizer->SetVisibility(true);
			NC_TeleportVisualizer->SetWorldLocation(TeleportDestination);
			UE_LOG(LogTemp, Warning, TEXT("[VR Teleport] Visualizer place a: %s"), *TeleportDestination.ToString());
		}
		else
		{
			NC_TeleportVisualizer->SetVisibility(false);
			UE_LOG(LogTemp, Warning, TEXT("[VR Teleport] Visualizer cache - destination invalide"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[VR Teleport] ERREUR - NC_TeleportVisualizer est NULL dans UpdateTeleportArc"));
	}

	UE_LOG(LogTemp, Warning, TEXT("[VR Teleport] Points: %d | Valide: %s | Dest: %s"),
		TeleportPathPositions.Num(),
		bValidTeleportLocation ? TEXT("OUI") : TEXT("NON"),
		*TeleportDestination.ToString());
}
void AVRChracter::ExecuteTeleport()
{
	// Reproduit exactement le Blueprint image 6 :
	// Camera Relative Location -> Break -> Make (X, Y, Z=0) -> RotateVector par Actor Yaw -> soustrait de la destination

	FVector CameraRelativeLocation = Camera->GetRelativeLocation();

	// Ignore la hauteur (Z=0) comme dans le Blueprint
	FVector CameraOffset2D = FVector(CameraRelativeLocation.X, CameraRelativeLocation.Y, 0.f);

	// RotateVector par le Yaw de l'actor
	FVector RotatedOffset = GetActorRotation().RotateVector(CameraOffset2D);

	// Destination finale = ProjectedTeleportLocation - RotatedOffset
	FVector NewLocation = TeleportDestination - RotatedOffset;
	NewLocation.Z = TeleportDestination.Z;

	// Teleport node du Blueprint
	TeleportTo(NewLocation, GetActorRotation());

	UE_LOG(LogTemp, Warning, TEXT("[VR Teleport] Teleporte vers: %s"), *NewLocation.ToString());
}

