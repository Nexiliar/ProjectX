// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectX/Items/PickUpActor.h"

// Sets default values
APickUpActor::APickUpActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshForItem"));
	StaticMesh->SetGenerateOverlapEvents(true);
	StaticMesh->SetSimulatePhysics(true);	
	StaticMesh->SetEnableGravity(true);
	StaticMesh->SetCollisionProfileName(TEXT("Query and Physics"));
	RootComponent = StaticMesh;


	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	BoxComponent->SetupAttachment(RootComponent);


	CloseToPickUp = CreateDefaultSubobject<UMaterialInstance>(TEXT("On Material"));
	GetOutOfWeapon= CreateDefaultSubobject<UMaterialInstance>(TEXT("Off Material"));;
}

// Called when the game starts or when spawned
void APickUpActor::BeginPlay()
{
	Super::BeginPlay();

	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &APickUpActor::CollisionBoxBeginOverlap);
	BoxComponent->OnComponentEndOverlap.AddDynamic(this, &APickUpActor::CollisionBoxEndOverlap);

	StaticMesh->OnComponentBeginOverlap.AddDynamic(this, &APickUpActor::StaticMeshBeginOverlap);

	//Инициализируем переменную Character
	Character = Cast<AProjectXCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	Character->OnIterractButtonPressed.AddDynamic(this, &APickUpActor::TryToPickUpItem);

	InitBackPack(Inventory);

	
}

// Called every frame
void APickUpActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}




void APickUpActor::CollisionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AProjectXCharacter*  CharacterP = Cast<AProjectXCharacter>(OtherActor);		
		if (CharacterP && CharacterP->InventoryComponent)
		{
			isOverlapping = true;
			OverlapStart_BP(isOverlapping);
		}		
}

void APickUpActor::CollisionBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AProjectXCharacter* CharacterP = Cast<AProjectXCharacter>(OtherActor);

//	CharacterP->OnIterractButtonPressed.Clear();

	isOverlapping = false;
	OverlapStart_BP(isOverlapping);
}

