# UE5-Third-Person-Camera-Reset
Short and simple Camera Reset mechanic that modifies the Control Rotation Yaw towards the Actor's Rotation Yaw through RInterpTo/RConstantInterpTo (you can choose). Pitch is default 0.0f but can be Actor's Rotation Pitch instead. C++ and Blueprint available!

## Introduction
Hi! Welcome to yet another one of my repositories!

This time, I present to you a very "simple" Camera Reset mechanic/feature (this wasn't so simple when I had to stay up till 03:00AM though) that, basically, sets the Control Rotation (will explain what this is in a bit), 
towards wherever the Actor is facing (if facing East, then east you go!).

To further emphasize this, imagine the following:
- The Character you are utilizing in-game is facing slightly towards left, towards a lake.
- Your Camera (what you are seeing on screen) is pointing slightly towards right, let's say, to a nearby tree.
- By tapping an Input (Keyboard/Gamepad/VR/Etc.), the Camera essentially interpolates (through an RInterpTo) within a certain speed (that can be changed) towards the specific angle that the Character's facing.

The code presented here allows you to choose the Pitch and/or Yaw. Usually the Control Rotation Pitch will be set to 0.0f, though you can easily swap it for the Actor Rotation Pitch, and also modify the Control Rotation Yaw towards the Actor Rotation Yaw.

Since not everyone works with C++, I left a Blueprint version of the same code with some tweaks to better accommodate to the Blueprint environment.

Lastly, the **Control Rotation** as mentioned above, is basically the camera view that the player has in-game. 

**Both Pitch and Yaw range from 0º to 360º and have no negative values.** 

**This works fundamentally different from the Actor Rotation.**

**The rotation of an Actor only ranges from 0º to 180º and drops down to -180º.**

Once it reaches 180º, becoming -180º, -179º, -178º, etc, and for this specific reason, extra calculations are required to align both angles.

**This concept is kinda hard to understand, so... Feel free to ask any questions if you don't understand something! ❤️** 

Further information on how these calculations 

## How to implement

### C++

> [!NOTE]
> **I am assuming that you are utilizing a PlayerController for this code. If using Character instead, some parts of the setup are a bit different.**
> **Check the corresponding files within this repository.**
> 
> **Also make sure to change YOURPROJECT_API to THENAMEOFYOURPROJECT_API**
> 
> **And change YourClassName to the name of your Class.**

<hr>

#### **Short Code template if you're in a hurry ;)**

##### **YourClassName.cpp**

```cpp
void AYourClassName::CameraReset(float InDeltaTime)
{
	if (GetPawn())
	{
		float ActorRotationYaw = GetPawn()->GetActorRotation().Yaw;
		ActorRotationYaw = FMath::RoundToFloat(ActorRotationYaw);

		float CurrentControlRotationPitch = GetControlRotation().Pitch;
		float CurrentControlRotationYaw = GetControlRotation().Yaw;

    	CurrentControlRotationPitch = FMath::RoundToFloat(CurrentControlRotationPitch);
		CurrentControlRotationYaw = FMath::RoundToFloat(CurrentControlRotationYaw);

		float ErrorTolerance = 1.0f;

		if (ActorRotationYaw < 0.0f)
		{
			ActorRotationYaw = ActorRotationYaw + (180 * 2);
		}

		if (FMath::IsNearlyEqual(CurrentControlRotationYaw, ActorRotationYaw, ErrorTolerance))
		{
			bCameraReset = false; // Sets to false to prevent 'Tick' from triggering this function any further
		}
		else
		{
			ControlRotation = FMath::RInterpTo(GetControlRotation(), FRotator(0.0f, ActorRotationYaw, 0.0f), InDeltaTime, 10.0f);
			// Alternative with a Constant Interpolation Speed instead of Strong Start/Ease Out
			// ControlRotation = FMath::RInterpConstantTo(GetControlRotation(), FRotator(0.0f, ActorRotationYaw, 0.0f), 1.0f, 10.0f);
		}
	}
}
```
<hr>

#### Long code template if you want or need the extra explanations

##### **YourClassName.h**

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
class YOURPROJECT_API AYourClassName : public APlayerController
{
	GENERATED_BODY()

protected:
  AYourClassName(); // Constructor

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
  * then having it here is the better option. Plus if you need/want it exposed to Blueprints.
  * 
  */
  // UPROPERTY(BlueprintReadWrite, Category = "Camera")
  // bool bCameraReset = false;
  
};
```

##### **YourClassName.cpp**

```cpp
// Fill out your copyright notice in the Description page of Project Settings.

#include "YourClassName.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "InputAction.h"

// Comment or remove this variable if you intend to use the one within .h
bool bCameraReset = false;

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

	// This is just a quick way of setting up a Key Binding.
	// Make sure you're using a more robust system if you're actually building a game with this code!
	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (EnhancedInputLocalPlayerSubsystem* EILPS = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer))
		{
			UInputMappingContext* IMC_Default;
			UInputAction* IA_CameraReset;

			// NewObject here essentially creates an instance of this IMC (InputMappingContext) and assigns a memory address to it.
			IMC_Default = NewObject<UInputMappingContext>(this, UInputMappingContext::StaticClass(), TEXT("IMC Default"));

			if (IMC_Default)
			{
				// Sets the IMC_Default as the first Input Mapping Context.
				EILPS->AddMappingContext(IMC_Default, 0);

				// Importantly verifies if the InputComponent (check InputComponent.h/.cpp for further information) is
				// of the type 'EnhancedInputComponent', which is a more advanced version of the 'InputComponent'.
				if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent))
				{
					// Binds a key to a specific Input Action within the IMC.
					// Note that EKeys could be any key on your keyboard, mouse, VR set, gamepad, phone, etc.
					// Type EKeys:: within the editor and you'll see the whole list of possible Input Keys.
					IMC_Default->MapKey(IA_CameraReset, EKeys::R);

					// Lastly, sets InputAction within the Enhanced Input Component,
					// with all the parameters defined above automatically.
					EIC->BindAction(IA_CameraReset, ETriggerEvent::Started, this, &AYourClassName::SetCameraResetTrue);
				}
			}
		}
	}
}

