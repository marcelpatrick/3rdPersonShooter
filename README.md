
# 3rdPersonShooter
This is a tank battle 3rd person shooter game. It uses Unreal Engine 4.24 and code in C++. It is based on a Udemy tutorial from GameDev.tv and Unreal.

# Project Steps:
1. Create Components (Parent Classes: BasePawn)
2. Create Sub-Components (Child Classes: Tank, Tower, Projectile)
3. User Input (Moving the Actor, Firing)
4. Actions and Events (Hit Events, Health Component, Apply Damage)
5. Game Rules, Game Mode and Game Controller (Game Cycle: Start > Death > Winning, Loosing > End)
6. Special Effects (Sounds, Particles)


-> #include are not described here and must be added to each respective component when needed. Refer to the code documents.

# 1: Create Components

Create a BasePawn C++ Class. This will be the parent class of all of our tank and tower components in the game.

## 1.1: Declare Component Variables:
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

## 1.2: Construct Component Objects:
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

# 2: Create Sub-Components

Create Tank and Tower components that will inherit from BasePawn parent component.
On Unreal Engine, right click on BasePawn and select "create c++ class derived from this class"

## 2.1: Tank Class:

The tank will be our player
Attach the Camera and Sprint arm to the Tank to control the 3rd person view
  Unreal Engine option: open BP_PawnTank, click on AddComponent button on the view port and add a Sprint Arm and a Camera attached to it
  c++ option:
  
### 2.1.1: Declare Component Variables:
  
```cpp
private:

	UPROPERTY(VisibleAnywhere, Category = "Components")
	class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	class UCameraComponent* Camera;
```

### 2.1.2: Construct Component Objects:

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

## 2.2: Tower Class

Create Tower Class derived from the BasePawn Class. 

On BP_PawnTurret change its parent class to Tower so that the Turret properties inherit those of the Tower i.e. turning.

## 2.3: Projectile Class:

Create a projectile c++ class with Actor as parent.

### 2.3.1: Declare Component Variables.

In Projectile.h
```cpp
private:
	// 1- Create a UStaticMesh component variable
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	UStaticMeshComponent* ProjectileMesh;
```

### 2.3.2: Construct Component Objects.
```cpp
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	// Set to false because we don't need this actor to call tick on every frame
	PrimaryActorTick.bCanEverTick = false;

	// 3- Construct or Declare our UStaticMesh variable to be the representation of this component in the world and assign its name
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Projectile Mesh"));

	// 4- Set this component as the root component of our object
	RootComponent = ProjectileMesh;
}
```

Create a new BluePrint based on this projectile c++ class. Open it and select the projectile static mesh for this BP.


# 3: User Input


## 3.1: Create an axis mapping for movement and an action mapping for firing  
Unreal > Edit > Project Settings > Input > Bindings > Axis Mapping / Action Mapping

## 3.2: Bind the axis / action mapping to our action callback functions

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
## 3.3: Fire Function:

In BasePawn.h, Declare the action callback function Fire(). Then Use TSubclassOf<> to spawn an object in our world that reflects a Blueprint and stores a UClass type object. UClass objects are Unreal objects that can communicate between c++ and Unreal blueprints. UClass translates any type of c++ class into an Unreal compatible class. This is necessary for the C++ class to be recognized by the Unreal Engine editor.

```cpp
protected:
	void Fire();
	
	// To use C++ functions to spawn actors (AProjectile) that are based on blueprints we need to use TSubClassOf<>
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TSubclassOf<class AProjectile> ProjectileClass;
```
In the tank blueprint > Combat > Projectile Class set BP_Projectile as the Projectile class to be spawned by the Tank. Now the Tank's projectile class is set to our BP_Projectile type, which is a UClass type. Meaning that now our tank will spawn a projectile that is based on the blueprint that we created, BP_Projectile and which already contains the static mesh of the projectile 3d representation. Had we not used TSubclassOf<> it would only spawn an object based on a raw c++ class which could not contain a static mesh. Do the same thing in BP_PawnTower blueprint.

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
### 3.3.1: Set the Projectile movement: 

In Projectile.h, Declare the movement component variable
```cpp
private:
	UPROPERTY(VisibleAnywhere, Category = "Movement")
	class UProjectileMovementComponent* ProjectileMovementComponent;
```

