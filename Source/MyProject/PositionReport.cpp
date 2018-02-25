// Fill out your copyright notice in the Description page of Project Settings.

#include "PositionReport.h"

#include "MyProject.h"
#include "Gameframework/Actor.h"
#include "Engine/World.h"

// Sets default values for this component's properties
UPositionReport::UPositionReport()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

}

// Called when the game starts
void UPositionReport::BeginPlay()
{
	Super::BeginPlay();  //this means do whatever is up the stream, up the inheritance tree

	//dynamically sizable string, a pointer to a place in memory that contains what we want, Get
	//Pointer to ObjectName is used because UE LOG is expecting a TCHAR array and the pointer effectively converts the fstring to this type
	FString ObjectName = GetOwner()->GetName();  //Use -> to look through the pointer to get the Getname method and store the variable
	FString ObjectPos = GetOwner()->GetTransform().GetLocation().ToString(); //Detecting the location of our tracked objects using . because its not a pointer
	UE_LOG(LogTemp, Warning, TEXT("%s is at %s"), *ObjectName, *ObjectPos); //Macro, Warning prints color based off warning/error/log - %s represents the object in the text as format operator - When doing a log like this dont forget the dereferencer

}


// Called every frame
void UPositionReport::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

