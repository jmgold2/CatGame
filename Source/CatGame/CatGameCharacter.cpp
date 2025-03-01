// Copyright Epic Games, Inc. All Rights Reserved.

#include "CatGameCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "InteractableObjects/FoodObj.h"
#include "InteractableObjects/GrabObj.h"
#include "InteractableObjects/PlayObj.h"
#include "InteractableObjects/PushObj.h"
#include "InteractableObjects/ScratchObj.h"
#include "InteractableObjects/SniffObj.h"
#include "InteractableObjects/DrinkObj.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// ACatGameCharacter

ACatGameCharacter::ACatGameCharacter(){
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	catMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CatMesh"));
	catMesh->SetupAttachment(RootComponent);

	//may have to initialize interactObj not sure yet

	// Create a camera boom (pulls in towards the player if there is a collision)
	cameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	cameraBoom->SetupAttachment(catMesh);
	cameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	cameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	followCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	followCamera->SetupAttachment(cameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	followCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void ACatGameCharacter::BeginPlay(){
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	//initialize boolean
	interactInProg = false;
}

//////////////////////////////////////////////////////////////////////////
// Input

void ACatGameCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent){
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(Move, ETriggerEvent::Triggered, this, &ACatGameCharacter::Walk);

		// Looking
		EnhancedInputComponent->BindAction(Look, ETriggerEvent::Triggered, this, &ACatGameCharacter::MoveCamera);

		//interacting with objects in the world
		EnhancedInputComponent->BindAction(Interact, ETriggerEvent::Started, this, &ACatGameCharacter::PickUp);
		/*
		* Binding this should be uneccessary
		* EnhancedInputComponent->BindAction(Interact, ETriggerEvent::Completed, this, &ACatGameCharacter::Drop);
		*/
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ACatGameCharacter::Walk(const FInputActionValue& Value){
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ACatGameCharacter::MoveCamera(const FInputActionValue& Value){
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

//start here while determing the type of object being interacted with then send to the correct action
void ACatGameCharacter::InitInteract(const FInputActionValue& Value) {
	if (interactInProg == false) {
		//draw line from cat in direction of the camera and identify class of object we want to interact with to trigger approriate function
		start = catMesh->GetComponentLocation();
		forwardVector = followCamera->GetForwardVector();
		end = (forwardVector * 100.0f) + start;

		if (GetWorld()->LineTraceSingleByChannel(hit, start, end, ECC_Visibility, defaultCompQueryParams, defaultResponseParams)) {
			//send to the desired function
			if (hit.GetActor()->GetClass()->IsChildOf(APushObj::StaticClass()) ) {

			}
			else if (hit.GetActor()->GetClass()->IsChildOf(ADrinkObj::StaticClass())) {

			}
			else if (hit.GetActor()->GetClass()->IsChildOf(AScratchObj::StaticClass())) {

			}
			else if (hit.GetActor()->GetClass()->IsChildOf(AFoodObj::StaticClass())) {

			}
			else if (hit.GetActor()->GetClass()->IsChildOf(AGrabObj::StaticClass())) {

			}
			else if (hit.GetActor()->GetClass()->IsChildOf(APlayObj::StaticClass())) {

			}
			else if (hit.GetActor()->GetClass()->IsChildOf(ASniffObj::StaticClass())) {

			}
		}
	}

}

void ACatGameCharacter::PickUp(const FInputActionValue& Value) {
	//call drop if already holding something
	if (interactInProg) {
		Drop(Value);
	}
	//otherwise pickup as normal
	else {

	}
}

void ACatGameCharacter::Drop(const FInputActionValue& Value) {

}