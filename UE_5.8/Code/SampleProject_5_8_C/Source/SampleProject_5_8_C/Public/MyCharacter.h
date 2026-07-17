// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

// Remove comment if you're using SetupPlayerInputComponent here
/**/
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "InputAction.h"

// Adds Spring Arm and Camera components
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

#include "MyCharacter.generated.h"

/**
*
*/
UCLASS()
class SAMPLEPROJECT_5_8_C_API AMyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMyCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Where the magic happens
	void CameraReset(float InDeltaTime);

	// Sets the bCameraReset to true
	void SetCameraResetTrue();

	// Function used by a Timer later setup to forcefully end Camera Reset
	void FinishCameraReset();

	//Comment these out if you are using SetupPlayerInputComponent here

	// Movement Function
	void Move(const FInputActionValue& Value);

	// Look Function (to rotate Camera)
	void Look(const FInputActionValue& Value);

	// Jump Function (Yay! Jumping!)
	void JumpStart(const FInputActionValue& Value);

	// InputMappingContext that's going to be used for Player Input
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputMappingContext* IMC_Default = nullptr;

	// InputAction for CameraReset
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* IA_CameraReset = nullptr;

	// InputAction for Movement
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* IA_Move = nullptr;

	// InputAction for Camera Rotation
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* IA_Look = nullptr;

	// InputAction for Jump (because why not, I love Jumping() in games!)
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* IA_Jump = nullptr;

	// Adding a Spring Component here for Camera usability
	UPROPERTY(EditAnywhere, Category = "Camera")
	USpringArmComponent* SpringArm;

	// Adding a Camera here so that you can control the 
	UPROPERTY(EditAnywhere, Category = "Camera")
	UCameraComponent* Camera;

	// The variable that allows the entirety of the Camera Reset mechanic to happen or not.
	UPROPERTY(EditAnywhere, Category = "Camera|Reset")
	bool bCameraReset = false;

	// Used at the end when we want to SetControlRotation with RInterpTo.
	UPROPERTY(EditAnywhere, Category = "Camera|Reset")
	float InterpSpeed = 10.0f;

	// Timer that runs whenever bCameraReset is called to true,
	// that forcefully finishes the Camera Reset mechanic to prevent erratic behavior.
	// Check implementation within MyPlayerController.cpp
	FTimerHandle Timer_FinishCameraReset;

	// Used on Timer_EndCameraReset
	UPROPERTY(EditAnywhere, Category = "Camera|Reset")
	float Timer_FCR_Seconds = 0.45f;
};
