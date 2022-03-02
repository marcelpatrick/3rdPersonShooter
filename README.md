
# 3rdPersonShooter
This is a tank battle 3rd person shooter game

## Project Steps:
1. Create Components (Parent Classes: BasePawn)
2. Create Sub-Components (Child Classes: Tank, Tower, Projectile)

## 1: Create Components

Create a BasePawn C++ Class. This will be the parent class of all of our tank and tower components in the game.

### 1.1: Create Component Variables:
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
### 2.1: Construct Component Objects:
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

## 2: Create Sub-Components

Create Tank and Tower components that will inherit from BasePawn parent component.
On Unreal Engine, right click on BasePawn and select "create c++ class derived from this class"

### Tank Class:

The tank will be our player
Attach the Camera and Sprint arm to the Tank to control the 3rd person view
  Unreal Engine option: open BP_PawnTank, click on AddComponent button on the view port and add a Sprint Arm and a Camera attached to it
  c++ option:
  
### 2.1: Create Component Variables:
  
```cpp
	private:

		UPROPERTY(VisibleAnywhere, Category = "Components")
		class USpringArmComponent* SpringArm;

		UPROPERTY(VisibleAnywhere, Category = "Components")
		class UCameraComponent* Camera;
```

### 2.1: Construct Component Objects:

Construct the Camera and Sprint arm components. Attach the spring arm to the root component and the camera to the spring arm

```cpp
ATank::ATank()
{
    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArm->SetupAttachment(RootComponent);
    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(SpringArm); 
}
```



