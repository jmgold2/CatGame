// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlayObj.generated.h"

UCLASS()
class CATGAME_API APlayObj : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APlayObj();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//mesh for actor
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* mesh;

	//physical model for actor
	UPROPERTY(EditAnywhere)
	USceneComponent* actualComp;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
