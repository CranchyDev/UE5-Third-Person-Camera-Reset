// Fill out your copyright notice in the Description page of Project Settings.

#include "YourCharacter.h"

#include "Components/CapsuleComponent.h"

// Comment or remove this variable if you intend to use the one within .h
bool bCameraReset = false;

AYourCharacter::AYourCharacter()
{
	// Create and Initialize Spring Arm
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->SetRelativeLocation(FVector(0.0f, 0.0f, 100.f));

	// Create and Initialize Camera
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	// Makes CapsuleComponent visible to help understanding where the Character currently is
	GetCapsuleComponent()->bHiddenInGame = false;

	// Sets the Control Rotation to not affect this Actor
	// This is a VERY important setup so that you can see the CameraReset in action
	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationYaw = false;
}

void AYourCharacter::BeginPlay()
{
	Super::BeginPlay();

}

// Part 1: Important code here
void AYourCharacter::Tick(float DeltaTime)
{
	if (bCameraReset)
	{
		CameraReset(DeltaTime);
	}
}

void AYourCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// This is just a quick way of setting up a Key Binding.
	// Make sure you're using a more robust system if you're actually building a game with this code!

	// Extra line of code because you need to access the ULocalPlayer, which is part of the PlayerController class.
	APlayerController* PC = Cast<APlayerController>(GetController());

	if (PC)
	{
		if (ULocalPlayer* LP = PC->GetLocalPlayer())
		{
			// If IMC_Default does not have any prior implementation from Blueprints then add them here.
			if (IMC_Default == nullptr)
			{
				// NewObject here essentially creates an instance of this IMC (InputMappingContext) and assigns a memory address to it.
				IMC_Default = NewObject<UInputMappingContext>(this, UInputMappingContext::StaticClass(), TEXT("IMC Default"));
			}

			// Adds IA_Action if it wasn't previously setup from Blueprints
			if (IA_CameraReset == nullptr)
			{
				IA_CameraReset = NewObject<UInputAction>(this, UInputAction::StaticClass(), TEXT("R"));
			}

			// Extraordinaly also creating two more InputAction to allow you to test this mechanic more laxxingly.
			if (IA_MoveForward == nullptr)
			{
				IA_MoveForward = NewObject<UInputAction>(this, UInputAction::StaticClass(), TEXT("IA_MoveForward"));
			}
			if (IA_MoveLeft == nullptr)
			{
				IA_MoveLeft = NewObject<UInputAction>(this, UInputAction::StaticClass(), TEXT("IA_MoveLeft"));
			}
			if (IA_MoveBackward == nullptr)
			{
				IA_MoveBackward = NewObject<UInputAction>(this, UInputAction::StaticClass(), TEXT("IA_MoveBackward"));
			}
			if (IA_MoveRight == nullptr)
			{
				IA_MoveRight = NewObject<UInputAction>(this, UInputAction::StaticClass(), TEXT("IA_MoveRight"));
			}

			if (IA_Look == nullptr)
			{
				IA_Look = NewObject<UInputAction>(this, UInputAction::StaticClass(), TEXT("Look"));
			}


			if (UEnhancedInputLocalPlayerSubsystem* EILPS = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LP))
			{
				if (IMC_Default)
				{
					// Sets the IMC_Default as the first Input Mapping Context.
					EILPS->AddMappingContext(IMC_Default, 0);

					// Importantly verifies if the InputComponent (check InputComponent.h/.cpp for further information) is
					// of the type 'EnhancedInputComponent', which is a more advanced version of the 'InputComponent'.
					if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
					{
						// Binds a key to a specific Input Action within the IMC.
						// Note that EKeys could be any key on your keyboard, mouse, VR set, gamepad, phone, etc.
						// Type EKeys:: within the editor and you'll see the whole list of possible Input Keys.
						IMC_Default->MapKey(IA_CameraReset, EKeys::R);

						// Lastly, sets the earlier InputAction (IA_CameraReset) within the Enhanced Input Component,
						// with all the parameters defined above automatically.
						EIC->BindAction(IA_CameraReset, ETriggerEvent::Started, this, &AYourCharacter::SetCameraResetTrue);

						/*
						* ------------------------
						* Extra setup for Movement
						* -------------------------
						*/
						IMC_Default->MapKey(IA_MoveForward, EKeys::W);
						IMC_Default->MapKey(IA_MoveLeft, EKeys::A);
						IMC_Default->MapKey(IA_MoveBackward, EKeys::S);
						IMC_Default->MapKey(IA_MoveRight, EKeys::D);

						EIC->BindAction(IA_MoveForward, ETriggerEvent::Triggered, this, &AYourCharacter::MoveForward);
						EIC->BindAction(IA_MoveLeft, ETriggerEvent::Triggered, this, &AYourCharacter::MoveLeft);
						EIC->BindAction(IA_MoveBackward, ETriggerEvent::Triggered, this, &AYourCharacter::MoveBackward);
						EIC->BindAction(IA_MoveRight, ETriggerEvent::Triggered, this, &AYourCharacter::MoveRight);
						/*
						* -------------------------------
						* Extra setup for Camera Rotation
						* -------------------------------
						*/

						// Set ValueType as Axis2D
						IA_Look->ValueType = EInputActionValueType::Axis2D;

						FEnhancedActionKeyMapping& MapKeyLook = IMC_Default->MapKey(IA_Look, EKeys::Mouse2D);

						UInputModifierNegate* NegateLook = NewObject<UInputModifierNegate>(IA_Look);

						NegateLook->bX = false;
						NegateLook->bY = true;
						NegateLook->bZ = false;

						MapKeyLook.Modifiers.AddUnique(NegateLook);

						EIC->BindAction(IA_Look, ETriggerEvent::Triggered, this, &AYourCharacter::Look);
					}
				}
			}
		}
	}
}