// Part 2:: Important code here
void AYourClassName::CameraReset(float InDeltaTime)
{
  	/*
	* This is one way of solving this problem
    * Feel free to utilize a different solution!
	* 
	* Don't forget to share it if you feel comfortable with that! <3
	*/

  // First checks if the Paw has already spawned or not.
  // Really important to avoid crashing.
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

    /*
	* Now checks if the Actor Rotation Yaw is below 180º.
    * This essentially means that this value is currently a negative value (somwhere between 0 and -180).
    * This calculation essentially removes that negative value and adds 180 on top of that.
	* Then combined with the actual value of the Rotation Yaw, this number becomes 180 till 360.
    */ 
		if (ActorRotationYaw < 0.0f)
		{
			ActorRotationYaw = ActorRotationYaw + (180 * 2);
		}

    /*
	* Here both Yaw's are being checked if they are nearly equal to each other.
	* 
    * The error tolerance works this way:
	* If the error tolerance is 1.0f
    * And if CurrentControlRotationYaw is 200 and ActorRotationYaw is 199,
	* then this statement is **true**.
	* 
	* If the error tolerance is 2.0f , then 200 to 198 would make this statement as true.
	* And if error tolerance is 3.0f, then 200 to 197 is true.
	* So on and so forth.
    */  
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

void AYourClassName::SetCameraResetTrue()
{
	bCameraReset = true;
}
```

### Blueprint

To be added later . . .

## Conclusion & Notes

Well, that is all for now!

Enjoy this beautiful piece of code and see you next time!

-- CranchyDev

Note_1: This feature only works with Third Person Cameras. 
		If you try to do this with a First Person Camera nothing will happen, because the Actor Rotation, both Pitch and Yaw, are aligned with the Camera's Rotation (or the Control Rotation).