In Projectile.cpp, Define the projectile movement component 
```cpp
AProjectile::AProjectile()
{

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement Component"));
	ProjectileMovementComponent->MaxSpeed = 1300.f;
	ProjectileMovementComponent->InitialSpeed = 1300.f;
```

## 3.4: Make the Tank turret mesh follow the mouse cursor to fire at that direction (get the hit result under the cursor)

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

## 3.5: Implement the Fire function for the Towers (including a timer)

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


# 4. Actions and Events


## 4.1: Hit event

- **HIT**: Projectile Component hits an Actor > it triggers a Hit Event > the Multicast Delegate function **OnComponentHit**, in the Projectile class, listens to this event and broadcasts **FHitResult** to the Callback Function **OnHit()**, also in the Projectile class, bound to it by **AddDynamic** 

### 4.1.1: Multicast Delegate:

Broadcast the hit event and call the callback function OnHit(). Use a Multicast Delegate OnComponentHit() - already present in the Unreal Engine code.

A Multicast Delegate Works like a listener that calls other functions when an event occurs. It will listen to the hit event generated by OnComponentHit() and broadcast the event data in FHitResult - the Call - to all Callback functions - OnHit() - that are bound to it in its invocation list.

In Projectile.cpp, in BeginPlay(): 
```cpp
void AProjectile::BeginPlay()
{
Super::BeginPlay();

	// .AddDynamic is the function that binds callback functions to be called by the Delegate
	//.AddDynamic(user object, callback function to be added to the hit event invocation list and then called)
	ProjectileMesh->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);
```

Make sure that in BP_Projectile blueprint, collision presets are set as "Collision Enabled: Query and Physics"

### 4.1.2: Callback Function

Declare the Callback Functions and Define which actions it will perform 

In Projectile.h: Declare the OnHit() Callback function to be broadcasted when the OnComponentHit function is called.
```cpp
private:
	// UFunction is like Uproperty but for functions, not for variables
	UFUNCTION()
	void OnHit(
		UPrimitiveComponent* HitComp, // Component that will hit something
		AActor* OtherActor, // Actor that got hit
		UPrimitiveComponent* OtherComp, // Other component that was hit (static mesh component associated to the actor)
		FVector NormalImpulse, // Force vector or impulse that will be imprinted by the hit 
		const FHitResult& Hit); // Variable that will store info about the hit
```

In Projectile.cpp: Define the OnHit Callback function to be called by OnComponentHit and specify what actions it will perform.
```cpp
void AProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{

}
```

## 4.2: Damage Event: 

- **HIT**: Projectile Component hits an Actor > it triggers a Hit Event > the Multicast Delegate function **OnComponentHit**, in the Projectile class, listens to this event and broadcasts **FHitResult** to the Callback Function **OnHit()**, also in the Projectile class, bound to it by **AddDynamic** 
   - **DAMAGE**: > the **OnHit()** Callback function will apply the damage using **UGamePlaystatics::ApplyDamage()** function inside it > **UGameplayStatics::ApplyDamage()** triggers a Damage Event > the Multicast Delegate function **OnTakeAnyDamage**, in HealthComponent class, listens to this event and broadcasts the damage parameters to the Callback function **DamageTaken()**, also in the HealthComponent class, bound to it by **AddDynamic** > **DamageTaken()** Callback function updates the health variables declared in HealthComponent.h, decreasing the health of the damaged actors

### 4.2.1: Apply Damage OnHit and generate the Damage Event:

