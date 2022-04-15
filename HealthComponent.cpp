// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthComponent.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "ToonTanksGameMode.h"


// 1- Set health variables
// 2- make BP_PawnTank own this HealthComponent - go into BP_PawnTank and add this component in there
// 3- do the same for BP_PawnTurret
// this way all of my actors will have a health component associated to them

// Damage action sequence:
	// 1- UGameplayStatics::ApplyDamage generates the damage event
		// 2- OnTakeAnyDamage (a multicast Delegate function) will listen to the demage event generate by ApplyDamage and broadcast the demage event data to all functions that are bound to it in its invocation list
			// 3- DamageTaken() (our own custom callback function) is the function that is binded will be called back by OnTakeAnyDamage whenever the damanges occur

// Damage sequance to set my code
	// 1- Create the callback function DamageTaken
		// 2- Bind it to the delegate OnTakeAnyDamage
			// 3- Call the damage event ApplyDamage

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	Health = MaxHealth; 

	// Access the owner of this component to bind the DamageTaken callback to the OnTakeAnyDamage delegate that is in the owner
		// returns a AActor pointer which is the component that owns this component
			// Call the owener with GetOwner and access the OnTakeAnyDamage function inside it
	GetOwner()->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::DamageTaken);

	// Get access to the gamemode
		// gamemode returns a AgamemodeBase variable and we are trying to store in a child class ToonTanksGameMode - so we need to cast to convert the AGameModeBase var into a ToonTanksGameMode var
	ToonTanksGameMode = Cast<AToonTanksGameMode>(UGameplayStatics::GetGameMode(this));

}


// Called every frame
void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

// Bind this callbackfunction (DamageTaken) to the Delegate (OnTakeAnyDamage) that is in the pawn that owns this component 
void UHealthComponent::DamageTaken(AActor *DamagedActor, float Damage, const UDamageType *DamageType, AController *Instigator, AActor *DamageCauser)
{
	// specify the impact of the damage on the health variables
	if (Damage <= 0.f) return;

	Health -= Damage;
	UE_LOG(LogTemp, Warning, TEXT("vc foi atingido!!!!!. Health = %f"), Health);

	// Check if health reached zero and the GameMode is valid and then call function actor died passing the actor that was hit
	if (Health <= 0.f && ToonTanksGameMode)
	{
		ToonTanksGameMode->ActorDied(DamagedActor);
		UE_LOG(LogTemp, Warning, TEXT("ator que morreu**********: %s"), *DamagedActor->GetName());
	}
	
}

