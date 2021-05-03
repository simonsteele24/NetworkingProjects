// ********************************************
// Title : Static Replication Actor
// Author : Simon Steele & Chuntao Lin
// Description : This Actor is for any non-player objects that need to be replicated.
//              Whatever blueprint inherits this will always have its transform saved
// ********************************************

#pragma once

#include "GameFramework/Actor.h"
#include "StaticReplicationActor.generated.h"

UCLASS()
class RN4UE4_SAMPLE_API AStaticReplicationActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AStaticReplicationActor();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) int actorNumber; // Represents the actor number for replication
	
};