In Projectile.cpp define the damage event to be generated by the OnHit() Callback Function when it gets called by the Multicast Delegate OnComponentHit()
```cpp
void AProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Get the owner of this Projectile component and store it in a local AActor pointer
	AActor* MyOwner = GetOwner();
	if (MyOwner == nullptr)
	{
		Destroy();
		return;
	}
	
	// Access the instigator controller out of this AActor pointer and store in a local pointer AController
	AController* MyOwnerInstigator = MyOwner->GetInstigatorController();

	// Get the damage type - defines the particular type of damage that we are causing - and store it in a local pointer UClass
	UClass* DamageTypeClass = UDamageType::StaticClass();

	// Apply damage if the projectile hit another actor, not hit us. check if: (other actor is not null && Other actor is not = to this actor - not cause damage to ourselves && other actor not equals to my owner - not to cause damage to my owner)
	if (OtherActor && OtherActor != this && OtherActor != MyOwner)
	{
		// Call apply damage - this will generate the damage event
		UGameplayStatics::ApplyDamage(
			OtherActor, /* damaged actor */
			Damage, /* damage amount */ 
			MyOwnerInstigator, /* the controller of the actor who is causing damage */
			this, /* actor causing the damage */ 
			DamageTypeClass /* type of the class associated to the type of damage we are causing */
			);
	}
	
	// Destroy the projectile after it hits anything
	Destroy();
}
```

## 4.3: Create a new HealthComponent c++ class in Unreal derived from Actor Component. 

In HealhComponent.h create health variables 
```cpp
private:

	UPROPERTY(EditAnywhere) 
	float MaxHealth = 500.f;
	float Health = 0.f;
```

In HealthComponent.cpp Define and ininitialize the health variables
```cpp
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	Health = MaxHealth; 
}
```

Include our custom component in our Blueprint. In BP_PawnTank, event graph, HealthComponent and do the same for BP_PawnTurret.

Use UGameplayStatics::ApplyDamage to trigger the damage event > Use the OnTakeAnyDamage() Delegate Function to Broadcast the damage event > and call the DamageTaken() Callback Function to > do what the damage will cause to the actors' health.

## 4.4: Multicast Delegate:

In HealthComponent.cpp, Define the Multicast Delegate Function OnTakeAnyDamage and bind it to its Callback Function DamageTaken.
```cpp
void UHealthComponent::BeginPlay()
{
	// Access the owner of this component to bind the DamageTaken callback to the OnTakeAnyDamage delegate that is in the owner of HealthComponen. Returns a AActor pointer which is the component that owns this HealthComponent.
	GetOwner()->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::DamageTaken);
	
	// Get access to the gamemode. gamemode returns a AgamemodeBase variable and we are trying to store in a child class ToonTanksGameMode - so we need to cast to convert the AGameModeBase var into a ToonTanksGameMode var
	ToonTanksGameMode = Cast<AToonTanksGameMode>(UGameplayStatics::GetGameMode(this));
}
```

## 4.5: Callback Function:

In HealthComponent.h Declare the Callback Function DamageTaken()
```cpp
private:
	// My custom callback function
	UFUNCTION()
	void DamageTaken(
		AActor* DamagedActor, // actor who is taking the damage
		float Damage, // damage amount
		const UDamageType* DamageType, // custom damage types - fire, explosion etc - that have custom actions
		AController* Instigator, // Controler responsible for the damage - controller possessing the pawn causing the damage
		AActor* DamageCauser // actor causing the damage - in this case, the projectile or "this"
		);
```

In HealthComponent.cpp Define the Callback Function DamageTaken() that is bound to OnTakeAnyDamage() Multicast Delegate Function that owns this HealthComponent. Inside it specify what kind of damage it will cause to the Health variables. 
```cpp
void UHealthComponent::DamageTaken(AActor *DamagedActor, float Damage, const UDamageType *DamageType, AController *Instigator, AActor *DamageCauser)
{
	// specify the impact of the damage on the health variables
	if (Damage <= 0.f) return;

	Health -= Damage;
	UE_LOG(LogTemp, Warning, TEXT("vc foi atingido!!!!!. Health = %f"), Health);	
}
```


# 5: Game Rules and Game Mode.

The Game Mode will be responsible for defining winning, loosing and death conditions, starting and ending the game and defining who the player0 is (default pawn).


## 5.1: Player Controller

Implement our custom Player Controller class

This class will be used to define the mouse cursor. It will also be called from the GameMode when the actor dies to disable input.

### 5.1.1: Create a C++ class.
Create ToonTanksPlayerController c++ class. 

#### 5.1.1.1: Declare:
In ToonTanksPlayerController.h, Declare a function to switch whether inputs will be enabled or disabled.
```cpp
public:
	// set player enabled state to add inputs to move the tank
	void SetPlayerEnabledState(bool bPlayerEnabled);
```