// Part 2: Important code here
void AYourCharacter::CameraReset(float InDeltaTime)
{
	/*
	* This is one way of solving this problem
	* Feel free to utilize a different solution!
	*
	* Don't forget to share it if you feel comfortable with that! <3
	*/

	// Not sure if it's incredibly important to check if there's a controller because that is rather implicit.
	// Leaving it here just in case.
	if (GetController())
	{
		// Gets the Rotation Yaw and Rounds it (essentially, eliminating small decimal numbers like 0.123781f)
		float ActorRotationYaw = GetActorRotation().Yaw;
		ActorRotationYaw = FMath::RoundToFloat(ActorRotationYaw);

		// Now gets the Control Rotation Pitch/Yaw
		float CurrentControlRotationPitch = GetController()->GetControlRotation().Pitch;
		float CurrentControlRotationYaw = GetController()->GetControlRotation().Yaw;

		// Also rounds both values to remove extra decimenal numbers
		CurrentControlRotationPitch = FMath::RoundToFloat(CurrentControlRotationPitch);
		CurrentControlRotationYaw = FMath::RoundToFloat(CurrentControlRotationYaw);

		// Define and initialize an ErrorTolerance to be used later
		float ErrorTolerance = 1.0f;

		/*
		* Now checks if the Actor Rotation Yaw is below 180º.
		* This essentially means that this value is currently a negative value (somewhere between 0 and -180).
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
			FRotator RotateToThis = FMath::RInterpTo(GetControlRotation(), FRotator(0.0f, ActorRotationYaw, 0.0f), InDeltaTime, 10.0f);
			GetController()->SetControlRotation(RotateToThis);

			// Alternative with a Constant Interpolation Speed instead of Strong Start/Ease Out
			// FRotator RotaToThisInstead = FMath::RInterpConstantTo(GetControlRotation(), FRotator(0.0f, ActorRotationYaw, 0.0f), 1.0f, 10.0f);
			// GetController()->SetControlRotation(RotateToThis);
		}
	}
}

void AYourCharacter::SetCameraResetTrue()
{
	bCameraReset = true;
}

void AYourCharacter::MoveForward()
{
	// Find out which way is forward based on camera/controller
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0.0f, Rotation.Yaw, 0.0f);

	const FVector DirectionForward = GetActorForwardVector();
	AddMovementInput(DirectionForward, 1.0f);
}

void AYourCharacter::MoveLeft()
{
	// Find out which way is forward based on camera/controller
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0.0f, Rotation.Yaw, 0.0f);

	const FVector DirectionLeft = GetActorRightVector();
	AddMovementInput(DirectionLeft, -1.0f);
}

void AYourCharacter::MoveBackward()
{
	// Find out which way is forward based on camera/controller
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0.0f, Rotation.Yaw, 0.0f);

	const FVector DirectionBackward = GetActorForwardVector();
	AddMovementInput(DirectionBackward, -1.0f);
}

void AYourCharacter::MoveRight()
{
	// Find out which way is forward based on camera/controller
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0.0f, Rotation.Yaw, 0.0f);

	const FVector DirectionRight = GetActorRightVector();
	AddMovementInput(DirectionRight, 1.0f);
}
