
# 3rdPersonShooter
This is a tank battle 3rd person shooter game. It uses Unreal Engine 4.24 and code in C++. It is based on a Udemy tutorial from GameDev.tv and Unreal.

## Project Steps:
1. Create Components (Parent Classes: BasePawn)
2. Create Sub-Components (Child Classes: Tank, Tower, Projectile)
3. Actions: Set Uset Input and Game Controllers
4. Game Logic and Rules (Hit Events, Health Component, Apply Damage, Death, Winning, Loosing)

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

### 2.1: Tank Class:

The tank will be our player
Attach the Camera and Sprint arm to the Tank to control the 3rd person view
  Unreal Engine option: open BP_PawnTank, click on AddComponent button on the view port and add a Sprint Arm and a Camera attached to it
  c++ option:
  
#### 2.1.1: Declare Component Variables:
  
```cpp
private:

	UPROPERTY(VisibleAnywhere, Category = "Components")
	class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	class UCameraComponent* Camera;
```

#### 2.1.2: Construct Component Objects:

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

### 2.2: Tower Class

Create Tower Class derived from the BasePawn Class. 

On BP_PawnTurret change its parent class to Tower so that the Turret properties inherit those of the Tower i.e. turning.

### 2.3: Projectile Class:

Create a projectile c++ class with Actor as parent.

#### 2.3.1: Declare Component Variables.

In Projectile.h
```cpp
private:
	// 1- Create a UStaticMesh component variable
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	UStaticMeshComponent* ProjectileMesh;
```

#### 2.3.2: Construct Component Objects.
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


## 3: Actions: Set User Input and Game Controllers

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
### 3.3: Fire Function:

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
#### 3.3.1: Set the Projectile movement: 

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

### 3.4: Make the Tank turret mesh follow the mouse cursor to fire at that direction (get the hit result under the cursor)

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

### 3.5: Implement the Fire function for the Towers (including a timer)

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


## 4. Game Logic and Rules

Projectile Component hits an Actor > it triggers a Hit Event > the Multicast Delegate function OnComponentHit, in the Projectile class, listens to this event and broadcasts FHitResult to the Callback Function OnHit() bound to it by AddDynamic > the OnHit() Callback function will apply the damage using UGamePlaystatics::ApplyDamage() function > UGameplayStatics::ApplyDamage() triggers a Damage Event > the Multicast Delegate function OnTakeAnyDamage, in HealthComponent class, listens to this event and broadcasts the damage parameters to the Callback function DamageTaken() bound to it by AddDynamic > DamageTaken() Callback function updates the HealthComponent variables decreasing the health of the damaged actors.

### 4.1: Hit event

Logic sequence: Projectile mesh hits some actor in the world > generates a Hit Event > Multicast Delegate - OnComponentHit() - calls the Callback functions subscribed to its invocation list - OnHit() - and broadcasts the FHitResult > Callback functions will execute whatever actions were defined inside them.

#### 4.1.1: Multicast Delegate:

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

#### 4.1.2: Callback Function

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

### 4.2: Damage Event: 

#### 4.2.1: Apply Damage OnHit and generate the Damage Event:

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

#### 4.1: Create a new HealthComponent c++ class in Unreal derived from Actor Component. 

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

#### 4.3: Multicast Delegate:

In HealthComponent.cpp, Define the Multicast Delegate Function OnTakeAnyDamage and bind it to its Callback Function DamageTaken.
```cpp
void UHealthComponent::BeginPlay()
{
	// Access the owner of this component to bind the DamageTaken callback to the OnTakeAnyDamage delegate that is in the owner of HealthComponen. Returns a AActor pointer which is the component that owns this HealthComponent.
	GetOwner()->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::DamageTaken);
}
```

#### 4.3: Callback Function:

In HealthComponent.h Declare the Callback Function.
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

In HealthComponent.cpp Define the Callback Function that is bound to OnTakeAnyDamage() Multicast Delegate Function that owns this HealthComponent. Inside it specify what kind of damage it will cause to the Health variables. 
```cpp
void UHealthComponent::DamageTaken(AActor *DamagedActor, float Damage, const UDamageType *DamageType, AController *Instigator, AActor *DamageCauser)
{
	// specify the impact of the damage on the health variables
	if (Damage <= 0.f) return;

	Health -= Damage;
	UE_LOG(LogTemp, Warning, TEXT("vc foi atingido!!!!!. Health = %f"), Health);	
}
```



















