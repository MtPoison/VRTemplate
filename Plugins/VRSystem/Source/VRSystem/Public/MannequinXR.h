// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "MannequinXR.generated.h"

/**
 * 
 */
UCLASS()
class VRSYSTEM_API UMannequinXR : public USkeletalMeshComponent
{
	GENERATED_BODY()
public:
	bool GetMirror(){return bMirror;}
private:
	bool bMirror;
	
};
