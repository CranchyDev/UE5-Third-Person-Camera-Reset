# UE5-Third-Person-Camera-Reset
Short and simple Camera Reset mechanic that modifies the Control Rotation Yaw towards the Actor's Rotation Yaw through RInterpTo/RConstantInterpTo (you can choose). Pitch is default 0.0f but can be Actor's Rotation Pitch instead. C++ and Blueprint available!

## Introduction
Hi! Welcome to yet another one of my repositories!

This time, I present to you a very "simple" (this wasn't so simple when I had o stay up till 03:00AM though) that resets the Control Rotation (will explain what this is in a bit), towards wherever the Actor's Rotation that the camera is attached to is.
In other words, if the Character you are utilizing in-game is facing slightly towards left of where your Camera point of view is actually pointing, meaning that, the Character isn't facing the same direction as the Camera, then, hopefully, by tapping an Input (Keyboard/Gamepad/VR/Etc.), the Camera essentially interpolates wihin a certain speed towards the specific angle that the Character's facing.

The code layed here allows you to choose the Pitch and/or Yaw. Usually the ControlRotation.Pitch will be set to 0.0f, though you can easily swap it for the Actor Rotation Pitch, and also modify the ControlRotation.Yaw towards the ActorRotation.Yaw.

Since not everyone works with C++ I left a Blueprint version of the same code with some tweaks to be better accomodate to the Blueprint environment.

Lastly, the **Control Rotation** as mentioned above, is basically the camera view that the player has in-game. Both Pitch and Yaw range from 0º to 360º and have no negative values. This works fundamentally different from the Actor Rotation whereas the latter goes from 0º to 180º and to -180º. For that specific reason, extra calculations are required to align both angles. Further information on how these calculations are done, presented in the code.

## How to implement

### C++

Copy&Paste:

**YourClassName.h**

**Please don't forget to change the name of the Class from this code. It's probably not 'YourClassName', I guess?**

```cpp
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "YourClassName.generated.h"

/**
 * This is just a standard sample code.
 * 
 * It is to be expected that the user only copies what is necessary.
 * 
 * The entire layout of the Class.h/.cpp exist only to help the user understand where each block of code goes to.
 */
UCLASS()
class YOURPROJECT_API YourClassName : public APlayerController
{
	GENERATED_BODY()

protected:
  AYourClassName(); // Constructor

  virtual void Tick(float DeltaTime) override; // Ticks every frame

  virtual void BeginPlay() override; // Called after Class is spawned in the world

  virtual void SetupInputComponent() override; // Usually called before BeginPlay() to set Player Input

  // Where the magic happens
  void CameraReset(float InDeltaTime);

  // Usually there's a variable here but you can also place it on top of the .cpp class if it's not gonna be used outside of this Classe's scope.
  // bCameraReset = false;
  
};
```

**YourClassName.cpp**

```cpp
// Fill out your copyright notice in the Description page of Project Settings.

#include "YourClassName.h"

// Comment or remove this variable if you intend to use the one within .h
bCameraReset = false;

AYourClassName::AYourClassName()
{
  // . . .
}

void AYourClassName::BeginPlay()
{
	Super::BeginPlay();

}

// Part 1: Important code here
void AYourClassName::Tick(float DeltaTime)
{
	if (bResetCamera)
	{
		CameraReset(DeltaTime);
	}
}

void AYourClassName::SetupInputComponent()
{
	Super::SetupInputComponent();
  
}

// Part 2:: Important code here
void AYourClassName::CameraReset(float InDeltaTime)
{
  	/*
	* This is one way of solving this problem
  * Feel free to utilize a different solution and share! <3
	*/

  // First checks if the Paw has already spawned or not
  // Really important to avoid crashing
	if (GetPawn())
	{
    // Gets the Rotation Yaw and Rounds it (essentially, eliminating small decimental numbers like 0.123781f)
		float ActorRotationYaw = GetPawn()->GetActorRotation().Yaw;
		ActorRotationYaw = FMath::RoundToFloat(ActorRotationYaw);

    // Now gets the ControlRotation Pitch/Yaw
		float CurrentControlRotationPitch = GetControlRotation().Pitch;
		float CurrentControlRotationYaw = GetControlRotation().Yaw;

    // Also rounds both values to remove extra decimenal numbers
    CurrentControlRotationPitch = FMath::RoundToFloat(CurrentControlRotationPitch);
		CurrentControlRotationYaw = FMath::RoundToFloat(CurrentControlRotationYaw);

    // Define and initialize an ErrorTolerance to be used later
		float ErrorTolerance = 1.0f;

    /* Now checks if the Actor Rotation Yaw is below 180º
    * This essentially means that this value is currently a negative value (somewhereb between 0 and -180)
    * This calculation essentially removes that negative value and adds 180 on top of that
    */ Then combined with the actual value of the Rotation Yaw, this number becomes 180 till 360
		if (ActorRotationYaw < 0.0f)
		{
			ActorRotationYaw = ActorRotationYaw + (180 * 2);
		}

    /* Here both Yaw's are being checked if they are nearly equal to each other, by an error tolerance
    * The error tolerance (example, 1.0f) means that, if they are nearly equal each other by an extra 1.0f
    * That is, if CurrentControlRotationYaw is 200 and ActorRotationYaw is 199, then this statement is 'true'
    */ If the error tolerance would be 2.0f, then 200 to 198 would make this statement as true, but if it were 200 to 199 it would still be false
		if (FMath::IsNearlyEqual(CurrentControlRotationYaw, ActorRotationYaw, ErrorTolerance))
		{
      // The boolean that controls this whole logic is now false and this function will no longer be called.
			bCameraReset = false; // Sets to false to prevent 'Tick' from triggering this function any further
		}
		else
		{
      // And for as long as bCameraReset is 'true', then this logic will run
			ControlRotation = FMath::RInterpTo(GetControlRotation(), FRotator(0.0f, ActorRotationYaw, 0.0f), InDeltaTime, 10.0f);

			// Alternative with a Constant Interpolation Speed instead of Strong Start/Ease Out
			//ControlRotation = FMath::RInterpConstantTo(GetControlRotation(), FRotator(0.0f, ActorRotationYaw, 0.0f), 1.0f, 10.0f);
		}
	}
}
```

### Blueprint

To be added later . . .

## Conclusion & Notes

Well, that is all for now!

Enjoy this beautiful piece of code and see you next time!

-- CranchyDev

Note_1: This feature only works with Third Person Cameras. If you try to do this with a First Person Camera nothing will happen, because the Actor Rotation, both Pitch and Yaw, are aligned with the Camera's Rotation (or the Control Rotation).
