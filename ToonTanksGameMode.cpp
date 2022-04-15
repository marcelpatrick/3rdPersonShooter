// Fill out your copyright notice in the Description page of Project Settings.


#include "ToonTanksGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Tank.h"
#include "Tower.h"
#include "ToonTanksPlayerController.h"
#include "TimerManager.h" 


// ACTOR DEATH: code order
    // 1- Create a HandleDestruction function will handle the destruction of our actors in the basepawn class and implement in tank and turret classes
        // 2- Create ActorDied function to handle what happens when they die - call it when health = 0, place in the gamemode class
            // 3- Call HandleDestuction in ActorDied function 
                // 4- Call ActorDied from the HealthComponent when Health reaches 0

// ACTOR DEATH: logic order
    // ApplyDamage: called by Projectile, trigger function that genarates the hit event
        // DemageTaken: callback function that responds to the delegate broadcast in the health component and applies the decrease in health variables and will call ActorDied function in the gamemode
            // ActorDied: funtion in the GameMode that handles what happens when an actor dies
                // HandleDestruction: function in the pawn that will destroy the actor



void AToonTanksGameMode::ActorDied(AActor* DeadActor)
{
    // check if the dead actor is the tank
    if (DeadActor == Tank)
    {
        Tank->HandleDestruction();

        // Check if player controller is valid
            // access the private function through a public getter function
            // check if out var that stores the player controller is valid
        if (ToonTanksPlayerController)
        {
            // disable input, disable mouse cursor
                // include our creted class ToonTanksPlayerController, declare it and access its method SetPlayerEnabled state passing false as a boolean
            ToonTanksPlayerController->SetPlayerEnabledState(false);
        }   
        // false for not winning the game
        GameOver(false);
    }
    // if the dead actor is the tower instead of the tank
    else if (ATower* DestroyedTower = Cast<ATower>(DeadActor))  /*take the result of the DeadActor cast to ATower type and assign to a local variable in this condition*/
    {
        // if this cast is successful
        DestroyedTower->HandleDestruction();

        // count how many towers were destroyed
            // Decrement towers count everytime a tower dies
        --TargetTowers;
        // end game when all towers are killed
        if (TargetTowers == 0)
        {
            GameOver(true);
        }
        
    }
}

void AToonTanksGameMode::BeginPlay()
{
    // Use super to access parent BeginPlay functionality 
    Super::BeginPlay();

    HandleGameStart();
}

void AToonTanksGameMode::HandleGameStart()
{

    //get tower count
    TargetTowers = GetTargetTowerCount();

    // poiter to APlayerPawn in a form of ATank pointer.
    Tank = Cast<ATank>(UGameplayStatics::GetPlayerPawn(this /*context*/, 0 /*player number*/));

    // store the player controller value inside my toon tanks player controller var 
    ToonTanksPlayerController = Cast<AToonTanksPlayerController>(UGameplayStatics::GetPlayerController(this, 0));

    // Call here a implementable event function and define what it is going to do in the blueprint
        // we will use this to call the timer at the beginning of the game
        // since I'm calling the function from inside another function that was already declared here, I don't need to use AToonTanksGameMode::
    StartGame();

    // if player controller is enabled (true)
    if (ToonTanksPlayerController)
    {
        // set it to false: restrict tanks movement
        ToonTanksPlayerController->SetPlayerEnabledState(false);

        // create a timer var from the type FTimerHandle
            // timer vars are used to perform an action once (or repeatedly) at specified intervals. 
            // Timers can be used to set actions to happen on a delay or over a period of time.
        FTimerHandle PlayerEnabledTimerHandle;

        // set a local variable for the timer Delegate
            // a delegate is like a listener that calls other function that are subscrived to its invokation list
            // timer delegate allows to create an object that can bind a callback function directly to the timer
            // so when a specif time delay has passed, this delegate function will call its callback function - SetPlayerEnabledState
                //passing in true as a parameter for this callback function
        FTimerDelegate PlayerEnabledTimerDelegate = FTimerDelegate::CreateUObject(
            ToonTanksPlayerController, /* user object that we are talking about here */
            &AToonTanksPlayerController::SetPlayerEnabledState, /* address of the callback function that will be called */
            true /* boolean value that will be passed back to the SetPlayerEnabledState callback function when it gets called in the end of the timer delay*/
        );
        // set my timer
        GetWorldTimerManager().SetTimer(PlayerEnabledTimerHandle, /* my timer handle var */
        PlayerEnabledTimerDelegate, /* my timer Delegate */
        StartDelay, /* float for the time delay */
        false /* false so that this timer doesn't loop */ 
        ); 
    }
    
}

// count all towers in the world
int32 AToonTanksGameMode::GetTargetTowerCount()
{
    TArray<AActor*> Towers;

    UGameplayStatics::GetAllActorsOfClass(
        this, /* world context object */
        ATower::StaticClass(), /* actor class type, ::StaticClass() is a function that returns the actor class */
        Towers /* the array that is going to be populated by this function - is an out parameter */
        );

    return Towers.Num();
}