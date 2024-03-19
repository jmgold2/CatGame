// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "CatGameCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class ACatGameCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* cameraBoom;

	/** follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* followCamera;

	UPROPERTY(VisibleAnywhere, Category = Mesh)
	USkeletalMeshComponent* catMesh;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* Move;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* Interact;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* Look;

public:
	ACatGameCharacter();
	

protected:

	/** Called for movement input */
	void Walk(const FInputActionValue& Value);

	/** Called for looking input */
	void MoveCamera(const FInputActionValue& Value);

	/* Called when interacting with a grabbable object*/
	void PickUp(const FInputActionValue& Value);

	/* Called when a grabbable object is interacted with while already holding something*/
	void Drop(const FInputActionValue& Value);

	/* Called for initial interact before object type determines future action */
	void InitInteract(const FInputActionValue& Value);

	//checks if character alreadu interacting with something
	bool interactInProg;

	//actor to interact with
	UPROPERTY(EditAnywhere)
	class AActor* interactObj;

	//used in to find actor for interact
	FVector start;
	FVector forwardVector;
	FVector end;
	FHitResult hit;
	FComponentQueryParams defaultCompQueryParams;
	FCollisionResponseParams defaultResponseParams;
			

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();

public:
	/** Returns cameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return cameraBoom; }
	/** Returns followCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return followCamera; }
	/** Returns catMesh subobject **/
	FORCEINLINE class USkeletalMeshComponent* GetMesh() const { return catMesh; }
};

