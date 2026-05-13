// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MannequinsXR.generated.h"

/**
 * 
 */
UCLASS()
class VRSYSTEM_API UMannequinsXR : public UAnimInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hand", meta=(AllowPrivateAccess="true"))
	float PoseAlphaGrasp = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hand", meta=(AllowPrivateAccess="true"))
	float PoseAlphaIndexCurl = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hand", meta=(AllowPrivateAccess="true"))
	float PoseAlphaPoint = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hand", meta=(AllowPrivateAccess="true"))
	float PoseAlphaThumbUp = 0.f;
};
