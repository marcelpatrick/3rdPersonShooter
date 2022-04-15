// Fill out your copyright notice in the Description page of Project Settings.


#include "Tower.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"


// In order for this to work we need to associate this C++ class to our Tower Mesh. 
    // open the tower BP, click on class settings, on the parent class drop down select our Tower c++ class


// Call our custom tick function
void ATower::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Shoot the tank

    // Check if tank is in range 
    if (InFireRange())
    {
        // And rotate turret to the tank
        RotateTurret(Tank->GetActorLocation());
    }
}

void ATower::HandleDestruction()
{
    // call HandleDestruction function that was implemented in basepawn - for sound effects
    Super::HandleDestruction();

    UE_LOG(LogTemp, Warning, TEXT("A TORRE FOI DESTRUIDA ###"));

    Destroy();
}

void ATower::BeginPlay()
{
    Super::BeginPlay();

    // GetPlayerPawn returns a APawn* and cannot be stored inside a ATank variable. cannot store a parent type inside a child pointer. Use Cast
    Tank = Cast<ATank>(UGameplayStatics::GetPlayerPawn(this, 0));

    // Set timer for the fire rate
        // GetWorldTimerManager().SetTimer(
            // FireRateTimerHandle: a FTimerHandle type variable that allows the timer manager to have multiple timer as allows us to access data of each timer, 
            // this: user class, the class we are working in,
            // &ATower::CheckFireCondition: address of our call back function, 
            // FireRate: time delay before callback function is called, 
            // true: whether or not this timer should loop)
    GetWorldTimerManager().SetTimer(FireRateTimerHandle, this, &ATower::CheckFireCondition, FireRate, true);
}

void ATower::CheckFireCondition()
{
    
    if (Tank == nullptr)
    {
        return;
    }

    // only shoot if tank is still alive and when it is dead stop shooting it
    if (InFireRange() && Tank->bAlive)
    {
        Fire();
    }
}

// Find distance to the tank
bool ATower::InFireRange()
{
    if (Tank)
    {
        float Distance = FVector::Dist(GetActorLocation(), Tank->GetActorLocation());
        if (Distance <= FireRange)
        {
            return true;
        }
    }
    return false;
}