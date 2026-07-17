// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// These are base #include that come with the creation of this class.
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

// These must be included for IMC and IA to work.
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "InputAction.h"

// Class generated file, crucial for this class to be functional.
// Also part of the base #include but this MUST be the last 
// otherwise the program won't compile.
#include "MyPlayerController.generated.h"

/**
 *
 */
UCLASS()
class SAMPLEPROJECT_5_8_C_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	// Default constructor
	AMyPlayerController();

	// Called after object from this class has spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Usually called before BeginPlay() to setup Player Input
	virtual void SetupInputComponent() override;

	// Where the magic happens
	void CameraReset(float InDeltaTime);

	// Sets the bCameraReset to true
	void SetCameraResetTrue();

	// Function used by a Timer later setup to forcefully end Camera Reset
	void FinishCameraReset();

	// Movement Function
	void Move(const FInputActionValue& Value);

	// Look Function (to rotate Camera)
	void Look(const FInputActionValue& Value);

	// Jump Function (Yay! Jumping!)
	void Jump(const FInputActionValue& Value);

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

	// The variable that allows the entirety of the Camera Reset mechanic to happen or not.
	UPROPERTY(EditAnywhere, Category = "Camera|Reset")
	bool bCameraReset = false;

	// Used at the end when we want to SetControlRotation with RInterpTo.
	UPROPERTY(EditAnywhere, Category = "Camera|Reset")
	float InterpSpeed = 10.0f;

	// Timer that runs when called from the function bound to IA_CameraReset.
	// Check implementation within MyPlayerController.cpp.
	FTimerHandle Timer_FinishCameraReset;

	// Used on Timer_EndCameraReset
	UPROPERTY(EditAnywhere, Category = "Camera|Reset")
	float Timer_FCR_Seconds = 0.45f;
};
