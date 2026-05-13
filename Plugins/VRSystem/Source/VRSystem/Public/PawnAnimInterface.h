// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PawnAnimInterface.generated.h"

/**
 * 
 */
UINTERFACE(Blueprintable)
class VRSYSTEM_API UPawnAnimInterface : public UInterface
{
	GENERATED_BODY()
};

class VRSYSTEM_API IPawnAnimInterface
{
	GENERATED_BODY()
public:
	

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Animation")
	void HideUnhideHand(bool bHide, bool bRightHand);
	
};
