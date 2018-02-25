// Fill out your copyright notice in the Description page of Project Settings.

#include "Grabber.h"

#include "Engine/World.h"
#include "Gameframework/Actor.h"
#include "Components/InputComponent.h"
#include "CollisionQueryParams.h"
#include "Components/PrimitiveComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "GameFramework/PlayerController.h"
#include "DrawDebugHelpers.h"
#include "MyProject.h"

#define OUT

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();
	FindPhysicsHandleComponent();
	SetupInputComponent();
}

void UGrabber::FindPhysicsHandleComponent()
{
	///Look for attached Physics Handle, using <blahblah> which is a generic signature, will work for multiple classes, will go find the physhandle by class
	PhysicsHandle = GetOwner()->FindComponentByClass <UPhysicsHandleComponent>();
	if (PhysicsHandle == nullptr)
	{
		//Phys handle is found
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("%s missing physics handle component"), *GetOwner()->GetName())
	}
}

///Look for attached Input Component, only appears at runtime, will work for multiple classes, will go find the physhandle by class
void UGrabber::SetupInputComponent()
{
	InputComponent = GetOwner()->FindComponentByClass <UInputComponent>();
	if (InputComponent)
	{
		InputComponent->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab); ///Detects key input, "this" is a reference to the grabber component, points to function ugrabber
		InputComponent->BindAction("Grab", IE_Released, this, &UGrabber::GrabRelease);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("%s missing Input component"), *GetOwner()->GetName())
	}
}

void UGrabber::Grab()
{

		///Line trace and see if we and reach any actors with physics body collision channel set
		auto HitResult = GetFirstPhysicsBodyInReach();
		auto ComponentToGrab = HitResult.GetComponent(); //What part of the object to grab, in this case the mesh
		auto ActorHit = HitResult.GetActor();

	///if we hit something then attach a physics handle
	if (ActorHit && (PhysicsHandle != nullptr)) 
		{
		PhysicsHandle->GrabComponentAtLocationWithRotation(
			ComponentToGrab,
			NAME_None, // means no "bones" of the object needed
			ComponentToGrab->GetOwner()->GetActorLocation(),
			FRotator(0)
		);
	}
}

void UGrabber::GrabRelease() 
{
	if (!PhysicsHandle) { return; }
	PhysicsHandle->ReleaseComponent();
}

// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!PhysicsHandle) { return; } ///if forced to leave, the rest wont be called
	//If the physics handle is attached
	if (PhysicsHandle->GrabbedComponent)

	{
		//move the object that we're holding
		PhysicsHandle->SetTargetLocation(GetReachLineEnd());
	}

}

FHitResult UGrabber::GetFirstPhysicsBodyInReach() const
{
	//setup query paramaters
	FCollisionQueryParams TraceParameters(FName(TEXT("")), false, GetOwner());   //False here makes it becomes a simple collider, the actor to ignore, which here is Owner So trace doesnt begin inside actor - If you wanted intersect, then change to true

	 ///Line trace (ray-cast)
	FHitResult HitResult;
	GetWorld()->LineTraceSingleByObjectType(
		OUT HitResult,
		GetReachLineStart(),
		GetReachLineEnd(),
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody), // struct that contains list of objects the query is interested in - ECollisionchannel is an Enum
		TraceParameters
	);

	///See what is hit
	AActor* ActorHit = HitResult.GetActor();
	if (ActorHit) //protection method, if actor hit, makes sure it's not associated with nothing, if it is, it will skip this code.
	{
		UE_LOG(LogTemp, Warning, TEXT("Line trace hit: %s"), *ActorHit->GetName())
	}
	return HitResult;
}

FVector UGrabber::GetReachLineEnd() const
{
	/// get player viewpoint this gametick
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewPointLocation, //Unreal requests these to be refered to as OUT, therefore setting a macro at the top to automatically define it for me. Read documentation
		OUT PlayerViewPointRotation
		);

	return PlayerViewPointLocation; // Define the reach of said vector (length)
}

FVector UGrabber::GetReachLineStart() const
{
	/// get player viewpoint this gametick
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewPointLocation, //Unreal requests these to be refered to as OUT, therefore setting a macro at the top to automatically define it for me. Read documentation
		OUT PlayerViewPointRotation
	);

	return PlayerViewPointLocation + PlayerViewPointRotation.Vector() * Reach; // Define the reach of said vector (length)
}