void APickUpActor::StaticMeshBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AProjectXCharacter* CharacterP = Cast<AProjectXCharacter>(OtherActor);
	if (CharacterP && CharacterP->InventoryComponent)
	{
		if (Inventory.IsWeapon)
		{
		}
		else
		{
			int32 RestItems;
			bool EmptySlotisFind = false;
			bool FreeStackisFind = false;
			CharacterP->InventoryComponent->SearchEmptySlotIndex(EmptySlotisFind);
			
			if (EmptySlotisFind)
			{
				if (Inventory.ItemsInfo.Itemtype != EItemType::Equipment)
				{
					if (CharacterP->InventoryComponent->AddItem(Inventory, AmountItemsTospawn, RestItems))
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
				else
				{

				}
				
			}
			else
			{
				if (CharacterP->InventoryComponent->SearchFreeStack(Inventory.ItemsInfo, FreeStackisFind))
				{
					if (CharacterP->InventoryComponent->AddItem(Inventory, AmountItemsTospawn, RestItems))
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
}

void APickUpActor::TryToPickUpItem()
{
	Character = Cast<AProjectXCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	int32 FreeSlotIndex = 0;
	bool isFreeSlot = Character->InventoryComponent->CheckCanTakeWeapon(FreeSlotIndex);
	if (isOverlapping)
	{
		EEquipmentSlotType SlotType = Inventory.EquipmentInfo.SlotType;
		switch (SlotType)
		{
		case EEquipmentSlotType::None:
			break;
		case EEquipmentSlotType::Weapon:
			break;
		case EEquipmentSlotType::Bracer:
			break;
		case EEquipmentSlotType::BodyKit:
			break;
		case EEquipmentSlotType::Armor:
			break;
		case EEquipmentSlotType::BackPack:
			EquipBackPack();
			this->Destroy();
			break;
		default:
			break;
		}
		/* 
		if (Inventory.IsWeapon)
		{
			if (isFreeSlot)
			{
				//Character->InventoryComponent->TryGetWeaponToInventory(WeaponInfo); rework whole logic
				this->Destroy();
			}
			else
			{
				int32 CurrentIndexSLot = 0;
				Character->GetCurrentSlot(CurrentIndexSLot);
				//	if (Character->InventoryComponent->SwitchWeaponToInventory(WeaponOnGroundInfo.WeaponInfo, CurrentIndexSLot, CurrentIndexSLot, WeaponOnGroundInfo))
				{
					//DropWeaponOnSwitch(WeaponOnGroundInfo);				
				}
			}
		}
		else
		{
			EquipBackPack();
			//Need rework pickupsystem
			bool EmptySlotisFind = false;
			int32 RestItems;
			Character->InventoryComponent->SearchEmptySlotIndex(EmptySlotisFind);
			if (EmptySlotisFind)
			{
				//UE_LOG(LogTemp, Warning, TEXT(" APickUpActor::TryToPickUpItem"));
				if (Character->InventoryComponent->AddItem(Inventory, AmountItemsTospawn, RestItems))
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
		}*/
	}	
}
void APickUpActor::InitBackPack(FInventory CurrentItemInfo)
{
	if (CurrentItemInfo.EquipmentInfo.SlotType == EEquipmentSlotType::BackPack)
	{
		if (CurrentItemInfo.ItemsInfo.ItemName == "LightBackPack")
		{
			SetBackPackSlotsAmount(6);
		}
		else if (CurrentItemInfo.ItemsInfo.ItemName == "MediumBackpack")
		{
			SetBackPackSlotsAmount(10);
		}
		else if (CurrentItemInfo.ItemsInfo.ItemName == "HugeBackPack")
		{
			SetBackPackSlotsAmount(15);
		}

		ERarity ItemRarity = CurrentItemInfo.EquipmentInfo.ItemRarity;
		switch (ItemRarity)
		{
		case ERarity::None:
			break;
		case ERarity::Common:
			SetBackPackSlotsAmount(GetBackPackSlotsAmount() + 1);
			break;
		case ERarity::Uncommon:
			SetBackPackSlotsAmount(GetBackPackSlotsAmount() + 2);
			break;
		case ERarity::Rare:
			SetBackPackSlotsAmount(GetBackPackSlotsAmount() + 5);
			break;
		case ERarity::Epic:
			SetBackPackSlotsAmount(GetBackPackSlotsAmount() + 10);
			break;
		case ERarity::Legendary:
			SetBackPackSlotsAmount(GetBackPackSlotsAmount() + 20);
			break;
		default:
			break;
		}
		InventorySlots.SetNum(GetBackPackSlotsAmount());
		UE_LOG(LogTemp, Warning, TEXT(" BackPackInitialized"));
	}
	
}
int32 APickUpActor::GetBackPackSlotsAmount()
{
	return BackPackSlotAmount;
}
void APickUpActor::SetBackPackSlotsAmount(int32 AmountOfSlots)
{
	BackPackSlotAmount = AmountOfSlots;
}
bool APickUpActor::EquipBackPack()
{
	bool bIsBackPackEquipSuccess = false;
	UProjectXInventoryComponent* myInventory = Cast<UProjectXInventoryComponent>(Character->GetComponentByClass(UProjectXInventoryComponent::StaticClass()));
	myInventory->EquipBackPack(Inventory, InventorySlots,GetBackPackSlotsAmount(), bIsBackPackEquipSuccess);
	return bIsBackPackEquipSuccess;
}
void APickUpActor::OverlapStart_BP_Implementation(bool isOverlaping)
{

}
 /* Метод дропа оружия, после подбора. Подумать о реворке.
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
		
			//DropedWeapon->SkeletalMesh->SetSkeletalMesh(DroppedItemCFG.WeaponSkeletalMesh);
			DropedWeapon->StaticMesh->SetStaticMesh(DroppedItemCFG.WeaponStaticMesh);
			DropedWeapon->WeaponOnGroundInfo.WeaponSkeletalMesh = DroppedItemCFG.WeaponSkeletalMesh;
			DropedWeapon->WeaponOnGroundInfo.WeaponStaticMesh = DroppedItemCFG.WeaponStaticMesh;
				
			DropedWeapon->isOverlapping = true;
			DropedWeapon->isWeapon = true;
			DropedWeapon->ActorForPickUp = this->GetClass();
			//UMaterialInterface* Temp  = DropedWeapon->SkeletalMesh->GetMaterial(0);
			
			//DropedWeapon->SkeletalMesh->SetMaterial(0, CloseToPickUp);
			
		
			Destroy();
			}
		}
		else
		{
			
		
		
		}
	 

	
}
*/


/*
int32 ABackPackActor::GetBackPackSlotsAmount()
{
	return BackPackSlotAmount;
}

void ABackPackActor::SetBackPackSlotsAmount(int32 AmountOfSlots)
{
	BackPackSlotAmount = AmountOfSlots;
}


void ABackPackActor::InitObject(FInventory CurrentItemInfo)
{
	Super::InitObject(CurrentItemInfo);
	UE_LOG(LogTemp, Warning, TEXT(" BackPackInitialized"));
	if (CurrentItemInfo.EquipmentInfo.SlotType == EEquipmentSlotType::BackPack)
	{
		if (CurrentItemInfo.ItemsInfo.ItemName == "LightBackPack")
		{
			SetBackPackSlotsAmount(6);
		}
		else if (CurrentItemInfo.ItemsInfo.ItemName == "MediumBackpack")
		{
			SetBackPackSlotsAmount(10);
		}
		else if (CurrentItemInfo.ItemsInfo.ItemName == "HugeBackPack")
		{
			SetBackPackSlotsAmount(15);
		}
	}
	ERarity ItemRarity = CurrentItemInfo.EquipmentInfo.ItemRarity;
	switch (ItemRarity)
	{
	case ERarity::None:
		break;
	case ERarity::Common:
		break;
	case ERarity::Uncommon:
		
		break;
	case ERarity::Rare:
		SetBackPackSlotsAmount(GetBackPackSlotsAmount() + 5);
		break;
	case ERarity::Epic:
		SetBackPackSlotsAmount(GetBackPackSlotsAmount() + 10);
		break;
	case ERarity::Legendary:
		SetBackPackSlotsAmount(GetBackPackSlotsAmount() + 20);
		break;
	default:
		break;
	}
	InventorySlots.SetNum(GetBackPackSlotsAmount());
	UE_LOG(LogTemp, Warning, TEXT(" BackPackInitialized"));
}

bool ABackPackActor::EquipBackPack()
{
	return false;
}

*/
