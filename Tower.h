// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BasePawn.h"
#include "Tank.h"
#include "Tower.generated.h"

/**
 * 
 */
UCLASS()
class TOONTANKS_API ATower : public ABasePawn
{
	GENERATED_BODY()
	
	public:

		// Override the original tick function so that we can run our own tick function on top of it
		virtual void Tick(float DeltaTime) override;

		void HandleDestruction();

	protected:

		// Also override beginplay
		virtual void BeginPlay() override;

	private:

		ATank* Tank; 

		UPROPERTY(EditDefaultsOnly, Category = "Combat")
		float FireRange = 100.f; 

		FTimerHandle FireRateTimerHandle; 
		float FireRate = 2.f;
		// call back function
		void CheckFireCondition();

		bool InFireRange();
};

