// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"
// 2- inclure libraries for my components: StaticMeshComponent
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/DamageType.h"
#include "Kismet/GameplayStatics.h" 
#include "Particles/ParticleSystemComponent.h"

// Multicast Delegate
	// Works like a listener that calls another function when an event occurs
		// The difference is that listeners and events don't rely on the code provided by the functions called
		// Whereas delegates depend upon the code from the functions that subscribed to it in order to execute


// UActorComponent:
	// has no transform: no rotation, location or scale
	// cannot attacht it to anything

// USceneComponent
	// has a transform 
	// supports attachment

// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	// Set to false because we don't need this actor to call tick on every frame
	PrimaryActorTick.bCanEverTick = false;

	// 3- Construct or Declare our UStaticMesh variable to be the representation of this component in the world and assign its name
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Projectile Mesh"));

	// 4- Set this component as the root component of our object
	RootComponent = ProjectileMesh;

	// 5- Create a Blueprint based on this class
		// go to content browser, c++ classes, right click on this class and click on "create blueprint based on this class"
		// in the blueprint, on Class settings, select the parent class as this class

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement Component"));
	ProjectileMovementComponent->MaxSpeed = 1300.f;
	ProjectileMovementComponent->InitialSpeed = 1300.f;

	// Construct the particle system that will be the smoke trail of the projectile
	TrailParticles = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Smoke Trail"));
	// Attach to the projectile root component
	TrailParticles->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	// Bind the OnHit function to the delegate
		// our mesh component -> hit event (Multicast Delegate - can have multiple functions bound to it to be called and broadcasts them to the classes)
			//Delegate keeps listening to events and broadcasts to all the other classes that have their functions bound to if that the event occurred. 
				//So when the event occurs all bound functions from all classes are called in response to the broadcast
		// .AddDynamic is the function that binds callback functions to be called by the Delegate
			//.AddDynamic(user object, callback function to be added to the hit event invocation list and then called)
	ProjectileMesh->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);

	// play launch sound as soon as the projectile is spawned
	if (LaunchSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
		this, /*world context object*/
		LaunchSound, /*sound*/
		GetActorLocation() /*location*/ 
		);
	}
	

}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Define the OnHit Callback function to be the called by OnComponentHit
void AProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// logs
		// UE_LOG(LogTemp, Warning, TEXT("on hit foi chamada"));
		// UE_LOG(LogTemp, Warning, TEXT("componente que foi atirado: %s"), *HitComp->GetName());
		// UE_LOG(LogTemp, Warning, TEXT("actor que foi acertado: %s"), *OtherActor->GetName());
		// UE_LOG(LogTemp, Warning, TEXT("outro ator que foi acertado: %s"), *OtherComp->GetName());

	// local actor pointer that stores the value of the owner
		// access the instigator controller out of this actor
	AActor* MyOwner = GetOwner();
	if (MyOwner == nullptr)
	{
		Destroy();
		return;
	}
	// get the instigator controller and store in a local var
	AController* MyOwnerInstigator = MyOwner->GetInstigatorController();

	// get the damage type - defines the particular type of damage that we are causing
	UClass* DamageTypeClass = UDamageType::StaticClass();

	// Apply damage if the projectile hit another actor, not hit us.
		// check if: (other actor is not null && Other actor is not = to this actor - not cause damage to ourselves && other actor not equals to my owner - not to cause damage to my owner)
	if (OtherActor && OtherActor != this && OtherActor != MyOwner)
	{
		// Call apply damage
			// this will generate the damage event
				// then OnTakeAnyDamage delegate will call back the callback function DamageTaken in response to this
					// DamageTaken will then apply the damage impact on the HealthComponent variables
		UGameplayStatics::ApplyDamage(
			OtherActor, /* damaged actor */
			Damage, /* damage amount */ 
			MyOwnerInstigator, /* the controller of the actor who is causing damage */
			this, /* actor causing the damage */ 
			DamageTypeClass /* type of the class associated to the type of damage we are causing */
			);

		// if hit particles is set and valid (not a nullpointer)
		if (HitParticles)
		{
			// Spawn the emmiter effect at this location. dynamically creates and destroys this particlee at run time (it is not a component and cannot be attached to the projectile)
			UGameplayStatics::SpawnEmitterAtLocation(this, HitParticles, GetActorLocation(), GetActorRotation());
		}

		// check if HitSound var had a value assigned to it and is not null
		if (HitSound)
		{
			UGameplayStatics::PlaySoundAtLocation(
			this, 
			HitSound,
			GetActorLocation()
			);
		}

		// Perform camera Shake
		if (HitCameraShakeClass)
		{
			// UE 4.25 - ClientPlayCameraShake, UE 4.26 - ClientStartCameraShake
				// function camera shake belongs to player controller
				// Pass in our HitCameraShake TSubclassOf var > HitCameraShakeClass is exposed in BP_Projectile > in BP_Projectile, assign our HitCameraShake blueprint to our HitCameraShakeClass variable 
					// On hit > our ClientPlayCameraShake function will perform the specific camera shake based on the shake parameters inside our HitCameraShakeClass var that were passed in by our HitCameraShake blueprint
			GetWorld()->GetFirstPlayerController()->ClientPlayCameraShake(HitCameraShakeClass);
		}
	}
	// Destroy the projectile after it hits anything
	Destroy();
}
