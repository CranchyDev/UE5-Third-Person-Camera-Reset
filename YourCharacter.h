// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "InputAction.h"

#include "YourCharacter.generated.h"

/**
 * 
 */
UCLASS()
class YOURPROJECT_API AYourCharacter : public ACharacter
{
	GENERATED_BODY()
	
protected:
	AYourCharacter(); // Constructor

	virtual void Tick(float DeltaTime) override; // Ticks every frame

	virtual void BeginPlay() override; // Called after Class is spawned in the world

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override; // Usually called before BeginPlay() to set Player Input

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

	// If this IMC (InputMappingContext) isn't defaulted within the Editor, 
	// you MUST instantiate it within the .cpp file.
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputMappingContext* IMC_Default;

	// Same goes here for this IA (InputAction).
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* IA_CameraReset;
};