#### 5.1.1.2: Define:
In ToonTanksPlayerController.cpp, Define this function.
```cpp
// if the player should be enabled, use EnableInput() to do it, else, use DisableInput() to disable it.
void AToonTanksPlayerController::SetPlayerEnabledState(bool bPlayerEnabled)
{
    if (bPlayerEnabled)
    {
        // access the pawn and enable or disable input
        GetPawn()->EnableInput(this);
    }
    else
    {
        GetPawn()->DisableInput(this);
    }

    // Disable or enable coursor based on whether or not the player should be enabled
    bShowMouseCursor = bPlayerEnabled;
}
```

### 5.1.2: Create a blueprint based on the C++ class
Create a BP_ToonTanksPlayerController blueprint based on this ToonTanksPlayerController class.

Open BP_ToonTanksPlayerController, in Class > PlayerController class: change the player controller to our custom BP_ToonTanksPlayerController.

In BP_ToonTanksPlayerController, Mouse interface > default mouse cursor > change it to crosshairs


## 5.2: GameMode: START

Define when the game starts and stops

### 5.2.1: Create a C++ class:
Create a new game mode class ToonTanksGameMode with Game Mode Base as parent class.

### 5.2.2: Create a blueprint based on this C++ class:
Create a new blueprint BP_ToonTanksGameMode based on this new class.

In Edit > Project settings > Maps and Modes > Default Modes > Default Game Mode, change the default game mode to BP_ToonTanksGameMode

In BP_ToonTanksGameMode > Classes > Default Pawn Class, change it to BP_PawnTank so that this will define the default pawn to be possessed when we start the game as being the tank - we will play as the tank actor

Select the tank actor, in its parameters Pawn > Auto Possess player selct Player0.

### 5.2.3: Declare the variables and functions

In ToonTanksGameMode.h, override BeginPlay() so that we can define out own actions when the game begins.
Declare a StartGame() function to allows us to define in which moment the game will start.
Declare a pointer variable that allows us to have access to the player controller.
Declare a variable to time untill the game starts.
Declare a function to define when the game starts.
Declare a function to count the number of enemies that are still alive.
Declare a variable to store the current number of enemies still alive.
```cpp
protected:
	// override begin play to store the tank pointer
	virtual void BeginPlay() override;

	// a blueprint implementable event funtion is a function that can be called from the code and have it's functionalities and behavior implemented in the blueprint
	UFUNCTION(BlueprintImplementableEvent)
	void StartGame();
	
private:
	// pointer var to access player controller
	class AToonTanksPlayerController* ToonTanksPlayerController;
	
	// timer that will count the amount of time until we can start moving
	float StartDelay = 3.f;
	
	// function to start timer
	void HandleGameStart();
	
	// track the number of towers
	int32 TargetTowers = 0;
	int32 GetTargetTowerCount();
```

### 5.2.4: Define the variables and functions

In ToonTanksGameMode.cpp 
Define the value of the player controller pointer.
Define when the game will start and what conditions need to be in place for this to happen.
Define a function to count all alive enemies in the world, call in on game start and pass the current result to a variable.
Set the timer.

#### (not implementing 164: the Start Game Widget, 165: Countdown Timer, 166: Display Countdown Time)

