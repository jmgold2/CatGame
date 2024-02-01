// Fill out your copyright notice in the Description page of Project Settings.


#include "GrabObj.h"

// Sets default values
AGrabObj::AGrabObj()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AGrabObj::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGrabObj::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

