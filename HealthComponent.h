// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) /* Allows us to spawn this component directly into blueprints */ ) 
class TOONTANKS_API UHealthComponent : public UActorComponent // UHealthComponent is derived from UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHealthComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:

	UPROPERTY(EditAnywhere) 
	float MaxHealth = 500.f;
	float Health = 0.f;
	
	// My custom callback function
	UFUNCTION()
	void DamageTaken(
		AActor* DamagedActor, // actor who is taking the damage
		float Damage, // damage amount
		const UDamageType* DamageType, // custom damage types - fire, explosion etc - that have custom actions
		AController* Instigator, // Controler responsible for the damage - controller possessing the pawn causing the damage
		AActor* DamageCauser // actor causing the damage - in this case, the projectile or "this"
		);

	class AToonTanksGameMode* ToonTanksGameMode;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
