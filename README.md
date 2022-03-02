
# 3rdPersonShooter
This is a tank battle 3rd person shooter game

## Project Steps:
1. Construct Components (BasePawn, Actor Meshes: Tank, Tower, Projectile)

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

