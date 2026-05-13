// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <rapidjson/internal/meta.h>

#include "CoreMinimal.h"
#include "NiagaraComponent.h"
#include "GameFramework/Actor.h"
#include "TeleportVisualizer.generated.h"

UCLASS()
class VRSYSTEM_API ATeleportVisualizer : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATeleportVisualizer();
	virtual void Tick(float DeltaTime) override;

	void SetPlayerPawn(APawn* Value) {PlayerPawn = Value;}
	void SetPlayerCameraManager(APlayerCameraManager* Value) {PlayerCameraManager = Value;}
	USceneComponent* GetRootComponent() const { return Root;}

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:	
	// Called every frame
	UPROPERTY(VisibleAnywhere, Category = "Root")
	TObjectPtr<USceneComponent> Root;
	
	UPROPERTY(VisibleAnywhere, Category = "NS")
	TObjectPtr<UNiagaraComponent> PlayerAreaBounds;
	
	UPROPERTY(VisibleAnywhere, Category = "NS")
	TObjectPtr<UNiagaraComponent> TeleportingRing;

	UPROPERTY()
	TObjectPtr<APawn> PlayerPawn;

	UPROPERTY()
	TObjectPtr<APlayerCameraManager> PlayerCameraManager;
};
