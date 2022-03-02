// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "BasePawn.generated.h"


// UPROPERTY SPECIFIERS
	// 	// It will be visible but not editable
	// 	UPROPERTY(VisibleAnywhere) 
	// 	int32 VisibleAnywhereInt = 12;
	// 	// Visible and editable
	// 	UPROPERTY(EditAnywhere)
	// 	int32 EditAnywhereInt = 23;
	// 	// This variable will only be visible in instances (copies of our default BP) that were dragged in the world
	// 	UPROPERTY(VisibleInstanceOnly)
	// 	int32 VisibleInstanceOnlyInt = 11;
	// 	UPROPERTY(VisibleDefaultsOnly)
	// 	float VisibleOnlyOnDefaultBP; 
	// 	UPROPERTY(EditDefaultsOnly)
	// 	int32 EditDefaultsOnlyInt = 9;
	// 	UPROPERTY(EditInstanceOnly) 
	// 	int32 EditInstanceOnlyInt = 32;
	// 	// Accessible also from the BP event graph
	// 	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	// 	int32 VisibleAnywhereIntBP = 23;
	// 	// Accessible (visible only) also from the BP event graph
	// 	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	// 	int32 EditAnywhereIntBP = 44;
	// 	// Editable on instance only
	// 	UPROPERTY(EditInstanceOnly)
	// 	FString MyString = "Patrick";
	// 	// meta: Metadata - to expose private variables
	// 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MyCategory", meta = (AllowPrivateAccess = "true"))
	// 	FString MeuNome = "Patrick";
	// 	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MyCategory", meta = (AllowPrivateAccess = "true"))
	// 	int32 MyInt = 234;

// UCLASS()
	// A UClass can interact between C++ and blueprints (reflection)
	// The UCLASS() macro is used to indicate that a C++ class will be part of Unreal's Reflection system. This is necessary for the C++ class to be recognized by the Unreal Engine editor.
	// represents a class type in unreal realated to that object (not a c++ class)
	// UClass variables can represent a blueprint class type and pass information contained in that blueprint such as 
	//which meshes are associated with it and its configurations

UCLASS()
class TOONTANKS_API ABasePawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ABasePawn();

	// Create handle destruction. called from the gamemode class so it need to be public
	void HandleDestruction();

// protected variables are accessible by this class AND also its child classes, but not by other classes
protected:

	void RotateTurret(FVector LookAtTarget);

	void Fire();

// member variables are private so that they are only accessible by the class that owns them
	// this prevents other classes that don't have nothing to do with this one going in, accessing these variables and messing with
	// the state of the objects that are children of this class
	// if any class can access any variable this makes it more prone to errors and bugs. private variables makes it easier to 
	//track where the bug came from. 
	// Also if a variable is used by multiple classes it creates a dependency in a sence that if you change it in class A you 
	//also have track all the classes in which it is being used in order to update it everywhere.
	// if you need to access these variables from outside of the class you need to use Getter and Setter methods
private: 

	// Include the components that I am going to use

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	// * pointers: are just addresses so they are more lightweight and efficient

	// FORWARD DECLARATION
		// "class" is used as forward declaration. so that we don't need to include the header file (library) for this type and saves space
		//but then we cannot use inheritance from parent classes.
		// we don't need to include the capsule component library here because we are not going to use this variable here
		// we only need to include capsule component library in the cpp file because it is where we will use this type to construct something
		// it is bad to include too much libraries in header files because these files are inherited by many other classes that may not need that library (this saves memory)
		// we don't need to include libraries if we are only going to declare the variable
		// we need libraries only when we are going to construct something out of it, access member variables or inherite properties from this variable type
	class UCapsuleComponent* CapsuleComp; 

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* BaseMesh; 

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* TurretMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USceneComponent* ProjectileSpawnPoint; 

	// To use C++ functions to spawn actors (AProjectile) that are based on blueprints we need to use TSubClassOf<>
		// TSubClassOf<> is a temple function that creates a variable that can store a UClass type
		// once created, in the tank blueprint, assign this variable to BP_Projectile to set the Projectile class to the BP_Projectile type
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TSubclassOf<class AProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	class UParticleSystem* DeathParticles;

	// var for sound
	UPROPERTY(EditAnywhere, Category = "Combat")
	class USoundBase* DeathSound;

	// var for camera shake
	UPROPERTY(EditAnywhere, Category = "Combat")
	TSubclassOf<class UCameraShake> DeathCameraShakeClass;

};
