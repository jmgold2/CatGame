// Fill out your copyright notice in the Description page of Project Settings.


#include "ScratchObj.h"

// Sets default values
AScratchObj::AScratchObj()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AScratchObj::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AScratchObj::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

