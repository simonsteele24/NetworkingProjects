// Fill out your copyright notice in the Description page of Project Settings.

#include "RN4UE4_Sample.h"
#include "StaticReplicationActor.h"


// Sets default values
AStaticReplicationActor::AStaticReplicationActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AStaticReplicationActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AStaticReplicationActor::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

