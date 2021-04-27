// Fill out your copyright notice in the Description page of Project Settings.

#include "RN4UE4_Sample.h"
#include "ReplicationActor.h"


// Sets default values
AReplicationActor::AReplicationActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AReplicationActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AReplicationActor::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

