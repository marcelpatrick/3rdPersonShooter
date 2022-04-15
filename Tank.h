// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BasePawn.h"
#include "Tank.generated.h"

/**
 * 
 */
UCLASS()
class TOONTANKS_API ATank : public ABasePawn
{
	GENERATED_BODY()
	
	public:

		// Constructor
		ATank();
		
		// Called to bind functionality to input: allows the pawn to handle input from mouse or a keyboard key
		virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

		// Called every frame
		virtual void Tick(float DeltaTime) override;

		void HandleDestruction();

		// A Getter function to access the private variable player controller from ToonTanksGameMode
			// const because getter functions don't change any variables in this class, it only returns that private variable as it is
		APlayerController* GetTankPlayerController() const { return TankPlayerController; }

		// boolean to check whether the tank is still alive or if it has died
		bool bAlive = true;

	protected:
		// Called when the game starts or when spawned
		virtual void BeginPlay() override;

	private:

		UPROPERTY(VisibleAnywhere, Category = "Components")
		class USpringArmComponent* SpringArm;

		UPROPERTY(VisibleAnywhere, Category = "Components")
		class UCameraComponent* Camera;

		UPROPERTY(EditAnywhere, Category = "Components")
		float Speed = 600.f;

		UPROPERTY(EditAnywhere, Category = "Components")
		float TurnRate = 100.f;

		void Move(float Value);
		void Turn(float Value);

		APlayerController* TankPlayerController; 
};

