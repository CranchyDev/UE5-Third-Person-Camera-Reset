// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerController.h"

// Used specifically to call UEnhancedInputLibrary::RequestRebuildControlMappingsUsingContext
#include "EnhancedInputLibrary.h"

// Needed for the ACharacter::Jump() function to be called
#include "GameFramework/Character.h"


AMyPlayerController::AMyPlayerController()
{
	// . . .
}

void AMyPlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void AMyPlayerController::Tick(float DeltaTime)
{
	if (bCameraReset)
	{
		CameraReset(DeltaTime);
	}
}

void AMyPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();


	// Not heavily documenting this as it is not the nature of the Camera Reset mechanic.
	//Still leaving some comments here but moreover for future self me.

	if (IsLocalPlayerController())
	{
		if (UEnhancedInputLocalPlayerSubsystem* EILPlayerSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent))
			{
				if (IMC_Default == nullptr)
				{
					IMC_Default = NewObject<UInputMappingContext>(this, UInputMappingContext::StaticClass(), TEXT("IMC_Default"));
					IA_CameraReset = NewObject<UInputAction>(this, UInputAction::StaticClass(), TEXT("IA_CameraReset"));
					IA_Move = NewObject<UInputAction>(this, UInputAction::StaticClass(), TEXT("IA_Move"));
					IA_Look = NewObject<UInputAction>(this, UInputAction::StaticClass(), TEXT("IA_Look"));
					IA_Jump = NewObject<UInputAction>(this, UInputAction::StaticClass(), TEXT("IA_Jump"));

					// Mapping IA_CameraReset first.
					// Every InputAction is a boolean (Digital) by default, so we don't have to set ValueType like in other Keys.
					// We do have to, however, define which Key is going to trigger this IA
					IA_CameraReset->ValueType = EInputActionValueType::Boolean;

					FEnhancedActionKeyMapping& MapKeyReset = IMC_Default->MapKey(IA_CameraReset, EKeys::R);

					UInputModifierNegate* NegateReset = NewObject<UInputModifierNegate>(IA_CameraReset);

					NegateReset->bX = false;
					NegateReset->bY = false;
					NegateReset->bZ = false;

					MapKeyReset.Modifiers.AddUnique(NegateReset);

					// And now we BindAction onto the EnhancedInputComponent that we initialized previously.
					EIC->BindAction(IA_CameraReset, ETriggerEvent::Started, this, &AMyPlayerController::SetCameraResetTrue);

					// Now mapping Keys to IA_Move
					// Starting with defining the IA_Move ValueType (importantly to Axis2D).
					IA_Move->ValueType = EInputActionValueType::Axis2D;

					// MapKeyW Setup
					FEnhancedActionKeyMapping& MapKeyW = IMC_Default->MapKey(IA_Move, EKeys::W);
					UInputModifierSwizzleAxis* SwizzleAxisW = NewObject<UInputModifierSwizzleAxis>(IA_Move);

					SwizzleAxisW->Order = EInputAxisSwizzle::YXZ;

					MapKeyW.Modifiers.AddUnique(SwizzleAxisW);

					// MapKeyA Setup
					FEnhancedActionKeyMapping& MapKeyA = IMC_Default->MapKey(IA_Move, EKeys::A);
					UInputModifierNegate* NegateA = NewObject<UInputModifierNegate>(IA_Move);

					NegateA->bX = true;
					NegateA->bY = true;
					NegateA->bZ = true;

					MapKeyA.Modifiers.AddUnique(NegateA);

					// MapKeyS Setup
					FEnhancedActionKeyMapping& MapKeyS = IMC_Default->MapKey(IA_Move, EKeys::S);
					UInputModifierSwizzleAxis* SwizzleAxisS = NewObject<UInputModifierSwizzleAxis>(IA_Move);
					UInputModifierNegate* NegateS = NewObject<UInputModifierNegate>(IA_Move);

					SwizzleAxisS->Order = EInputAxisSwizzle::YXZ;

					NegateS->bX = true;
					NegateS->bY = true;
					NegateS->bZ = true;

					MapKeyS.Modifiers.AddUnique(SwizzleAxisS);
					MapKeyS.Modifiers.AddUnique(NegateS);

					FEnhancedActionKeyMapping& MapKeyD = IMC_Default->MapKey(IA_Move, EKeys::D);

					EIC->BindAction(IA_Move, ETriggerEvent::Triggered, this, &AMyPlayerController::Move);

					// Now setting IA_Look
					// Importantly setting IA_Look ValueType to Axis2D first once again.
					IA_Look->ValueType = EInputActionValueType::Axis2D;

					FEnhancedActionKeyMapping& MapKeyLook = IMC_Default->MapKey(IA_Look, EKeys::Mouse2D);

					UInputModifierNegate* NegateLook = NewObject<UInputModifierNegate>(IA_Look);

					NegateLook->bX = false;
					NegateLook->bY = true;
					NegateLook->bZ = false;

					MapKeyLook.Modifiers.AddUnique(NegateLook);

					EIC->BindAction(IA_Look, ETriggerEvent::Triggered, this, &AMyPlayerController::Look);

					// Lastly setting IA_Jump
					// Once more, the default value is boolean (Digital), so we only need to use MapKey and BindAction.
					FEnhancedActionKeyMapping& MapKeyJump = IMC_Default->MapKey(IA_Jump, EKeys::SpaceBar);

					EIC->BindAction(IA_Jump, ETriggerEvent::Started, this, &AMyPlayerController::Jump);

					// Sets the IMC_Default as the first Input Mapping Context.
					EILPlayerSubsystem->AddMappingContext(IMC_Default, 0);
				}
				else
				{
					EIC->BindAction(IA_CameraReset, ETriggerEvent::Started, this, &AMyPlayerController::SetCameraResetTrue);
					EIC->BindAction(IA_Move, ETriggerEvent::Triggered, this, &AMyPlayerController::Move);
					EIC->BindAction(IA_Look, ETriggerEvent::Triggered, this, &AMyPlayerController::Look);
					EIC->BindAction(IA_Jump, ETriggerEvent::Started, this, &AMyPlayerController::Jump);

					// Before AddMappingContext, we force the IMC to reconstruct all the new existing Mappings.
					// Not mandatory, but leaving it here just in case.
					UEnhancedInputLibrary::RequestRebuildControlMappingsUsingContext(IMC_Default);

					// Finally sets the IMC_Default as the first Input Mapping Context.
					// We are now ready to utilize it!
					EILPlayerSubsystem->AddMappingContext(IMC_Default, 0);
				}
			}
		}
	}
}

