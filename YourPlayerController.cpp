// Fill out your copyright notice in the Description page of Project Settings.

#include "YourPlayerController.h"

// Comment or remove this variable if you intend to use the one within .h
bool bCameraReset = false;

AYourPlayerController::AYourPlayerController()
{
	// . . .
}

void AYourPlayerController::BeginPlay()
{
	Super::BeginPlay();

}

// Part 1: Important code here
void AYourPlayerController::Tick(float DeltaTime)
{
	if (bCameraReset)
	{
		CameraReset(DeltaTime);
	}
}

void AYourPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// This is just a quick way of setting up a Key Binding.
	// Make sure you're using a more robust system if you're actually building a game with this code!
	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		// If IMC_Default does not have any prior implementation from Blueprints then add them here.
		if (IMC_Default == nullptr)
		{
			// NewObject here essentially creates an instance of this IMC (InputMappingContext) and assigns a memory address to it.
			IMC_Default = NewObject<UInputMappingContext>(this, UInputMappingContext::StaticClass(), TEXT("IMC Default"));
		}
		
		if (UEnhancedInputLocalPlayerSubsystem* EILPS = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer))
		{
			if (IMC_Default)
			{
				// Sets the IMC_Default as the first Input Mapping Context.
				EILPS->AddMappingContext(IMC_Default, 0);

				// Adds IA_Action if it wasn't previously setup from Blueprints
				if (IA_CameraReset == nullptr)
				{
					IA_CameraReset = NewObject<UInputAction>(this, UInputAction::StaticClass(), TEXT("R"));
				}
				
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
					EIC->BindAction(IA_CameraReset, ETriggerEvent::Started, this, &AYourPlayerController::SetCameraResetTrue);
				}
			}
		}
	}
}

// Part 2: Important code here
void AYourPlayerController::CameraReset(float InDeltaTime)
{
	/*
	* This is one way of solving this problem.
	* Feel free to utilize a different solution!
	*
	* Don't forget to share it if you feel comfortable with that! <3
	*/

	// First checks if the Pawn has already spawned or not.
	// Really important to avoid crashing.
	if (GetPawn())
	{
		// Gets the Rotation Yaw and Rounds it (essentially, eliminating small decimal numbers like 0.123781f).
		float ActorRotationYaw = GetPawn()->GetActorRotation().Yaw;
		ActorRotationYaw = FMath::RoundToFloat(ActorRotationYaw);

		// Now gets the ControlRotation Pitch/Yaw.
		float CurrentControlRotationPitch = GetControlRotation().Pitch;
		float CurrentControlRotationYaw = GetControlRotation().Yaw;

		// Also rounds both values to remove extra decimenal numbers.
		CurrentControlRotationPitch = FMath::RoundToFloat(CurrentControlRotationPitch);
		CurrentControlRotationYaw = FMath::RoundToFloat(CurrentControlRotationYaw);

		// Define and initialize an ErrorTolerance variable to be used later.
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
		*
		* ErrorTolerance = 1.0f;
		*
		* Consider CurrentControlRotationYaw is **X** and ActorRotationYaw is **Y**.
		* If the difference between X and Y or Y and X is 1, then the statement is **true**.
		* The difference is calculated by doing a simple subtraction (x - y) or (y - x).
		*
		* Further example:
		* If CurrentControlRotationYaw is 200 and ActorRotationYaw is 199,
		* then this statement is **true**.
		*
		* If the error tolerance is 2.0f, then 200 to 198 would make this statement as true.
		* And if error tolerance is 3.0f, then 200 to 197 is true.
		*
		* So on and so forth.
		*
		* Note: these values will never be negative, so you don't have to worry about that.
		*
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

void AYourPlayerController::SetCameraResetTrue()
{
	bCameraReset = true;
}
