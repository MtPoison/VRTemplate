// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "HandVR.generated.h"

/**
 * 
 */
UCLASS()
class VRSYSTEM_API UHandVR : public UAnimInstance
{
	GENERATED_BODY()
public:
	
	void SetMirror( bool Value ){ bMirror = Value; }

	void SetPoseAlphaGrasp( float Value ) { PoseAlphaGrasp = Value; }

	void SetPoseAlphaIndexCurl ( float Value ) { PoseAlphaIndexCurl = Value; }

	void SetPoseAlphaThumbUp ( float Value ) { PoseAlphaThumbUp = Value; }
	
	void SetPoseAlphaPoint ( float Value ) { PoseAlphaPoint = Value; }

	void SetCurrentPoseAlphaThumbUp ( float Value ) { CurrentPoseAlphaThumbUp = Value; }

	void SetCurrentPoseAlphaPoint ( float Value ) { CurrentPoseAlphaPoint = Value; }
	
protected:
	
	UPROPERTY(BlueprintReadOnly)
	bool bMirror;
	
	UPROPERTY(BlueprintReadOnly)
	float PoseAlphaGrasp;
	
	UPROPERTY(BlueprintReadOnly)
	float PoseAlphaIndexCurl;
	
	UPROPERTY(BlueprintReadOnly)
	float PoseAlphaThumbUp;

	UPROPERTY(BlueprintReadOnly)
	float PoseAlphaPoint;
	
	UPROPERTY(BlueprintReadWrite)
	float CurrentPoseAlphaThumbUp;
	
	UPROPERTY(BlueprintReadWrite)
	float CurrentPoseAlphaPoint;
	
};