// Must called only from 'Tick'.
void AMyPlayerController::CameraReset(float InDeltaTime)
{
	///
	// ---------------------------------------------------------------
	// This is one way of solving this problem.
	// Feel free to utilize a different solution!
	//
	// Don't forget to share it if you feel comfortable with that! <3
	// ---------------------------------------------------------------
	///

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

		///
		// --------------------------------------------------------------------------------------------------
		// Now checks if the Actor Rotation Yaw is below 180º.
		// This essentially means that this value is currently a negative value (somewhere between 0 and -180).
		// This calculation essentially removes that negative value and adds 180 on top of that.
		// Then combined with the actual value of the Rotation Yaw, this number becomes 180 till 360.
		// --------------------------------------------------------------------------------------------------
		///
		if (ActorRotationYaw < 0.0f)
		{
			ActorRotationYaw = ActorRotationYaw + 360; // Or (180 * 2), achieves the same result.
		}

		///
		// ----------------------------------------------------------------------------------
		// Here both Yaw's are being checked if they are nearly equal to each other.
		//
		// The error tolerance works this way:
		//
		// Consider ErrorTolerance = 1.0f;
		//
		// Consider CurrentControlRotationYaw is X and ActorRotationYaw is Y.
		// If the difference between X and Y or Y and X is 1, then the statement is true.
		// The difference is calculated by doing a simple subtraction (x - y) or (y - x).
		//
		// Further example:
		// If CurrentControlRotationYaw is 200 and ActorRotationYaw is 199,
		// then this statement is true.
		//
		// If the error tolerance is 2.0f, then 200 to 198 would make this statement as true.
		// And if error tolerance is 3.0f, then 200 to 197 is true.
		//
		// So on and so forth.
		//
		// Note: these values will never be negative, so you don't have to worry about that.
		// ----------------------------------------------------------------------------------
		///
		if (FMath::IsNearlyEqual(CurrentControlRotationYaw, ActorRotationYaw, ErrorTolerance))
		{
			// The boolean that controls this whole logic is now false and this function will no longer be called.
			bCameraReset = false; // Sets to false to prevent 'Tick' from triggering this function any further
		}
		else
		{
			// And for as long as bCameraReset is 'true', then this logic will run
			ControlRotation = FMath::RInterpTo(GetControlRotation(), FRotator(0.0f, ActorRotationYaw, 0.0f), InDeltaTime, InterpSpeed);

			// Alternative with a Constant Interpolation Speed instead of Strong Start/Ease Out
			//ControlRotation = FMath::RInterpConstantTo(GetControlRotation(), FRotator(0.0f, ActorRotationYaw, 0.0f), InDeltaTime, InterpSpeed);
		}
	}
}

