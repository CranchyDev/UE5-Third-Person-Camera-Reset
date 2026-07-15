// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "YourPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class YOURPROJECT_API AYourPlayerController : public APlayerController
{
	GENERATED_BODY()
	
	virtual void Tick(float DeltaTime) override; // Ticks every frame

	virtual void BeginPlay() override; // Called after Class is spawned in the world

	virtual void SetupInputComponent() override; // Usually called before BeginPlay() to set Player Input

	// Where the magic happens
	void CameraReset(float InDeltaTime);

	// Sets the bCameraReset to true
	void SetCameraResetTrue();

	/*
	* Make sure that you're using either this bCameraReset or the one within the .cpp file.
	* Remove the one that you're not using and keep the other one.
	* Remember that, if you intend this variable to be accessed by another Class outside of this one,
	* then having it here is the only option.
	*
	* Plus if you need/want it exposed to Blueprints, then keeping it here is also the correct option.
	*
	*/

	// UPROPERTY(BlueprintReadWrite, Category = "Camera")
	// bool bCameraReset = false;
};
