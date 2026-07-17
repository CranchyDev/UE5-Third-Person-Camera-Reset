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

- **Both Pitch and Yaw range from 0º to 360º and have no negative values.** 

- **This works fundamentally different from the Actor Rotation.**

- **The rotation Yaw of an Actor only ranges from 0º to 180º and drops down to -180º.**

- **Once it reaches 180º, becoming -180º, -179º, -178º, etc, and for this specific reason, extra calculations are required to align both angles.**

This concept is kinda hard to understand, so... Feel free to ask any questions if you don't understand something! ❤️

Further information on how these calculations are implemented, can be found within the corresponding files of this repository.

## How to implement

### C++

> [!NOTE]
> **C++ warning:**
> 
> **There's an option for the same mechanic to exist either within MyCharacter or MyPlayerController.**
> 
> **If you prefer to control everything with MyPlayerController, then do not uncomment certain parts within MyCharacter, namely:**
> 
> - **SetupPlayerInputComponent.**
> - **SetCameraResetTrue.**
> 
> **However, if you prefer to control everything with MyCharacter, then comment out the necessary code within MyPlayerController.**
> 
> **The default is to use MyPlayerController (set within SamepleGameMode.cpp).**
> 
> **For further information, check the corresponding files within this repository.**
> 
> **Also make sure to change YOURPROJECT_API to THENAMEOFYOURPROJECT_API, in case you are 'Copy&Pasting'.**
> **And change YourClassName to the name of your Class.**

<hr>

### **Short Code template if you're in a hurry ;)**

#### **MyPlayerController.cpp**

```cpp
// Must called only from 'Tick'.
void AMyPlayerController::CameraReset(float InDeltaTime)
{
	///
	// ---------------------------------------------------------------
	// This is one way of solving this problem.
	//
	// Feel free to utilize a different solution!
	//
	// Don't forget to share it if you feel comfortable with that! <3
	// ---------------------------------------------------------------
	///

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
			ActorRotationYaw = ActorRotationYaw + 360; // Or (180 * 2), achieves the same result.
		}

		if (FMath::IsNearlyEqual(CurrentControlRotationYaw, ActorRotationYaw, ErrorTolerance))
		{
			// The boolean that controls this whole logic is now false and this function will no longer be called.
			bCameraReset = false; // Sets to false to prevent 'Tick' from triggering this function any further
		}
		else
		{
			ControlRotation = FMath::RInterpTo(GetControlRotation(), FRotator(0.0f, ActorRotationYaw, 0.0f), InDeltaTime, InterpSpeed);

			// ControlRotation = FMath::RInterpConstantTo(GetControlRotation(), FRotator(0.0f, ActorRotationYaw, 0.0f), InDeltaTime, InterpSpeed);
		}
	}
}
```
<hr>

#### Link to each respective file

##### [**YourPlayerController.h**](YourPlayerController.h)
##### [**YourPlayerController.cpp**](YourPlayerController.cpp)
##### [**YourCharacter.h**](YourCharacter.h)
##### [**YourCharacter.cpp**](YourCharacter.cpp)

### Blueprint

#### Step 1:
<img width="1257" height="762" alt="Screenshot1" src="https://github.com/user-attachments/assets/824684db-479c-44dc-b93e-c819fd4243ae" />


#### Step 2:
<img width="1335" height="886" alt="Screenshot2" src="https://github.com/user-attachments/assets/1d3de5b0-a27d-4a9c-8fa6-cda1189ebd35" />


#### Step 3:
<img width="1860" height="863" alt="Screenshot3" src="https://github.com/user-attachments/assets/7741ecf0-d833-47d8-a4a5-14e81f12e8b4" />


## Conclusion & Notes

Well, that is all for now!

Enjoy this beautiful piece of code and see you next time!

-- CranchyDev

Note_1: This feature only works with Third Person Cameras. 
		If you try to do this with a First Person Camera nothing will happen, because the Actor Rotation, both Pitch and Yaw, are aligned with the Camera's Rotation (or the Control Rotation).