void AMyPlayerController::SetCameraResetTrue()
{
	// Initial trigger of the Camera Reset mechanic.
	bCameraReset = true;

	/// -------------------------------------------------------------------------------------------------------------------
	// Starts the timer that will forcefully finish the Camera Reset mechanic.
	// This exists to prevent erratic behavior like, the loop doesn't end, or the camera doesn't reset properly.
	// --------------------------------------------------------------------------------------------------------------------
	// Please be aware that if you modify Timer_FCR_Seconds, you will most likely have to change InterpSpeed or vice versa,
	// as they both affect the speed of RInterpTo.
	/// -------------------------------------------------------------------------------------------------------------------
	GetWorldTimerManager().SetTimer(Timer_FinishCameraReset, this, &AMyPlayerController::FinishCameraReset, Timer_FCR_Seconds, false);
}

void AMyPlayerController::Move(const FInputActionValue& Value)
{
	// Letting this small check exist here just in case there's currently no possessed Pawn
	if (!GetPawn())
	{
		return;
	}

	// Get and set certain values to define where the Player Character moves
	const FVector2D WhereToMove = Value.Get<FVector2D>();
	const FRotator LookYawRotation(0.0f, GetControlRotation().Yaw, 0.0f);
	const FVector ForwardBackDirection = FRotationMatrix(LookYawRotation).GetUnitAxis(EAxis::Y);
	const FVector LeftRightDirection = FRotationMatrix(LookYawRotation).GetUnitAxis(EAxis::X);

	// Importantly verifies if the Input Value isn't zero to avoid running the code unnecessarily
	// Is this what it's actually preventing? :thinking:
	if (Value.IsNonZero())
	{
		GetPawn()->AddMovementInput(ForwardBackDirection, WhereToMove.X);
		GetPawn()->AddMovementInput(LeftRightDirection, WhereToMove.Y);
	}
}

void AMyPlayerController::Look(const FInputActionValue& Value)
{
	// Vector2D: X is Mouse X (Yaw), Y is Mouse Y (Pitch)
	const FVector2D LookAxisValue = Value.Get<FVector2D>();

	// Add Yaw (Left/Right)
	AddYawInput(LookAxisValue.X);

	// Add Pitch (Up/Down)
	AddPitchInput(LookAxisValue.Y);
}

void AMyPlayerController::Jump(const FInputActionValue& Value)
{
	if (!GetPawn())
	{
		return;
	}

	ACharacter* PlayerCharacter = Cast<ACharacter>(GetPawn());

	if (PlayerCharacter)
	{
		PlayerCharacter->Jump();
	}
}

void AMyPlayerController::FinishCameraReset()
{
	bCameraReset = false;
	GetWorldTimerManager().ClearTimer(Timer_FinishCameraReset); // Clears the timer just in case.
}