```cpp

void AToonTanksGameMode::BeginPlay()
{
    // Use super to access parent BeginPlay functionality 
    Super::BeginPlay();

    HandleGameStart();
}

void AToonTanksGameMode::HandleGameStart()
{

    //get tower count
    TargetTowers = GetTargetTowerCount();

    // poiter to APlayerPawn in a form of ATank pointer.
    Tank = Cast<ATank>(UGameplayStatics::GetPlayerPawn(this /*context*/, 0 /*player number*/));

    // store the player controller value inside my toon tanks player controller var 
    ToonTanksPlayerController = Cast<AToonTanksPlayerController>(UGameplayStatics::GetPlayerController(this, 0));

	// since I'm calling the function from inside another function that was already declared here, I don't need to use AToonTanksGameMode::
    StartGame();

    // if player controller is enabled (true)
    if (ToonTanksPlayerController)
    {
        // set it to false: restrict tanks movement
        ToonTanksPlayerController->SetPlayerEnabledState(false);

        // create a timer var from the type FTimerHandle
        FTimerHandle PlayerEnabledTimerHandle;

        // set a local variable for the timer Delegate so when a specif time delay has passed, this delegate function will call its callback function - SetPlayerEnabledSt passing in true as a parameter to enable player's movements when the timer is finished counting.
        FTimerDelegate PlayerEnabledTimerDelegate = FTimerDelegate::CreateUObject(
            ToonTanksPlayerController, /* user object that we are talking about here */
            &AToonTanksPlayerController::SetPlayerEnabledState, /* address of the callback function that will be called */
            true /* boolean value that will be passed back to the SetPlayerEnabledState callback function when it gets called in the end of the timer delay*/
        );
        // set my timer
        GetWorldTimerManager().SetTimer(PlayerEnabledTimerHandle, /* my timer handle var */
        PlayerEnabledTimerDelegate, /* my timer Delegate */
        StartDelay, /* float for the time delay */
        false /* false so that this timer doesn't loop */ 
        ); 
    }
    
// count all towers in the world
int32 AToonTanksGameMode::GetTargetTowerCount()
{
    TArray<AActor*> Towers;

    UGameplayStatics::GetAllActorsOfClass(
        this, /* world context object */
        ATower::StaticClass(), /* actor class type, ::StaticClass() is a function that returns the actor class */
        Towers /* the array that is going to be populated by this function - is an out parameter */
        );

    return Towers.Num();
}

```


## 5.3: GameMode: DEATH

- **HIT**: Projectile Component hits an Actor > it triggers a Hit Event > the Multicast Delegate function **OnComponentHit**, in the Projectile class, listens to this event and broadcasts **FHitResult** to the Callback Function **OnHit()**, also in the Projectile class, bound to it by **AddDynamic** 
   - **DAMAGE**: > the **OnHit()** Callback function will apply the damage using **UGamePlaystatics::ApplyDamage()** function inside it > **UGameplayStatics::ApplyDamage()** triggers a Damage Event > the Multicast Delegate function **OnTakeAnyDamage**, in HealthComponent class, listens to this event and broadcasts the damage parameters to the Callback function **DamageTaken()**, also in the HealthComponent class, bound to it by **AddDynamic** > **DamageTaken()** Callback function updates the health variables declared in HealthComponent.h, decreasing the health of the damaged actors
       - **DEATH**: > (If Health <= 0) > From inside **DamageTaken()** callback function, call the **ActorDied()** function in the ToonTanksGameMode class > From inside **ActorDied()**, call the **HandleDestruction()** function in the BasePawn class that defines what happens when the actor gets destroyed - special effects, particles, sound - and hides the actor from the game so that it is no longer visible.

### 5.3.1: ActorDied() function

In ToonTanksGameMode.h Declare the ActorDied() function. Add a Tank variable to check if the dead actor was the tank or the tower. Override BeginPlay().
```cpp
public:

// Actor died function. needs to be public because it will be called from the HealthComponent
	void ActorDied(AActor* DeadActor);

protected:

	// override begin play to store the tank pointer
	virtual void BeginPlay() override;

private:

	// variable to store the tank pointer 
	class ATank* Tank;
```

In ToonTanksGameMode.cpp 
Define ActorDied() function. If the actor who died was the tank (and not the tower) call the HandleDestruction() function.
Define BeginPlay() and call HandleGameStart() from whithin it
Define a HandleGameStart() function to define when the game can start (Timer), when the player can be enabled to move once the game starts and to initialize the ToonTanksPlayerController variable
```cpp
void AToonTanksGameMode::ActorDied(AActor* DeadActor)
{
    // check if the dead actor is the tank
    if (DeadActor == Tank)
    {
        Tank->HandleDestruction();

        // Check if player controller is valid
        if (ToonTanksPlayerController)
        {
            // disable input, disable mouse cursor
            ToonTanksPlayerController->SetPlayerEnabledState(false);
        }   
    }
    // if the dead actor is the tower instead of the tank
    else if (ATower* DestroyedTower = Cast<ATower>(DeadActor))  /*take the result of the DeadActor cast to ATower type and assign to a local variable in this condition*/
    {
        // if this cast is successful
        DestroyedTower->HandleDestruction();
    }
}
```

