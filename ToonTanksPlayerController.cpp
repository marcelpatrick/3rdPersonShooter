// Fill out your copyright notice in the Description page of Project Settings.


#include "ToonTanksPlayerController.h"
#include "GameFramework/Pawn.h" 


// if the player should be enabled, use EnableInput() to do it, else, use DisableInput() to disable it.
void AToonTanksPlayerController::SetPlayerEnabledState(bool bPlayerEnabled)
{
    if (bPlayerEnabled)
    {
        // access the pawn and enable or disable input
        GetPawn()->EnableInput(this);
    }
    else
    {
        GetPawn()->DisableInput(this);
    }

    // Disable or enable coursor based on whether or not the player should be enabled
    bShowMouseCursor = bPlayerEnabled;
}