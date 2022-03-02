
#include "BasePawn.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Projectile.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"


UStaticMeshComponent;

// UE Inheritance Hierarchy
// OBJECT:
// 	ACTOR: has a Mesh, visual representation, can be placed in the world. Have actor components
// 		PAWN: has movement, can be possessed by a controller to direct its movements with mouse or keyboard inputs
// 			CHARACTER: moves like a person (walking on two legs, flying, crowching etc), handles collision
// 		CONTROLLER:
// 			PlayerController
// 			AI Controller
// 	ActorComponent: reusable functionalities that can be attached to an Actor
// 		USceneComponent: doesnt have a mesh so I cannot see it. has a transform to define rotation and location. you can attach other components to it
// 			UCapsuleComponent: handles Collision
// 			UStaticMeshComponent: has a visual representation in the world. belogs to pawn class
// 			RootComponent: Doesnt have visual representation. everything that derives from the Actor class will inherit the root component. BasePawn also inherits the root component variable because it inherits the actor class
// *** since both UCapsuleComponent and RootComponent are inherit from USceneComponent, I can assign a capsule as my 
// root component so that I can use a capsule (that has collision) to be my root.

// Sets default values
ABasePawn::ABasePawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create a UCapsuleComponent variable to which we can later assign the other components of this object
		// CreateDefaultSubobject is a template function: 
		// A function that adapts to the type that is passed whithing <>
		// the text in the () is the name of this template function
		// It returns an address of the created type function so it needs to be store inside a pointer
		// So we are using a UCapsuleComponent to construct an object of this type
	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Collider"));

	// Declare the components you are going to use
		// and attach each subcomponent to its respective component

	// Assign CampsuleComp as the root component
		//so that when adding a base pawn to the world it always comes with a campsule component first
	RootComponent = CapsuleComp;

	// Create the static mesh components that we are going to attach to the root component
	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Base Mesh"));
	// Attach the base mesh to the root
	BaseMesh->SetupAttachment(CapsuleComp);

	TurretMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Turret Mesh"));
	TurretMesh->SetupAttachment(BaseMesh);

	ProjectileSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Spawn Point")); 
	ProjectileSpawnPoint->SetupAttachment(TurretMesh);

}

// defines what happens when the basepawn and its inherited classes die
void ABasePawn::HandleDestruction()
{
	// Show the explosion when the base pawn object dies
		// Spawn emitter at its proper location 
		// Use UGameplayStatics::SpawnEmitterAtLocation
	if (DeathParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(
		this,
		DeathParticles,
		GetActorLocation(),
		GetActorRotation() 
		);
	}

	if (DeathSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
		this, 
		DeathSound,
		GetActorLocation()
		);
	}

	if (DeathCameraShakeClass)
	{
		GetWorld()->GetFirstPlayerController()->ClientPlayCameraShake(DeathCameraShakeClass);
	}
	
}

void ABasePawn::RotateTurret(FVector LookAtTarget)
{
	// Create a Vector and assign to it the component's location. Vector = end point - start point
	FVector ToTarget = LookAtTarget - TurretMesh->GetComponentLocation();
	// Create a Rotator and assign to it the Yaw from the creted vector
	FRotator LookAtRotation = FRotator(0.f, ToTarget.Rotation().Yaw, 0.f);
	// Pass in our Rotator to set the turrent's rotation
	TurretMesh->SetWorldRotation(LookAtRotation);
}

void ABasePawn::Fire()
{
	// Get location of projectile spawn point and draw a debug sphere there
	FVector Location = ProjectileSpawnPoint->GetComponentLocation();
	FRotator Rotation = ProjectileSpawnPoint->GetComponentRotation();

	// DrawDebugSphere(
	// 		GetWorld(),
	// 		Location,
	// 		15.f,
	// 		12,
	// 		FColor::Red,
	// 		false,
	// 		10.f
	// 	);

	// Spawn the projectile from the fire point
		// auto allows the compiller to figure out the type to assign to this var
			// same thing as doing AProjectile*
	AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>(ProjectileClass, Location, Rotation);
	// set owner of this projectile actor
	Projectile->SetOwner(this);
}

