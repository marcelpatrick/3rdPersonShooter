
# 3rdPersonShooter
This is a tank battle 3rd person shooter game. It uses Unreal Engine 4.24 and code in C++. It is based on a Udemy tutorial from GameDev.tv and Unreal.

## Project Steps:
1. Create Components (Parent Classes: BasePawn)
2. Create Sub-Components (Child Classes: Tank, Tower, Projectile)
3. Set Uset Input and Game Controllers

## 1: Create Components

Create a BasePawn C++ Class. This will be the parent class of all of our tank and tower components in the game.

### 1.1: Declare Component Variables:
In its header file initialize and expose the variables that will correspond to each of the components that we will use in the world:

```cpp
private: 

	class UCapsuleComponent* CapsuleComp; 

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* BaseMesh; 

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* TurretMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USceneComponent* ProjectileSpawnPoint; 
```

### 1.2: Construct Component Objects:
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
  
### 2.1: Declare Component Variables:
  
```cpp
	private:

		UPROPERTY(VisibleAnywhere, Category = "Components")
		class USpringArmComponent* SpringArm;

		UPROPERTY(VisibleAnywhere, Category = "Components")
		class UCameraComponent* Camera;
```

### 2.2: Construct Component Objects:

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
In Unreal: 
. in BP_PawnTank, in ViewPort, Class Options, change BP_PawnTank parent class from BasePawn to the Tank class we created. This way these components will be visible inside this BP.
. In the game view port, select tank and change Auto Possess Player to Player 0.

### Tower Class

Create Tower Class derived from the BasePawn Class. 

On BP_PawnTurret change its parent class to Tower so that the Turret properties inherit those of the Tower i.e. turning.


## 3: Set User Input and Game Controllers

### 3.1: Create an axis mapping for movement and an action mapping for firing  
Unreal > Edit > Project Settings > Input > Bindings > Axis Mapping / Action Mapping


### 3.2: Bind the axis / action mapping to our action callback functions

Declare the SetupPlayerInputComponent() function - exclude it from BasePawn - and the Move() and Turn() funtions in the Tank header file. 
Include Speed and Turn Rate variables to fine tune the tank's movements.

```cpp
public:
	// Called to bind functionality to input: allows the pawn to handle input from mouse or a keyboard key
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
private:
	void Move(float Value);
	void Turn(float Value);
	
	UPROPERTY(EditAnywhere, Category = "Components")
	float Speed = 600.f;

	UPROPERTY(EditAnywhere, Category = "Components")
	float TurnRate = 100.f;
```

Define our SetupPlayerInputComponent() in Tank.cpp
Inside SetupPlayerInputComponent() bind each user input axis or action mapping to its correspondent action callback functions

```cpp
void ATank::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Bind the move function to the move forward axis map: bind the input from the axis mapping "MoveForward" to "this" instance and perform the callback function stored in the address &ATank::Move
	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ATank::Move);
	// Bind the turn function to the Turn axis map
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &ATank::Turn);  
	// Bind Fire function to the Fire action map
	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Pressed, this, &ATank::Fire);
}
```
Define the action callback functions Move() and Turn() in Tank.cpp:

```cpp
void ATank::Move(float Value)
{   
	// Initialize a vector as zero to be the distance the actor is going to run
	FVector DeltaLocation = FVector::ZeroVector;

	// Scale movement by the same rate of ticks per second so that movement rate is adjusted to the same tick rate per second
	float DeltaTime = UGameplayStatics::GetWorldDeltaSeconds(this);
	DeltaLocation.X = Value * Speed * DeltaTime;

	// AddActorLocalOffset moves the actor according to its local xyz axis (not the world axis)
	AddActorLocalOffset(DeltaLocation, true);
}

void ATank::Turn(float Value)
{
    // Initialize a rotator as zero
    FRotator DeltaRotation = FRotator::ZeroRotator;
    
    // Scale movement by the same rate of ticks per second
    float DeltaTime = UGameplayStatics::GetWorldDeltaSeconds(this);
    DeltaRotation.Yaw = Value * TurnRate * DeltaTime;
    
    // AddActorLocalOffset moves the actor according to its local xyz axis (not the world axis)
    AddActorLocalRotation(DeltaRotation, true);
}
```
Declare the action callback function Fire() in BasePawn.h

```cpp
protected:
	void Fire();
```

Define the action callback function Fire() in BasePawn.cpp - because this one will be inherited by both the Tank and the Tower actors.

```cpp
void ABasePawn::Fire()
{
	// Get location of projectile spawn point
	FVector Location = ProjectileSpawnPoint->GetComponentLocation();
	FRotator Rotation = ProjectileSpawnPoint->GetComponentRotation();

	// Spawn the projectile from the fire point
	AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>(ProjectileClass, Location, Rotation);
	
	// set owner of this projectile to whichever actor fired it so that we know who is firing and who is getting hit by the fired projectile
	Projectile->SetOwner(this);
}
``` 

