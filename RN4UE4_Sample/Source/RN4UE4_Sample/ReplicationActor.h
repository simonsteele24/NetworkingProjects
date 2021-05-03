// ********************************************
// Title : Replication Actor
// Author : Simon Steele & Chuntao Lin
// Description : This Actor is basically the player pawn for each player.
//              Whenever a player puts in movement input, it moves this object
// ********************************************

#pragma once

#include "GameFramework/Actor.h"
#include "ReplicationActor.generated.h"

UCLASS()
class RN4UE4_SAMPLE_API AReplicationActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AReplicationActor();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	/* Public Vars */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int playerNum = 0; // Represents the player number for the player controlling this actor
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bIsOwner = false; // Represents if this actor is on the host client
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bShouldJump = false; // Represents if the actor should jump. Essentially a flag to check if a jump force is required
};
