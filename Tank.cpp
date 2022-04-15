// Fill out your copyright notice in the Description page of Project Settings.


#include "Tank.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Kismet/GameplayStatics.h" 
#include "DrawDebugHelpers.h"


// "this" is a World Context object
    // is a keyword we can use inside any class in C++ to tell the function which context we are talking about, 
    //which world we are in and that we want the function to run inside "this" world, relating to "this" particular object
    // otherwise the function won't know in which context to operate or where it is.
    // is a pointer to the instance of any particular object that we are working in

// STATIC MEMBER VARIABLES
    // When you want to have a variable that always has the same value for every object of the class, forever and ever, make it static . If you have a method that does not use any instance variables or instance methods, you should probably make it static .  
    // When we declare a member of a class as static it means no matter how many objects of the class are created, there is only one copy of the static member. ... A static member is shared by all objects of the class.
    // The static variable is a class level variable and it is common to all the class objects i.e. a single copy of the static variable is shared among all the class objects. A static method manipulates the static variables in a class
    //  once the variable has been initialized, it remains in memory until the end of the program. You can think of it as saying that the variable sticks around, maintaining its value, until the program completely ends.

// CONTROLER
    // an invisible object associated with the Pawn
    // on begin play the controller will possess the pawn

// Define the Tank Constructor
ATank::ATank()
{
    // Create contructors for every component
    // Attach the spring arm to the root component and the camera to the spring arm
    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArm->SetupAttachment(RootComponent);
    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(SpringArm); 
}

void ATank::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    // Super just refers to the inherited class. When you call Super::Tick, you're saying you want your override method to do the same thing that the inherited method does, and afterwards, you want to do whatever additional functionality you added
        // By using Super::Method you can call any methods from this class parent class without the need to know the name of the parent class. you could also so instead: ParentClassName::Method
        // Call super because we are overriding a virtual function so we make sure we still call all the base functions that were there already in the parent class
        // The parent class will call its version
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    // Bind the move function to the move forward axis map
        // use function BindAxis(the name of the axis mapping we defined in the project settings input section, 
        //"this" means a pointer do the object we are biding the funtion for, the instance to the class that we are in
        //it will point to the instance of the take pawn in the game, 
        // address of the function we are biding)
        // BindAxis(): bind the input from the axis mapping "MoveForward" to "this" instance and perform from the function stored in the address &ATank::Move)
    PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ATank::Move);
    // Bind turn function
    PlayerInputComponent->BindAxis(TEXT("Turn"), this, &ATank::Turn);  
    // Bind Fire function as an action mapping
        // Action mapping: assign a key to it and a function is only called when this action is taken - not every tick like with axis 
        // &ATank::Fire is a callback function: a callbackfunction is a function that is passed as a parameter inside a method 
        //so that it is only called right after the first function is already called
    PlayerInputComponent->BindAction(TEXT("Fire"), IE_Pressed, this, &ATank::Fire);
}

void ATank::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Get the HIT RESULT from cursor to move the turnet acording to its position
        // will draw an invisible line (LINE TRACE) from the start point to the end point of the vector we want to track
        // if ther is an object on the way of this line this will generate a HIT EVENT
        //and store the data (hit location, impact point, actor we hit etc) in a FHitResult struct
        // Specify a trace channel or collision channel ECC_Visibility - objects that block this visibility channel will generate hit events

    // check if player controller ref if valid
    if (TankPlayerController)
    {
        FHitResult HitResult;

        TankPlayerController->GetHitResultUnderCursor(
            ECollisionChannel::ECC_Visibility, //define the trace channel
            false, //bTraceComplex
            HitResult); // store the data about the hit event inside a FHitResult variable
        
        // // Draw a debug sphere to check where we are hitting
        // DrawDebugSphere(
        //     GetWorld(), //which world are we in
        //     HitResult.ImpactPoint, //center of the sphere
        //     20.f, //radius of the sphere
        //     24, //segments
        //     FColor::Red, 
        //     false, //persistency
        //     -1.f //persistency time. if -1 will be called on every frame
        // );

        RotateTurret(HitResult.ImpactPoint);
    }
}

void ATank::HandleDestruction()
{
    Super::HandleDestruction();

    // Hide the tank rather than destroying it to be able to see its camera after death
    SetActorHiddenInGame(true);
    SetActorTickEnabled(false);
    
    bAlive = false;
}

// Called when the game starts or when spawned
void ATank::BeginPlay()
{
	Super::BeginPlay();

    // Store the controller in the pointer variable APlayerController*
        // GetController returns a pointer type AController* (parent of APlayerController*)
        // We can have an pointer type AController* (PARENT) store a pointer type APlayerController* (CHILD) but not the other way around
        // Parent <- stores <- Child / Child X does not store X Parent
        // Altought the pointer (address) AController* stores an object of type APlayerController, we cannot store a pointer AController* inside another pointer APlayerController*
        // Casting: changes the type of pointer1 into pointer2 as long as the object returned by pointer1 maches pointer2
            // Cast<type we want to cast TO>(type of pointer we want to cast FROM): returns a APlayerController* pointer
            // Cast<TO>(FROM)
    TankPlayerController = Cast<APlayerController>(GetController());
}

// bind the move function to the move forward access mapping so that the inputs W and S can move the tank forward or backwards
    //binds the functionality of this function to the W and S inputs
void ATank::Move(float Value)
{   
    // Initialize a vector as zero to be the distance the actor is going to run
	FVector DeltaLocation = FVector::ZeroVector;

    // Assign a value variable to the axis on which we want the actor to move 
    // Adjust vehicle speed by the speed variable * frame rate speed to make it always consistent independent of frame rate fluctuations
        // X = Value * DeltaTime. Scale movement by the same rate of ticks per second so that movement rate is adjusted to the same tick rate per second
	float DeltaTime = UGameplayStatics::GetWorldDeltaSeconds(this);
    DeltaLocation.X = Value * Speed * DeltaTime;

    // AddActorLocalOffset moves the actor according to its local xyz axis (not the world axis)
        // by one "offset" or a fixed distance that will be repeated everytime this function is called
        // Pass DeltaLocation to be the movement offset of this function
        // "true" to enable sweeping meaning that the engine will calculate if there is an object on the way of the movement and stop if collision enabled
	AddActorLocalOffset(DeltaLocation, true);
}

void ATank::Turn(float Value)
{
    // ZeroRotator is a static member variable.
    FRotator DeltaRotation = FRotator::ZeroRotator;
    float DeltaTime = UGameplayStatics::GetWorldDeltaSeconds(this);
    DeltaRotation.Yaw = Value * TurnRate * DeltaTime;
    AddActorLocalRotation(DeltaRotation, true);
}