### 3.3: Make the Tank turret mesh follow the mouse cursor to fire at that direction (get the hit result under the cursor)

In Tank, Declare and Define BeginPlay and remove it from BasePawn

Declare and Define the RotateTurret() function in BasePawn to be inherited by both the Tank and the Tower

in BasePawn.h:
```cpp
protected:

	void RotateTurret(FVector LookAtTarget);
```

in BasePawn.cpp
```cpp
void ABasePawn::RotateTurret(FVector LookAtTarget)
{
	// Create a Vector and assign to it the component's location. Vector = end point - start point
	FVector ToTarget = LookAtTarget - TurretMesh->GetComponentLocation();
	// Create a Rotator and assign to it the Yaw from the creted vector
	FRotator LookAtRotation = FRotator(0.f, ToTarget.Rotation().Yaw, 0.f);
	// Pass in our Rotator to set the turrent's rotation
	TurretMesh->SetWorldRotation(LookAtRotation);
}
```

In Tank.h, Create a pointer variable to store the PlayerController parameters

```cpp
private:
	APlayerController* TankPlayerController; 
```
In Tank.cpp, use GetPlayerController() and assignt its AController result to our APlayerController* variable using Cast to convert the the parent variable type (AController) into the child variable type (APlayerController). Parent <- stores <- Child / Child X does not store X Parent

```cpp
void ATank::BeginPlay()
{
	Super::BeginPlay();

	TankPlayerController = Cast<APlayerController>(GetController());
}
```

Then use the TankPlayerController variable to get the cursor position. Use GetHitResultUnderCursor() and pass its HitResult.ImpactPoint as a parameter to the RotateTurre() function so that it can point the turret to the cursor position.

```cpp
void ATank::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Get the HIT RESULT from cursor to move the turnet acording to its position

    // check if player controller ref if valid
    if (TankPlayerController)
    {
        FHitResult HitResult;

        TankPlayerController->GetHitResultUnderCursor(
            ECollisionChannel::ECC_Visibility, //define the trace channel
            false, //bTraceComplex
            HitResult); // store the data about the hit event inside a FHitResult variable

        RotateTurret(HitResult.ImpactPoint);
    }
}
```

### 3.4: Implement the fire function for the Towers (including a timer)

In Tower.h, Create a Tick and begin play functions and override them. Also create a ATank* pointer to store the Tank's location in order for the turret to find it and follow it. Also, create a variable type FTimerHandle to store info about the world time and pass this as parameters to set our timer if a delay for the fire rate. Declare a CheckFireCondition() function to check if the Towers are in the right moment to fire and a InFireRange() function to trigger if Tank is within fire range: 
```cpp
	public:
		// Override the original tick function so that we can run our own tick function on top of it
		virtual void Tick(float DeltaTime) override;
	protected:
		// Also override beginplay
		virtual void BeginPlay() override;
	private:
		ATank* Tank; 

		UPROPERTY(EditDefaultsOnly, Category = "Combat")
		float FireRange = 100.f; 

		FTimerHandle FireRateTimerHandle; 
		float FireRate = 2.f;
		
		// call back function
		void CheckFireCondition();

		bool InFireRange();
```
In Tank.h, Declare a bool to tell if the tank is still alive, if it is not, stop firing at it
```cpp
	public:
		// boolean to check whether the tank is still alive or if it has died
		bool bAlive = true;
```

In Tower.cpp, Define our custom tick function to find the tank location and rotate the turret towards the tank if it is in range. Also Define our custom BeginPlay function to get Tank location in order for the turret to follow it. Then create a Tank pointer variable to store its location and set the timer. Also, Define CheckFireCondition() and InFireRange functions.
```cpp
// Call our custom tick function
void ATower::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Shoot the tank

    // Check if tank is in range 
    if (InFireRange())
    {
        // And rotate turret to the tank
        RotateTurret(Tank->GetActorLocation());
    }
}

void ATower::BeginPlay()
{
    Super::BeginPlay();

    // GetPlayerPawn returns a APawn* and cannot be stored inside a ATank variable. cannot store a parent type inside a child pointer. Use Cast
    Tank = Cast<ATank>(UGameplayStatics::GetPlayerPawn(this, 0));

    // Set timer for the fire rate
    GetWorldTimerManager().SetTimer(FireRateTimerHandle, this, &ATower::CheckFireCondition, FireRate, true);
}

void ATower::CheckFireCondition()
{
    
    if (Tank == nullptr)
    {
        return;
    }

    // only shoot if tank is still alive and when it is dead stop shooting it
    if (InFireRange() && Tank->bAlive)
    {
        Fire();
    }
}

// Find distance to the tank
bool ATower::InFireRange()
{
    if (Tank)
    {
        float Distance = FVector::Dist(GetActorLocation(), Tank->GetActorLocation());
        if (Distance <= FireRange)
        {
            return true;
        }
    }
    return false;
}
```






