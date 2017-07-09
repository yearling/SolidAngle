# UEGamePlayFrame

## AActor
	UCLASS()
	class AMyActor : public AActor
	{
	    GENERATED_BODY()
	
	    // Called at start of game.
	    void BeginPlay();
	
	    // Called when destroyed.
	    void EndPlay(const EEndPlayReason::Type EndPlayReason);
	
	    // Called every frame to update this actor.
	    void Tick(float DeltaSeconds);
	};

## UComponent
	UCLASS()
	class UMyComponent : public UActorComponent
	{
	    GENERATED_BODY()
	
	    // Called after the owning Actor was created
	    void InitializeComponent();
	
	    // Called when the component or the owning Actor is being destroyed
	    void UninitializeComponent();
	
	    // Component version of Tick
	    void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction);
	};

## Spawning Actor 
1. UMyObject* NewObj = NewObject<UMyObject>(); 
2. Spawn from world
  
		AMyActor* CreateCloneOfMyActor(AMyActor* ExistingActor, FVector SpawnLocation, FRotator SpawnRotation)
		{
		    UWorld* World = ExistingActor->GetWorld();
		    FActorSpawnParameters SpawnParams;
		    SpawnParams.Template = ExistingActor;
		    World->SpawnActor<AMyActor>(ExistingActor->GetClass(), SpawnLocation, SpawnRotation, SpawnParams);
		} 

## CreateComponent 

	UCLASS()
	class AMyActor : public AActor
	{
	    GENERATED_BODY()
	    UPROPERTY()
	    USphereComponent* MyCollisionComp;
	    AMyActor()
	    {
	        MyCollisionComp = CreateDefaultSubobject<USphereComponent>(FName(TEXT("CollisionComponent"));
	        MyCollisionComp->RelativeLocation = FVector::ZeroVector;
	        MyCollisionComp->SphereRadius = 20.0f;
	    }
	};

## Cast
	UPrimitiveComponent* Primitive = MyActor->GetComponentByClass(UPrimitiveComponent::StaticClass());
	USphereComponent* SphereCollider = Cast<USphereComponent>(Primitive);
	if (SphereCollider != nullptr)
	{
	        // ...
	}

## Destroy
	1. 	MyActor->Destroy();
	2. 	MyActor->SetLifeSpan(1); //延迟1s
	3.   
		// Hides visible components
		MyActor->SetActorHiddenInGame(true);
		
		// Disables collision components
		MyActor->SetActorEnableCollision(false);
		
		// Stops the Actor from ticking
		MyActor->SetActorTickEnabled(false);

## Accessing the Actor from Component 
	 AActor* ParentActor = 
	 MyComponent->GetOwner();  

## Accessing a Component from the Actor  
	UMyComponent* MyComp = MyActor->FindComponentByClass<UMyComponent>(); 

## Finding Actors
	// Find Actor by name (also works on UObjects)
	AActor* MyActor = FindObject<AActor>(nullptr, TEXT("MyNamedActor"));
	
	// Find Actors by type (needs a UWorld object)
	for (TActorIterator<AMyActor> It(GetWorld()); It; ++It)
	{
	        AMyActor* MyActor = *It;
	        // ...
	}  

	// Find UObjects by type
	for (TObjectIterator<UMyObject> It; It; ++it)
	{
	    UMyObject* MyObject = *It;
	    // ...
	}
	
	// Find Actors by tag (also works on ActorComponents, use TObjectIterator instead)
	for (TActorIterator<AActor> It(GetWorld()); It; ++It)
	{
	    AActor* Actor = *It;
	    if (Actor->ActorHasTag(FName(TEXT("Mytag"))))
	    {
	        // ...
	    }
	} 

## Adding tags to Actors 
	// Actors can have multiple tags
	MyActor.Tags.AddUnique(TEXT("MyTag")); 

## Adding tags to ActorComponents 
	// Components have their own array of tags
	MyComponent.ComponentTags.AddUnique(TEXT("MyTag"));  
  
## Comparing tags on Actors and ActorComponents 
	// Checks the tag on the GameObject it is attached to
	if (MyComponent.CompareTag("MyTag"))
	{
	    // ...
	}
	
	// Checks if an Actor has this tag
	if (MyActor->ActorHasTag(FName(TEXT("MyTag"))))
	{
	    // ...
	}
	// Checks if an ActorComponent has this tag
	if (MyComponent->ComponentHasTag(FName(TEXT("MyTag"))))
	{
	    // ...
	}

## RayTrace
	APawn* AMyPlayerController::FindPawnCameraIsLookingAt()
	{
	    // You can use this to customize various properties about the trace
	    FCollisionQueryParams Params;
	    // Ignore the player's pawn
	    Params.AddIgnoredActor(GetPawn());
	
	    // The hit result gets populated by the line trace
	    FHitResult Hit;
	
	    // Raycast out from the camera, only collide with pawns (they are on the ECC_Pawn collision channel)
	    FVector Start = PlayerCameraManager->GetCameraLocation();
	    FVector End = Start + (PlayerCameraManager->GetCameraRotation().Vector() * 1000.0f);
	    bool bHit = GetWorld()->LineTraceSingle(Hit, Start, End, ECC_Pawn, Params);
	
	    if (bHit)
	    {
	        // Hit.Actor contains a weak pointer to the Actor that the trace hit
	        return Cast<APawn>(Hit.Actor.Get());
	    }
	
	    return nullptr;
	}

## Trigger

	UCLASS()
	class AMyActor : public AActor
	{
	    GENERATED_BODY()
	
	    // My trigger component
	    UPROPERTY()
	    UPrimitiveComponent* Trigger;
	
	    AMyActor()
	    {
	        Trigger = CreateDefaultSubobject<USphereComponent>(TEXT("TriggerCollider"));
	
	        // Both colliders need to have this set to true for events to fire
	        Trigger.bGenerateOverlapEvents = true;
	
	        // Set the collision mode for the collider
	        // This mode will only enable the collider for raycasts, sweeps, and overlaps
	        Trigger.SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	    }
	
	    virtual void NotifyActorBeginOverlap(AActor* Other) override;
	
	    virtual void NotifyActorEndOverlap(AActor* Other) override;
	};

## KinematicRigidbodies
	UCLASS()
	class AMyActor : public AActor
	{
	    GENERATED_BODY()
	
	    UPROPERTY()
	    UPrimitiveComponent* PhysicalComp;
	
	    AMyActor()
	    {
	        PhysicalComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionAndPhysics"));
	        PhysicalComp->SetSimulatePhysics(false);
	        PhysicalComp->SetPhysicsLinearVelocity(GetActorRotation().Vector() * 100.0f);
	    }
	};  

## Input
	UCLASS()
	class AMyPlayerController : public APlayerController
	{
	    GENERATED_BODY()
	
	    void SetupInputComponent()
	    {
	        Super::SetupInputComponent();
	
	        InputComponent->BindAction("Fire", IE_Pressed, this, &AMyPlayerController::HandleFireInputEvent);
	        InputComponent->BindAxis("Horizontal", this, &AMyPlayerController::HandleHorizontalAxisInputEvent);
	        InputComponent->BindAxis("Vertical", this, &AMyPlayerController::HandleVerticalAxisInputEvent);
	    }
	
	    void HandleFireInputEvent();
	    void HandleHorizontalAxisInputEvent(float Value);
	    void HandleVerticalAxisInputEvent(float Value);
	};