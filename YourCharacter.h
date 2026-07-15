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

	// Rotates Camera 
	void Look(const FInputActionValue& Value);

	// Character Movement Functions (this is very archaic, please change this setup if possible)
	void MoveForward();
	void MoveLeft();
	void MoveBackward();
	void MoveRight();

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

	/*
	* Adding extra variables here in combination with functions related to movement,
	* to help you test the mechanic of CameraReset more easily.
	* 
	* The variables, therefore, are: 
	* 1 - InputMappingContext
	* 5 - InputActions
	* 
	*/

	// InputMappingContext 
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputMappingContext* IMC_Default = nullptr;

	// InputAction for CameraReset
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* IA_CameraReset = nullptr;

	// InputAction for Forward Movement
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* IA_MoveForward = nullptr;

	// InputAction for Left Movement
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* IA_MoveLeft = nullptr;

	// InputAction for Backward Movement
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* IA_MoveBackward = nullptr;

	// InputAction for Right Movement
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* IA_MoveRight = nullptr;

	// InputAction for Camera Rotation
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* IA_Look = nullptr;

	// Adds a Spring Arm to the Character that the CameraThirdPerson will be attached to
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* SpringArm = nullptr;

	// Adds a Camera Component to the Character so that the player can actually look around
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* Camera = nullptr;
};
