// Fill out your copyright notice in the Description page of Project Settings.

#include "RN4UE4_Sample.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include "ReplicationActor.h"
#include "MyLobby.h"

// Sets default values
AMyLobby::AMyLobby()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMyLobby::BeginPlay()
{
	Super::BeginPlay();
	StartLobby();
}

// Called every frame
void AMyLobby::Tick( float DeltaTime )
{
	Super::Tick(DeltaTime);
}

void AMyLobby::StartLobby()
{
	numOfPlayers = 0;
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Loading Lobby!"));
}

