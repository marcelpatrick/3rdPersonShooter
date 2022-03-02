
# 3rdPersonShooter
This is a tank battle 3rd person shooter game

## Project Steps:
1. Construct Components (BasePawn, Actor Meshes: Tank, Tower, Projectile)


## 1: Construct Components

Create a BasePawn C++ Class

In its header file initialize and expose the variables that will correspond to each of the components that we will use in the world:

```cpp
private: 

// Declare the components that I am going to use
class UCapsuleComponent* CapsuleComp; 

UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
UStaticMeshComponent* BaseMesh; 

UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
UStaticMeshComponent* TurretMesh;

UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
USceneComponent* ProjectileSpawnPoint; 
```
In the cpp file, Construct these components so that they are able to ve visible in our world

```cpp
ABasePawn::ABasePawn()
{
PrimaryActorTick.bCanEverTick = true;

// CONSTRUCT THE COMPONENTS TO BE USED IN THE GAME: Use CreateDefaultSubobject passing the type of each component inside de <>
// Construct a UCapsuleComponent variable to which we can later assign the other components and that will be the root of all components
CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Collider"));
RootComponent = CapsuleComp;
BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Base Mesh"));
TurretMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Turret Mesh"));
ProjectileSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Spawn Point")); 

// ATTACH EACH SUBCOMPONENT TO ITS RESPECTIVE COMPONENT: Projectile > Turret of the tank and towers > Base of the tank and towers > root capsule component
BaseMesh->SetupAttachment(CapsuleComp);
TurretMesh->SetupAttachment(BaseMesh);
ProjectileSpawnPoint->SetupAttachment(TurretMesh);
}
```