### 5.3.1: Call the ActorDied() function from the HealthComponent class to perform the death actions when Health reaches 0.

In HealthComponent.cpp, call ActorDied() from the DamageTaken() callback function to perform death when damage makes the Health variables reach 0.
```cpp
void UHealthComponent::DamageTaken(AActor *DamagedActor, float Damage, const UDamageType *DamageType, AController *Instigator, AActor *DamageCauser)
{
	// Check if health reached zero and the GameMode is valid and then call function actor died passing the actor that was hit
	if (Health <= 0.f && ToonTanksGameMode)
	{
		ToonTanksGameMode->ActorDied(DamagedActor);
		UE_LOG(LogTemp, Warning, TEXT("ator que morreu**********: %s"), *DamagedActor->GetName());
	}	
}
```

## 5.4: Define what actions happen when player gets destroyed.

In BasePawn.h, declare the HandleDestruction() function:
```cpp
public:
	// Create handle destruction. It will be called from a different class, ToonTanksGameMode class, so it needs to be public
	void HandleDestruction();
```
In BasePawn.cpp Define HandleDestruction() funtion:
```cpp
// defines what happens when the basepawn and its inherited classes die
void ABasePawn::HandleDestruction()
{
	// Show the explosion when the base pawn object dies
	
	
}
```
Implement HandleDestruction() in the Tower class.

In Tower.h, declare the HandleDestruction() function:
```cpp
public:
	// Create handle destruction. It will be called from a different class, ToonTanksGameMode class, so it needs to be public
	void HandleDestruction();
```
In Tower.cpp Define HandleDestruction() funtion and in it call Super::HandleDestruction() to inherit the implementations of this function from the BasePawn.
```cpp
// defines what happens when the basepawn and its inherited classes die
void ATower::HandleDestruction()
{
	Super::HandleDestruction();
	Destroy();
}
```

Implement HandleDestruction() in the tank class.

In Tank.h, declare the HandleDestruction() function and Declare a variable to store the tank player controller.
```cpp
public:
	// Create handle destruction. It will be called from a different class, ToonTanksGameMode class, so it needs to be public
	void HandleDestruction();
	
	// A Getter function to access the private variable player controller from ToonTanksGameMode
	// const because getter functions don't change any variables in this class, it only returns that private variable as it is
	APlayerController* GetTankPlayerController() const { return TankPlayerController; }
```
In Tank.cpp Define HandleDestruction() funtion and in it call Super::HandleDestruction() to inherit the implementations of this function from the BasePawn.
```cpp
// defines what happens when the basepawn and its inherited classes die
void ATank::HandleDestruction()
{
	Super::HandleDestruction();
	SetActorHiddenInGame(true);
	SetActorTickEnabled(false);
}
```

## 5.5: GameMode: GAME OVER / WIN - LOOSE

Define what happens when GameOver: either we destroy all enemies (Win Condition - GameOver = true) or we get destroyed (Loose Condition - GameOver = false)

### 5.5.1: Declare variables:

In ToonTanksGameMode.h
Create a blueprint implementable event so that we can communicate our c++ code with our blueprints and show a Game Over widget on the scree. 
```cpp
protected: 
	// function for winning the game
	UFUNCTION(BlueprintImplementableEvent)
	void GameOver(bool bWonGame);

```

### 5.5.2: Define functions

In ToonTanksGameMode.cpp 
Call the GameOver function when our actor dies or when we have destroyed all enemies
```cpp
void AToonTanksGameMode::ActorDied(AActor* DeadActor)
{
    // check if the dead actor is the tank
    if (DeadActor == Tank)
    {
        Tank->HandleDestruction();

        // Check if player controller is valid
        if (ToonTanksPlayerController)
        {
            // disable input, disable mouse cursor
            ToonTanksPlayerController->SetPlayerEnabledState(false);
        }   
	
        // If tank gets destroyed, call GameOver funtion with the loosing condition (false)
        GameOver(false);
    }
    // if the dead actor is the tower instead of the tank
    else if (ATower* DestroyedTower = Cast<ATower>(DeadActor))  /*take the result of the DeadActor cast to ATower type and assign to a local variable in this condition*/
    {
        // if this cast is successful
        DestroyedTower->HandleDestruction();

        // Decrement towers count everytime a tower dies and if it reaches 0 call GameOver with a winning condition (true)
        --TargetTowers;
        // end game when all towers are killed
        if (TargetTowers == 0)
        {
            GameOver(true);
        }
    }
}
```

