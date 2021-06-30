// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectX/Items/PickUpActor.h"

// Sets default values
APickUpActor::APickUpActor()
{
	

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Collision Sphere"));
	SphereComponent->SetSphereRadius(90);
	RootComponent = SphereComponent;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshForItem"));
	StaticMesh->SetGenerateOverlapEvents(false);
	StaticMesh->SetCollisionProfileName(TEXT("PhysicsOnly"));
	StaticMesh->SetupAttachment(RootComponent);

	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshForItem"));
	SkeletalMesh->SetGenerateOverlapEvents(false);
	SkeletalMesh->SetCollisionProfileName(TEXT("PhysicsOnly"));
	SkeletalMesh->SetSimulatePhysics(true);
	SkeletalMesh->SetupAttachment(RootComponent);
	
	/* 
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshForItem"));
	StaticMesh->SetGenerateOverlapEvents(false);
	StaticMesh->SetCollisionProfileName(TEXT("PhysicsOnly"));
	RootComponent = StaticMesh;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Collision Sphere"));
	SphereComponent->SetSphereRadius(90);
	//SphereComponent->SetupAttachment(RootComponent);
	
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshForItem"));
	SkeletalMesh->SetGenerateOverlapEvents(false);
	SkeletalMesh->SetCollisionProfileName(TEXT("PhysicsOnly"));
	//SkeletalMesh->SetupAttachment(RootComponent);
	//SkeletalMesh->SetSimulatePhysics(true);
	*/
	CloseToPickUp = CreateDefaultSubobject<UMaterialInstance>(TEXT("On Material"));
	GetOutOfWeapon= CreateDefaultSubobject<UMaterialInstance>(TEXT("Off Material"));;
}

// Called when the game starts or when spawned
void APickUpActor::BeginPlay()
{
	Super::BeginPlay();
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &APickUpActor::CollisionSphereBeginOverlap);
	SphereComponent->OnComponentEndOverlap.AddDynamic(this, &APickUpActor::CollisionSphereEndOverlap);
	

	//Инициализируем переменную Character
	Character = Cast<AProjectXCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	Character->OnIterractButtonPressed.AddDynamic(this, &APickUpActor::TryToPickUpItem);
	


}

// Called every frame
void APickUpActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APickUpActor::CollisionSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AProjectXCharacter*  CharacterP = Cast<AProjectXCharacter>(OtherActor);
	
	//UE_LOG(LogTemp, Warning, TEXT("CollisionSphereBeginOverlap"));

		if (CharacterP && CharacterP->InventoryComponent)
		{
			
			if (isWeapon)
			{
				isOverlapping = true;								
			}
			else
			{								
					int32 RestItems;
					bool isFind = false;
					CharacterP->InventoryComponent->SearchEmptySlotIndex(isFind);
					if(isFind)
					{
						if (CharacterP->InventoryComponent->AddItem(MasterItem, AmountItemsTospawn, NameOfTheItem, RestItems))
						{

							if (RestItems > 0)
							{
								AmountItemsTospawn = RestItems;
							}
							else
							{
								this->Destroy();
							}
						}
					}
				
				
			}
		}
		
}

void APickUpActor::CollisionSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AProjectXCharacter* CharacterP = Cast<AProjectXCharacter>(OtherActor);

//	CharacterP->OnIterractButtonPressed.Clear();

	isOverlapping = false;
	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("U STOPED OVERLAPING"));
}

void APickUpActor::TryToPickUpItem()
{

	Character = Cast<AProjectXCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	int32 FreeSlotIndex = 0;
	bool isFreeSlot = Character->InventoryComponent->CheckCanTakeWeapon(FreeSlotIndex);
	if (isOverlapping)
	{
		if (isFreeSlot)
		{
			

			Character->InventoryComponent->TryGetWeaponToInventory(WeaponOnGroundInfo.WeaponInfo);
			this->Destroy();
		}
		else
		{
			int32 CurrentIndexSLot = 0;
			Character->GetCurrentSlot(CurrentIndexSLot);
			if (Character->InventoryComponent->SwitchWeaponToInventory(WeaponOnGroundInfo.WeaponInfo, CurrentIndexSLot, CurrentIndexSLot, WeaponOnGroundInfo))
			{

				DropWeaponOnSwitch(WeaponOnGroundInfo);
				
			}

			
			
			

		}
	}
	
}

void APickUpActor::DropWeaponOnSwitch(FDropItem DroppedItemCFG)
{

	if (ActorForPickUp)
	{

		FVector SpawnLocation = StaticMesh->GetComponentLocation();
		FRotator SpawnRotation = StaticMesh->GetComponentRotation();

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		SpawnParams.Owner = GetOwner();
		SpawnParams.Instigator = GetInstigator();
	
	
		
	
		APickUpActor* DropedWeapon = GetWorld()->SpawnActor<APickUpActor>(ActorForPickUp, SpawnLocation, SpawnRotation, SpawnParams);
		if (DropedWeapon)
		{

	

			DropedWeapon->WeaponOnGroundInfo = DroppedItemCFG;
		
			DropedWeapon->SkeletalMesh->SetSkeletalMesh(DroppedItemCFG.WeaponSkeletalMesh);
			DropedWeapon->StaticMesh->SetStaticMesh(DroppedItemCFG.WeaponStaticMesh);
			DropedWeapon->WeaponOnGroundInfo.WeaponSkeletalMesh = DroppedItemCFG.WeaponSkeletalMesh;
			DropedWeapon->WeaponOnGroundInfo.WeaponStaticMesh = DroppedItemCFG.WeaponStaticMesh;
				
			DropedWeapon->isOverlapping = true;
			DropedWeapon->isWeapon = true;
			DropedWeapon->ActorForPickUp = this->GetClass();
			UMaterialInterface* Temp  = DropedWeapon->SkeletalMesh->GetMaterial(0);
			
			DropedWeapon->SkeletalMesh->SetMaterial(0, CloseToPickUp);
			
		
			Destroy();
			}
		}
		else
		{
			
		
		
		}
	

	
}





