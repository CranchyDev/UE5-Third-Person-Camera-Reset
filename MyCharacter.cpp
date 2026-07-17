// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacter.h"

// Used specifically to call UEnhancedInputLibrary::RequestRebuildControlMappingsUsingContext
#include "EnhancedInputLibrary.h"

// Needed to access PlayerController.h within 'SetupPlayerInputComponent' and 'Look'
#include "GameFramework/PlayerController.h"

// Needed to access GetCapsuleComponent()
#include "Components/CapsuleComponent.h"

// Needed to create and setup a Static Mesh
#include "Components/StaticMeshComponent.h"

// Needed to access CharacterMovementComponent
#include "GameFramework/CharacterMovementComponent.h"

// Needed to access GetArrowComponent()
#include "Components/ArrowComponent.h"


// Sets default values
AMyCharacter::AMyCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Importantly sets some default values here to allow proper Camera Rotation and Movement
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	// Makes an inherited ArrowComponent HiddenInGame to false to help visualize the Camera Reset in action
	if (GetArrowComponent())
	{
		// This allows you to see it while playing/testing the game
		GetArrowComponent()->SetHiddenInGame(false);
	}

	// Spring Arm Component creation and setup.
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f));
	SpringArm->TargetArmLength = 400.0f;
	SpringArm->bUsePawnControlRotation = true;

	// Camera Component creation and setup.
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	// NOTE: This block of code wasn't written by me.
	// Creates a Static Mesh to help visualize movement and the Camera Reset mechanic.
	UStaticMeshComponent* CharacterCube = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Character Cube"));
	if (CharacterCube)
	{
		CharacterCube->SetupAttachment(GetCapsuleComponent());

		static ConstructorHelpers::FObjectFinder<UStaticMesh> CharacterCubeBase(TEXT("/Engine/BasicShapes/Cube.Cube"));
		if (CharacterCubeBase.Succeeded())
		{
			CharacterCube->SetStaticMesh(CharacterCubeBase.Object);
		}

		static ConstructorHelpers::FObjectFinder<UMaterial> Material(TEXT("/Engine/EditorMaterials/Cloth/CameraLitVertexColor.CameraLitVertexColor"));
		if (Material.Succeeded())
		{
			CharacterCube->SetMaterial(0, Material.Object);
		}

		// 3. Scale the cube to match the Capsule Component's dimensions
		float CapsuleHalfHeight = GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
		float CapsuleRadius = GetCapsuleComponent()->GetUnscaledCapsuleRadius();

		// Calculate scale factors (Default engine cube is 100 units wide/tall)
		float TargetZScale = (CapsuleHalfHeight * 2.0f) / 100.0f;
		float TargetXYScale = (CapsuleRadius * 2.0f) / 100.0f;

		CharacterCube->SetRelativeScale3D(FVector(TargetXYScale, TargetXYScale, TargetZScale));

		// Ensure it is centered within the capsule
		CharacterCube->SetRelativeLocation(FVector::ZeroVector);

		// Sets an inherited ArrowComponent relative location to be on top of the CapsuleHalfHeight
		GetArrowComponent()->SetRelativeLocation(FVector(0.0f, 0.0f, (CapsuleHalfHeight + 2.0f)));
	}
	// NOTE: End of block of code not written by me.
	// P.S.: I was saving time and I didn't want to investigate how to implement what is written in this block of code.
}

// Called when the game starts or when spawned
void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bCameraReset)
	{
		CameraReset(DeltaTime);
	}

	if (Timer_FinishCameraReset.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("%f"), GetWorldTimerManager().GetTimerRemaining(Timer_FinishCameraReset));
	}
}

// Called to bind functionality to input
void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
/*
	APlayerController* PC = Cast<APlayerController>(GetController());

	if ((PC) && (PC->IsLocalPlayerController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* EILPlayerSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
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
					EIC->BindAction(IA_CameraReset, ETriggerEvent::Started, this, &AMyCharacter::SetCameraResetTrue);

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

					EIC->BindAction(IA_Move, ETriggerEvent::Triggered, this, &AMyCharacter::Move);

					// Now setting IA_Look
					// Importantly setting IA_Look ValueType to Axis2D first once again.
					IA_Look->ValueType = EInputActionValueType::Axis2D;

					FEnhancedActionKeyMapping& MapKeyLook = IMC_Default->MapKey(IA_Look, EKeys::Mouse2D);

					UInputModifierNegate* NegateLook = NewObject<UInputModifierNegate>(IA_Look);

					NegateLook->bX = false;
					NegateLook->bY = true;
					NegateLook->bZ = false;

					MapKeyLook.Modifiers.AddUnique(NegateLook);

					EIC->BindAction(IA_Look, ETriggerEvent::Triggered, this, &AMyCharacter::Look);

					// Lastly setting IA_Jump
					// Once more, the default value is boolean (Digital), so we only need to use MapKey and BindAction.
					FEnhancedActionKeyMapping& MapKeyJump = IMC_Default->MapKey(IA_Jump, EKeys::SpaceBar);

					EIC->BindAction(IA_Jump, ETriggerEvent::Started, this, &AMyCharacter::JumpStart);

					// Sets the IMC_Default as the first Input Mapping Context.
					EILPlayerSubsystem->AddMappingContext(IMC_Default, 0);
				}
				else
				{
					EIC->BindAction(IA_CameraReset, ETriggerEvent::Started, this, &AMyCharacter::SetCameraResetTrue);
					EIC->BindAction(IA_Move, ETriggerEvent::Triggered, this, &AMyCharacter::Move);
					EIC->BindAction(IA_Look, ETriggerEvent::Triggered, this, &AMyCharacter::Look);
					EIC->BindAction(IA_Jump, ETriggerEvent::Started, this, &AMyCharacter::JumpStart);

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
	*/
}