### 5.5.3: Create the GameOver blueprint implementable event and show the widget

Create a new widget blueprint: in the unreal engine > create a widget folter > right click > user interface > widget blueprint. call it WBP_EngGameWidget.
Include a display text component and set it to "is variable".

![image](https://user-images.githubusercontent.com/12215115/158021451-b0af04f8-3ca0-4c7b-9fa5-58317dade81a.png)

in BP_ToonTanksGameMode, Event Graph, 

Create an event game over, link it to Create WPB End Game Widget and ourput Set Text. Use select to show "You Won" or "You Lost" depending upon the boolean variable value that is coming out of event gameover: 

![image](https://user-images.githubusercontent.com/12215115/158021553-acd3cce7-54cb-422a-be3e-dc0857058fba.png)


# 6: Special Effects


## 6.1: Particle Systems

### 6.1.1: Declare variables

In Projectile.h, Declare pointer variables of the type particle system
```cpp
private: 
	// particle syst var
	UPROPERTY(EditAnywhere, Category = "Combat")
	class UParticleSystem* HitParticles;

	// particle system component
	UPROPERTY(EditAnywhere, Category = "Combat")
	class UParticleSystemComponent* TrailParticles;
```

In BasePawn.h
```cpp
private:
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	class UParticleSystem* DeathParticles;
```

### 6.1.2: Spawn Particles

In Projectile.cpp, Spawn the particle when we hit something, inside the apply damage if statement created previously
```cpp
void AProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{

	if (OtherActor && OtherActor != this && OtherActor != MyOwner)
	{
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
}
```

In Projectile.cpp, in the constructor function, construct the smoke trail particle variable and attach it to the root component so that it follows the projectile around.
```cpp
AProjectile::AProjectile()
{
	// Construct the particle system that will be the smoke trail of the projectile
	TrailParticles = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Smoke Trail"));
	// Attach to the projectile root component
	TrailParticles->SetupAttachment(RootComponent);
}
```

In BasePawn.cpp, spawn the death particles emitter when the actor gets destroyed
```cpp
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
}
```

### 6.1.3: Set the particle systems in the Blueprints

In BP_Projectile > select BP_Projectile > in our HitParticles variable field > select the particles we are going to use from the drop down.

In BP_Projectile > Event Grapgh > select trail particles > details > Particles > Template variable > select projectile trail

In BP_PawnTank > select BP_PawnTank(self) > in Details > Combat > Death Particles > select the particles mesh we are going to use

In BP_PawnTurret > select BP_PawnTurret(self) > in Details > Combat > Death Particles > select the particles mesh we are going to use

## 6.2: Sounds

### 6.2.1: Declare Variables

In Projectile.h, Declare the sound pointer variables of type USoundBase*
```cpp
private:
	// var for sound
	UPROPERTY(EditAnywhere, Category = "Combat")
	USoundBase* LaunchSound;

	// var for sound
	UPROPERTY(EditAnywhere, Category = "Combat")
	USoundBase* HitSound;
```

In BasePawn.h, declare a sound for when our any of the actors die
```cpp
private:
	// var for sound
	UPROPERTY(EditAnywhere, Category = "Combat")
	class USoundBase* DeathSound;
```

### 6.2.2: Play Sounds when actors get hit or die


******************* PAREI 172: 5:00 *******************






### 6.2.3: Set the sound components in the Blueprints

In BP_Projectile > Event Grapgh > select BP_Projectile(self) > Details > Combat > HitSound, LaunchSound > select the sound component from the dropdown menu

In BP_PawnTank > Event Grapgh > select BP_PawnTank(self) > Details > Combat > DeathSound > select the sound component from the dropdown menu

In BP_PawnTurret > Event Grapgh > select BP_PawnTurret(self) > Details > Combat > DeathSound > select the sound component from the dropdown menu
