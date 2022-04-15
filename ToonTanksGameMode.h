// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ToonTanksGameMode.generated.h"

/**
 * 
 */
UCLASS()
class TOONTANKS_API AToonTanksGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:

// Actor died function. needs to be public because it will be called from the HealthComponent
	void ActorDied(AActor* DeadActor);

protected:

	// override begin play to store the tank pointer
	virtual void BeginPlay() override;

	// blueprint implementable event for the timer. 
		// a blueprint implementable event funtion is a function that can be called on code and implement it's functionalities and behavior in the blueprint
	UFUNCTION(BlueprintImplementableEvent)
	void StartGame();

	// function for winning the game
	UFUNCTION(BlueprintImplementableEvent)
	void GameOver(bool bWonGame);


private:

	// variable to store the tank pointer 
	class ATank* Tank;

	// pointer var to access player controller
	class AToonTanksPlayerController* ToonTanksPlayerController;

	// timer that will count the amount of time until we can start moving
	float StartDelay = 3.f;

	// function to start timer
	void HandleGameStart();

	// track the number of towers
	int32 TargetTowers = 0;
	int32 GetTargetTowerCount();
	
};
