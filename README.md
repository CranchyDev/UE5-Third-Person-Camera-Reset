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
> **I am assuming that you are utilizing a PlayerController for this code. If using Character instead, some parts of the setup are a bit different.**
> **Check the corresponding files within this repository.**
> 
> **Also make sure to change YOURPROJECT_API to THENAMEOFYOURPROJECT_API**
> 
> **And change YourClassName to the name of your Class.**

<hr>

#### **Short Code template if you're in a hurry ;)**

##### **YourPlayerController.cpp**

```cpp
void AYourPlayerController::CameraReset(float InDeltaTime)
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

#### Link to each respective file

##### **YourPlayerController.h [insertlinkhere]**
##### **YourPlayerController.cpp [insertlinkhere]**
##### **YourCharacter.h [insertlinkhere]**
##### **YourCharacter.cpp [insertlinkhere]**

**Please don't forget to change the name of the Class from this code. It's probably not 'YourPlayerController' and 'YourCharacter', I guess?**

### Blueprint

To be added later . . .

## Conclusion & Notes

Well, that is all for now!

Enjoy this beautiful piece of code and see you next time!

-- CranchyDev

Note_1: This feature only works with Third Person Cameras. 
		If you try to do this with a First Person Camera nothing will happen, because the Actor Rotation, both Pitch and Yaw, are aligned with the Camera's Rotation (or the Control Rotation).