// Must called only from 'Tick'.
void AMyCharacter::CameraReset(float InDeltaTime)
{
	///
	// ---------------------------------------------------------------
	// This is one way of solving this problem.
	// Feel free to utilize a different solution!
	//
	// Don't forget to share it if you feel comfortable with that! <3
	// ---------------------------------------------------------------
	///

	// First checks if there's any active Controller (most likely yes).
	// Really important to avoid crashing.
	if (GetController())
	{
		// Gets the Rotation Yaw and Rounds it (essentially, eliminating small decimal numbers like 0.123781f).
		float ActorRotationYaw = GetActorRotation().Yaw;
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
			FRotator NewRotation = FMath::RInterpTo(GetControlRotation(), FRotator(0.0f, ActorRotationYaw, 0.0f), InDeltaTime, 10.0f);
			GetController()->SetControlRotation(NewRotation);

			// Alternative with a Constant Interpolation Speed instead of Strong Start/Ease Out
			//ControlRotation = FMath::RInterpConstantTo(GetControlRotation(), FRotator(0.0f, ActorRotationYaw, 0.0f), 1.0f, 10.0f);
		}
	}
}

void AMyCharacter::SetCameraResetTrue()
{
	// Uncomment this if you prefer to use the Character setup!
	// Original is within MyPlayerController.
	// Initial trigger of the Camera Reset mechanic.
	//bCameraReset = true;

	/// -------------------------------------------------------------------------------------------------------------------
	// Starts the timer that will forcefully finish the Camera Reset mechanic.
	// This exists to prevent erratic behavior like, the loop doesn't end, or the camera doesn't reset properly.
	// --------------------------------------------------------------------------------------------------------------------
	// Please be aware that if you modify Timer_FCR_Seconds, you will most likely have to change InterpSpeed or vice versa,
	// as they both affect the speed of RInterpTo.
	/// -------------------------------------------------------------------------------------------------------------------
	//GetWorldTimerManager().SetTimer(Timer_FinishCameraReset, this, &AMyCharacter::FinishCameraReset, Timer_FCR_Seconds, false);
}

void AMyCharacter::Move(const FInputActionValue& Value)
{
	// Get and set certain values to define where the Player Character moves
	const FVector2D WhereToMove = Value.Get<FVector2D>();
	const FRotator LookYawRotation(0.0f, GetControlRotation().Yaw, 0.0f);
	const FVector ForwardBackDirection = FRotationMatrix(LookYawRotation).GetUnitAxis(EAxis::Y);
	const FVector LeftRightDirection = FRotationMatrix(LookYawRotation).GetUnitAxis(EAxis::X);

	// Importantly verifies if the Input Value isn't zero to avoid running the code unnecessarily
	// Is this what it's actually preventing? :thinking:
	if (Value.IsNonZero())
	{
		AddMovementInput(ForwardBackDirection, WhereToMove.X);
		AddMovementInput(LeftRightDirection, WhereToMove.Y);
	}
}

void AMyCharacter::Look(const FInputActionValue& Value)
{
	// Checks if there's any PlayerController possessing this Pawn
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC)
	{
		return;
	}

	// Vector2D: X is Mouse X (Yaw), Y is Mouse Y (Pitch)
	const FVector2D LookAxisValue = Value.Get<FVector2D>();

	// Add Yaw (Left/Right)
	PC->AddYawInput(LookAxisValue.X);

	// Add Pitch (Up/Down)
	PC->AddPitchInput(LookAxisValue.Y);
}

void AMyCharacter::JumpStart(const FInputActionValue& Value)
{
	Jump();
}

void AMyCharacter::FinishCameraReset()
{
	bCameraReset = false;
	GetWorldTimerManager().ClearTimer(Timer_FinishCameraReset);
}
