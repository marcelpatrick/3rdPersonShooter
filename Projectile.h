// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

// We can foward declare a class here so that all var underneeth already use this same foward declaration
class USoundBase;


// 0- On File>Project>Create new C++ class


UCLASS()
class TOONTANKS_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	// 1- Create a UStaticMesh component variable
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	UStaticMeshComponent* ProjectileMesh;

	UPROPERTY(VisibleAnywhere, Category = "Movement")
	class UProjectileMovementComponent* ProjectileMovementComponent;

	// Declare the callback function to be broadcasted when the OnComponentHit function is called
		// UFunction is like Uproperty but for functions, not for variables
	UFUNCTION()
	void OnHit(
		UPrimitiveComponent* HitComp, // Component that will hit something
		AActor* OtherActor, // Actor that got hit
		UPrimitiveComponent* OtherComp, // Other component that was hit (static mesh component associated to the actor)
		FVector NormalImpulse, // Force vector or impulse that will be imprinted by the hit 
		const FHitResult& Hit); // Variable that will store info about the hit

	UPROPERTY(EditAnywhere)
	float Damage = 50.f;

	// particle syst var
	UPROPERTY(EditAnywhere, Category = "Combat")
	class UParticleSystem* HitParticles;

	// particle system component
	UPROPERTY(EditAnywhere, Category = "Combat")
	class UParticleSystemComponent* TrailParticles;

	// var for sound
	UPROPERTY(EditAnywhere, Category = "Combat")
	USoundBase* LaunchSound;

	// var for sound
	UPROPERTY(EditAnywhere, Category = "Combat")
	USoundBase* HitSound;

	// camera shake var - for UE 4.25 type is UCameraShake, for UE 4.26 type is UCameraShakeBase
	UPROPERTY(EditAnywhere, Category = "Combat")
	// TSubClassOf<> is a template function that creates a variable that can store a UClass type. 
		// it is used to spawn actors in our world that are blueprint based (such as our HitCameraShake blueprint)
	TSubclassOf<class UCameraShake> HitCameraShakeClass;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
